
#include <stdio.h>

#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_st7789.h"

#include "driver/gpio.h"
#include "driver/spi_master.h"

#include "esp_err.h"
#include "esp_log.h"

#include "lvgl.h"

#include "lcd.h"

static const char *TAG = "lcd";

// ─────────────────────────────────────────────────────────────
// LVGL
// ─────────────────────────────────────────────────────────────
static void lvgl_flush_cb(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    esp_lcd_panel_handle_t lcd_panel_handle = disp_drv->user_data;
    esp_lcd_panel_draw_bitmap(lcd_panel_handle, area->x1, area->y1, area->x2 + 1, area->y2 + 1, color_p);
    lv_disp_flush_ready(disp_drv);
}

void lcd_init(void)
{
    // ─────────────────────────────────────────────────────────────
    // BACKLIGHT GPIO CONFIGURATION
    // ─────────────────────────────────────────────────────────────
    // Configure the LCD backlight pin as an output pin
    ESP_LOGI(TAG, "Turn off LCD backlight");
    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << LCD_BL
    };

    // Apply the GPIO configuration
    ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));
    gpio_set_level(LCD_BL, !LCD_BL_ON); // Turn backlight off


    // ─────────────────────────────────────────────────────────────
    // SPI BUS CONFIGURATION
    // ─────────────────────────────────────────────────────────────
    // Configure SPI bus to communicate with the LCD display
    ESP_LOGI(TAG, "Initialize SPI bus");
    spi_bus_config_t spi_mst_config = {
        .sclk_io_num = LCD_SCLK,
        .mosi_io_num = LCD_MOSI,
        .miso_io_num = LCD_MISO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = LCD_WIDTH * LCD_HEIGHT * sizeof(uint16_t),
    };
    ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &spi_mst_config, SPI_DMA_CH_AUTO)); // Set up SPI bus

    // ─────────────────────────────────────────────────────────────
    // LCD PANEL IO CONFIGURATION
    // ─────────────────────────────────────────────────────────────
    // Handles communication between ESP32 and LCD display
    ESP_LOGI(TAG, "Install panel IO");
    esp_lcd_panel_io_handle_t lcd_panel_io_handle = NULL;
    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = LCD_DC,
        .cs_gpio_num = LCD_CS,
        .pclk_hz = LCD_PIXEL_CLOCK_HZ,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .spi_mode = 0,
        .trans_queue_depth = 10,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi(LCD_HOST, &io_config, &lcd_panel_io_handle)); // Set up LCD IO

    // ─────────────────────────────────────────────────────────────
    // ST7789 LCD DRIVER INITIALIZATION
    // ─────────────────────────────────────────────────────────────
    ESP_LOGI(TAG, "Install ST7789 panel driver");
    esp_lcd_panel_handle_t lcd_panel_handle = NULL; // <---- May need to be move outside to global later
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = LCD_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
    };

    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(lcd_panel_io_handle, &panel_config, &lcd_panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(lcd_panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(lcd_panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(lcd_panel_handle, false));
    ESP_ERROR_CHECK(esp_lcd_panel_mirror(lcd_panel_handle, false, false));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(lcd_panel_handle, true));

    // Turn backlight on
    ESP_LOGI(TAG, "Turn on backlight");
    gpio_set_level(LCD_BL, LCD_BL_ON);

    // ─────────────────────────────────────────────────────────────
    // LVGL INITIALIZATION
    // ─────────────────────────────────────────────────────────────
    ESP_LOGI(TAG, "Initialize LVGL");
    lv_init();

    // ─────────────────────────────────────────────────────────────
    // LVGL DRAW BUFFER
    // ─────────────────────────────────────────────────────────────
    static lv_color_t lv_buf1[LCD_WIDTH * 40];
    static lv_color_t lv_buf2[LCD_WIDTH * 40];
    static lv_disp_draw_buf_t draw_buf;
    lv_disp_draw_buf_init(&draw_buf, lv_buf1, lv_buf2, LCD_WIDTH * 40);

    // ─────────────────────────────────────────────────────────────
    // LVGL DISPLAY DRIVER
    // ─────────────────────────────────────────────────────────────
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = LCD_WIDTH;
    disp_drv.ver_res = LCD_HEIGHT;
    disp_drv.flush_cb = lvgl_flush_cb;
    disp_drv.draw_buf = &draw_buf;
    disp_drv.user_data = lcd_panel_handle;
    lv_disp_drv_register(&disp_drv);
}