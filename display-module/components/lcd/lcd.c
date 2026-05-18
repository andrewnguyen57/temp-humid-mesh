/*
#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_st7789.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_err.h"
#include "esp_log.h"
#include "lvgl.h"
#include "lcd.h"


const uint16_t colours[NUM_COLOURS] = {
    LCD_WHITE, LCD_BLACK, LCD_RED, LCD_GREEN, LCD_BLUE, LCD_YELLOW, LCD_CYAN, LCD_MAGENTA
};
const char *colour_names[NUM_COLOURS] = {
    "white", "black", "red", "green", "blue", "yellow", "cyan", "magenta"
};

void cycle_colours(uint16_t *buffer, uint16_t *colours, const char *colour_names[], esp_lcd_panel_handle_t panel_handle, int n) {
    while (true) {
        for (int i = 0; i < n; i++) {
            vTaskDelay(pdMS_TO_TICKS(1000));
            ESP_LOGI(TAG, "Displaying %s", colour_names[i]);
            for (int j = 0; j < LCD_WIDTH; j++) {
                buffer[j] = colours[i];
            }
            for (int y = 0; y < LCD_HEIGHT; y++) {
                esp_lcd_panel_draw_bitmap(panel_handle, 0, y, LCD_WIDTH, y + 1, buffer);
            } 
        }
    }
}

    // ─────────────────────────────────────────────────────────────
    // FOR LATER USE
    // ─────────────────────────────────────────────────────────────

    ESP_LOGI(TAG, "Fill screen white");
    // Allocate a single row of white pixels (0xFFFF = white in RGB565)
    uint16_t *buf = heap_caps_malloc(LCD_WIDTH * sizeof(uint16_t), MALLOC_CAP_DMA);
    assert(buf);
    for (int i = 0; i < LCD_WIDTH; i++) {
        buf[i] = LCD_WHITE;
    }
    for (int y = 0; y < LCD_HEIGHT; y++) {
        esp_lcd_panel_draw_bitmap(panel_handle, 0, y, LCD_WIDTH, y + 1, buf);
    }

    ESP_LOGI(TAG, "Turn on LCD backlight");
    gpio_set_level(LCD_BL, LCD_BL_ON);

*/