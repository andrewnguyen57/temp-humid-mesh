/**
 * @file sensor_data.h
 * @brief Sensor Data object shared by all components
 * 
 */

 #pragma once

typedef struct {
    uint32_t sensor_id;
    float temp;
    float humid;
    uint32_t last_update_ms;
    bool valid;
} sensor_data_t;