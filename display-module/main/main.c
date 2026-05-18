#include <stdio.h>

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

static const char *TAG = "lcd";

static void lvgl_flush_cb(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    esp_lcd_panel_handle_t panel_handle = disp_drv->user_data;
    esp_lcd_panel_draw_bitmap(panel_handle, area->x1, area->y1, area->x2 + 1, area->y2 + 1, color_p);
    lv_disp_flush_ready(disp_drv);
}

static void btn_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * btn = lv_event_get_target(e);
    if(code == LV_EVENT_CLICKED) {
        static uint8_t cnt = 0;
        cnt++;

        /*Get the first child of the button which is the label and change its text*/
        lv_obj_t * label = lv_obj_get_child(btn, 0);
        lv_label_set_text_fmt(label, "Button: %d", cnt);
    }
}

static void lv_tick_task(void *arg)
{
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1));
        lv_tick_inc(1);
    }
}

void app_main(void)
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
    spi_bus_config_t buscfg = {
        .sclk_io_num = LCD_SCLK,
        .mosi_io_num = LCD_MOSI,
        .miso_io_num = -1,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = LCD_WIDTH * LCD_HEIGHT * sizeof(uint16_t),
    };
    ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO)); // Set up SPI bus

    // ─────────────────────────────────────────────────────────────
    // LCD PANEL IO CONFIGURATION
    // ─────────────────────────────────────────────────────────────
    // Handles communication between ESP32 and LCD display
    ESP_LOGI(TAG, "Install panel IO");
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = LCD_DC,
        .cs_gpio_num = LCD_CS,
        .pclk_hz = LCD_PIXEL_CLOCK_HZ,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .spi_mode = 0,
        .trans_queue_depth = 10,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi(LCD_HOST, &io_config, &io_handle)); // Set up LCD IO

    // ─────────────────────────────────────────────────────────────
    // ST7789 LCD DRIVER INITIALIZATION
    // ─────────────────────────────────────────────────────────────
    ESP_LOGI(TAG, "Install ST7789 panel driver");
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = LCD_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
    };

    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_handle, false));
    ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel_handle, false, false));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));

    // Turn backlight on
    gpio_set_level(LCD_BL, LCD_BL_ON);

    // ─────────────────────────────────────────────────────────────
    // LVGL INITIALIZATION
    // ─────────────────────────────────────────────────────────────
    lv_init();

    // ─────────────────────────────────────────────────────────────
    // LVGL TICK SOURCE
    // ─────────────────────────────────────────────────────────────
    xTaskCreate(lv_tick_task, "lv_tick", 2048, NULL, configMAX_PRIORITIES - 1, NULL);


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
    disp_drv.user_data = panel_handle;
    lv_disp_drv_register(&disp_drv);

    // ─────────────────────────────────────────────────────────────
    // CREATE LVGL OBJECT
    // ─────────────────────────────────────────────────────────────
    lv_obj_t * btn1 = lv_btn_create(lv_scr_act());     /*Add a button the current screen*/
    lv_obj_set_pos(btn1, 10, 10);                            /*Set its position*/
    lv_obj_set_size(btn1, 100, 50);                          /*Set its size*/
    lv_obj_add_event_cb(btn1, btn_event_cb, LV_EVENT_ALL, NULL);           /*Assign a callback to the button*/

    lv_obj_t * label1 = lv_label_create(btn1);          /*Add a label to the button*/
    lv_label_set_text(label1, "One");                     /*Set the labels text*/
    lv_obj_center(label1);

    lv_obj_t * btn2 = lv_btn_create(lv_scr_act());
    lv_obj_set_pos(btn2, 130, 10);
    lv_obj_set_size(btn2, 100, 50);
    lv_obj_add_event_cb(btn2, btn_event_cb, LV_EVENT_ALL, NULL);

    lv_obj_t *label2 = lv_label_create(btn2);
    lv_label_set_text(label2, "Two");
    lv_obj_center(label2);

    uint32_t last_update = 0;
    int counter1 = 0;
    int counter2 = 0;
    while (true) {
        lv_timer_handler();
        
        if (lv_tick_get() - last_update >= 1000) {  // every 1 second
        last_update = lv_tick_get();
        counter1++;
        counter2 += 2;  // different rate for variety

        lv_label_set_text_fmt(label1, "One: %d", counter1);
        lv_label_set_text_fmt(label2, "Two: %d", counter2);
    }
        vTaskDelay(pdMS_TO_TICKS(5));
    }

}
