/**
 * @file gui.h
 * @brief LVGL GUI header file
 * 
 * Contain all the constants and API for GUI
 */
#pragma once

#define DISPLAY_QUEUE_SIZE 10

// Swapped RGB565 colours matching your LCD

// Simple colours only
#define COLOR_BLACK    0x000000
#define COLOR_WHITE    0xFFFFFF
#define COLOR_RED      0xFF0000
#define COLOR_BLUE     0x00FF00
#define COLOR_GREEN    0x0000FF

#define CARD_W  110
#define CARD_H   65

typedef struct {
    lv_obj_t *label;
    uint32_t node_id;
    float temp;
    float humid;
} disp_data_t;

esp_err_t gui_init(void);   