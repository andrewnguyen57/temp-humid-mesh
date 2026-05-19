#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_wifi.h"
#include "esp_timer.h"

#include "lvgl.h"

#include "lcd.h"

#include "gui.h"

// Tag for ESP LOGI
//static const char *TAG = "lcd";



void app_main(void)
{   
    lcd_init();

    gui_init();

    while(1) {
        lv_timer_handler();
        lv_tick_inc(10);
        vTaskDelay(pdMS_TO_TICKS(10));
    }

}
