/**
 * @file sensor.h
 * @brief Sensor header file
 * 
 * Contain all the constants and APIs for Sensor
 */

#pragma once

#define SENSOR_ID   1 // Change this value when flashing a new sensor node
#define SENSOR_SCL  GPIO_NUM_22
#define SENSOR_SDA  GPIO_NUM_23
#define SENSOR_QUEUE_SIZE 10

// Prototypes
esp_err_t sensor_init(void);
void sensor_task(void *vParameters);
