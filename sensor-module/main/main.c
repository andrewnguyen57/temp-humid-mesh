#include <stddef.h>
#include "freertos/FreeRTOS.h"

#include "sensor.h"

#include "network.h"


void app_main()
{
    network_init();
    sensor_init();
    xTaskCreate(sensor_task, "sensor_task", 4096, NULL, 5, NULL);
    xTaskCreate(espnow_send_task, "espnow_send_task", 4096, NULL, 5, NULL);
}