/**
 * @file network.c
 * @brief ESPNOW connection 
 * 
 * Initiate NVS, WiFi and ESPNOW
 * ESPNOW Sending callback, initiate peer connection, handles sending data
 */

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "esp_err.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_netif.h"
#include "esp_now.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

#include "sensor_data.h"
#include "network.h"

static const char *TAG = "network";

// MAC addresses
// Device's own MAC address
static uint8_t s_device_mac[ESP_NOW_ETH_ALEN];
// Display module MAC address (the display module is the only receiving node)
static uint8_t s_recv_mac[ESP_NOW_ETH_ALEN] = {0xD4, 0xE9, 0xF4, 0xED, 0x89, 0x70};

// Queue object
QueueHandle_t g_espnow_queue;

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
    ESP_ERROR_CHECK(esp_wifi_set_protocol(ESPNOW_WIFI_IF, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N|WIFI_PROTOCOL_LR));
#endif

    return ESP_OK;
}

// ─────────────────────────────────────────────────────────────
// ESPNOW SEND CALL BACK
// ─────────────────────────────────────────────────────────────
// Function for handling when sending ESPNOW data
static void espnow_send_cb(const esp_now_send_info_t *tx_info, esp_now_send_status_t status) 
{
    if (status == ESP_NOW_SEND_SUCCESS) {
        ESP_LOGI(TAG, "Send success");
    }
    else {
        ESP_LOGE(TAG, "Send failed");
    }
}

// ─────────────────────────────────────────────────────────────
// ESPNOW INITIATION
// ─────────────────────────────────────────────────────────────
static esp_err_t espnow_send_init(void)
{
    // Create queue for receving data from sensor
    g_espnow_queue = xQueueCreate(ESPNOW_QUEUE_SIZE, sizeof(sensor_data_t));
    if (g_espnow_queue == NULL) {
        ESP_LOGE(TAG, "Create queue fail");
        return ESP_FAIL;
    }

    // Initialize ESPNOW and register sending callback
    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_send_cb(espnow_send_cb));

    // Init peer
    esp_now_peer_info_t peer = {
        .channel = ESPNOW_CHANNEL,
        .ifidx = ESPNOW_WIFI_IF,
        .encrypt = false,
    };
    // Copy mac addres from above to the peer address
    memcpy(peer.peer_addr, s_recv_mac, ESP_NOW_ETH_ALEN);
    // Add peer
    ESP_ERROR_CHECK(esp_now_add_peer(&peer));

    return ESP_OK;
}

// ─────────────────────────────────────────────────────────────
// ESPNOW SENDING TASK
// ─────────────────────────────────────────────────────────────
void espnow_send_task(void *vParameters) {

    // Temporary data
    sensor_data_t data;

    // Send data to the peer
    while (xQueueReceive(g_espnow_queue, &data, portMAX_DELAY) == pdTRUE) {
        esp_now_send(s_recv_mac, (uint8_t *)&data, sizeof(data));
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
    ESP_ERROR_CHECK(espnow_send_init());

    return ESP_OK;
}

