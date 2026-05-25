/**
 * @file network.c
 * @brief ESPNOW connection 
 * 
 * Initiate NVS, WiFi and ESPNOW
 * ESPNOW Receiving callback, handles receiving data and send to gui to be displayed
 */

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "esp_now.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_now.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include "esp_event.h"

#include "network.h"
#include "sensor_data.h"
#include "gui.h"

static const char *TAG = "network";

// MAC addresses
// Device's own MAC address
static uint8_t s_device_mac[ESP_NOW_ETH_ALEN];

// Queue object
QueueHandle_t g_espnow_queue;

extern sensor_data_t g_nodes[6];

// ─────────────────────────────────────────────────────────────
// GET MAC ADDRESS
// ─────────────────────────────────────────────────────────────
// Function to dynamically get the MAC address of the esp32 and store into an array s_device_mac
static esp_err_t get_mac(void)
{   
    ESP_LOGI(TAG, "Read device MAC address");
    esp_read_mac(s_device_mac, ESP_MAC_WIFI_STA);

    ESP_LOGI(TAG, "MAC Address: %02X, %02X, %02X, %02X, %02X, %02X\n", 
        s_device_mac[0], s_device_mac[1], s_device_mac[2], 
        s_device_mac[3], s_device_mac[4], s_device_mac[5]);

    return ESP_OK;
}

// ─────────────────────────────────────────────────────────────
// NVS INITIATION
// ─────────────────────────────────────────────────────────────
static esp_err_t nvs_init(void)
{
    ESP_LOGI(TAG, "Initialize NVS");
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    return ESP_OK;
}

// ─────────────────────────────────────────────────────────────
// WIFI INITIATION
// ─────────────────────────────────────────────────────────────
static esp_err_t wifi_init(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    wifi_init_config_t wifi_cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(ESPNOW_WIFI_MODE));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_set_channel(ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE));
    ESP_LOGI(TAG, "Wifi init using channel %d", ESPNOW_CHANNEL);
    
#if CONFIG_ESPNOW_ENABLE_LONG_RANGE
    ESP_ERROR_CHECK(esp_wifi_set_protocol(ESP_WIFI_IF, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N|WIFI_PROTOCOL_LR));
#endif

    return ESP_OK;
}

// ─────────────────────────────────────────────────────────────
// ESPNOW RECEIVE CALL BACK
// ─────────────────────────────────────────────────────────────
// Function for handling when receiving ESPNOW data
static void espnow_recv_cb(const esp_now_recv_info_t *recv_info, const uint8_t *data, int data_len)
{
    // Handle data length error
    if (data_len != sizeof(sensor_data_t)) {
        ESP_LOGE(TAG, "Unexpected data length: %d", data_len);
        return;
    }

    // When data is received from ESPNOW, send it to the Queue
    if (xQueueSend(g_espnow_queue, data, 0) != pdTRUE) {
        ESP_LOGW(TAG, "Queue full, dropping data");
    }

    // This function is for debugging in ESP LOGI, uncomment to debug receving data
    //ESP_LOGI(TAG, "RX from %02X:%02X:%02X:%02X:%02X:%02X",
    //recv_info->src_addr[0], recv_info->src_addr[1], recv_info->src_addr[2],
    //recv_info->src_addr[3], recv_info->src_addr[4], recv_info->src_addr[5]);
}

// ─────────────────────────────────────────────────────────────
// ESPNOW INITIATION
// ─────────────────────────────────────────────────────────────
static esp_err_t espnow_recv_init(void)
{
    // Init FreeRTOS Queue
    g_espnow_queue = xQueueCreate(ESPNOW_QUEUE_SIZE, sizeof(sensor_data_t));

    // Memory initiation check
    if (g_espnow_queue == NULL) {
        ESP_LOGE(TAG, "Create queue fail");
        return ESP_FAIL;
    }

    // Initialize ESPNOW and register receiving callback
    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_recv_cb(espnow_recv_cb)); // this works with the function above

    return ESP_OK;
}

// ─────────────────────────────────────────────────────────────
// ESPNOW RECEIVING TASK
// ─────────────────────────────────────────────────────────────
void espnow_recv_task(void *pvParameter)
{
    // Temporary data
    sensor_data_t data;

    // When data is received in Queue, parse it to be displayed in gui.c
    while (xQueueReceive(g_espnow_queue, &data, portMAX_DELAY) == pdTRUE) {
        // Debugging line
        ESP_LOGI(TAG, "Received from node %d, Temp: %.2f, Humid: %.2f", data.sensor_id, data.temp, data.humid);
        
        // Send data to GUI queue
        if (data.sensor_id >= 1 && data.sensor_id <= 6) {
            sensor_data_t *node = &g_nodes[data.sensor_id - 1];
            node->sensor_id = data.sensor_id;
            node->temp = data.temp;
            node->humid = data.humid;
            node->last_update_ms = xTaskGetTickCount() * portTICK_PERIOD_MS;
            node->valid = true;
        }
    }
}

// ─────────────────────────────────────────────────────────────
// NETWORK INITATION
// ─────────────────────────────────────────────────────────────
// Call all above functions
esp_err_t network_init(void)
{
    ESP_ERROR_CHECK(get_mac());
    ESP_ERROR_CHECK(nvs_init());
    ESP_ERROR_CHECK(wifi_init());
    ESP_ERROR_CHECK(espnow_recv_init());

    return ESP_OK;
}

