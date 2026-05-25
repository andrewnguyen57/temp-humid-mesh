/**
 * @file sensor.c
 * @brief Initiate the sensor and send data
 * 
 */

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "sht4x.h"

#include "esp_system.h"
#include "esp_err.h"
#include "esp_log.h"

#include "sensor.h"
#include "sensor_data.h"

static const char *TAG = "sensor";

// STH4X object
static sht4x_t sen;

// Queue object
extern QueueHandle_t g_espnow_queue;

// ─────────────────────────────────────────────────────────────
// SENSOR INITATION
// ─────────────────────────────────────────────────────────────
// Function tp init I2C and SHT4X Sensor
esp_err_t sensor_init(void)
{
    ESP_ERROR_CHECK(i2cdev_init());
    memset(&sen, 0, sizeof(sht4x_t));

    ESP_ERROR_CHECK(sht4x_init_desc(&sen, 0, SENSOR_SDA, SENSOR_SCL));
    ESP_ERROR_CHECK(sht4x_init(&sen));

    return ESP_OK;
}

// ─────────────────────────────────────────────────────────────
// SENSOR SENDING
// ─────────────────────────────────────────────────────────────
// Function to send data into ESPNOW Queue
static esp_err_t send_data(sensor_data_t *data)
{
    if (xQueueSend(g_espnow_queue, data, 0) != pdTRUE) {
        ESP_LOGE(TAG, "Send data to queue fail");
        return ESP_FAIL;
    }
    return ESP_OK;
}

// ─────────────────────────────────────────────────────────────
// SENSOR TASK
// ─────────────────────────────────────────────────────────────
// Function to collect data from the sensor and send it to ESPNOW QUEUE using send_data
void sensor_task(void *vParameters)
{   
    // Temporary data
    sensor_data_t data;

    // Tell the display which node is sending the data
    data.sensor_id = SENSOR_ID; // Change this in sensor.h before flashing

    while (1) {
        ESP_ERROR_CHECK(sht4x_measure(&sen, &data.temp, &data.humid));
        ESP_LOGI(TAG, "ID = %d TEMP = %.2f, HUMID = %.2f", data.sensor_id, data.temp, data.humid); // Debugging
        ESP_ERROR_CHECK(send_data(&data));
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}

