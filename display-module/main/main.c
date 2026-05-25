/**
 * @file main.c
 * @brief Call all the component functions
 * 
 */

#include <stddef.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "lvgl.h"

#include "lcd.h"

#include "gui.h"

#include "network.h"

void app_main(void)
{   
    lcd_init();
    gui_init();
    network_init();
    xTaskCreate(espnow_recv_task, "espnow_recv", 4096, NULL, 4, NULL);

    while(1) {
        lv_timer_handler();
        lv_tick_inc(10);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
