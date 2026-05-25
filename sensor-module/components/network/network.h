/**
 * @file network.h
 * @brief Network header file
 * 
 * Contain all the constants and APIs for Network
 */

 #pragma once

#define ESPNOW_WIFI_MODE WIFI_MODE_STA
#define ESPNOW_WIFI_IF WIFI_IF_STA
#define ESPNOW_QUEUE_SIZE 10
#define ESPNOW_CHANNEL 1

// Prototype
esp_err_t network_init(void);
void espnow_send_task(void *vParameters);