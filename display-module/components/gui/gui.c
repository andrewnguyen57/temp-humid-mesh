/**
 * @file gui.c
 * @brief LVGL GUI implementation for the CYD
 * 
 * Displays temperature and humidity from up to 6 different sensor nodes
 */

#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "esp_err.h"

#include "lvgl.h"

#include "sensor_data.h"
#include "gui.h"

// Global node data
sensor_data_t g_nodes[6];

// ─────────────────────────────────────────────────────────────
// TIMER CALLBACK
// ─────────────────────────────────────────────────────────────
// takes data received from network, parse and format it into display data
// the display data formatted into buf and updates the lvgl label
static void timer_cb(lv_timer_t *timer)
{
    disp_data_t *disp_data = timer->user_data;
    sensor_data_t *node = &g_nodes[disp_data->node_id - 1];
    char buf[64];

    uint32_t now_ms = xTaskGetTickCount() * portTICK_PERIOD_MS;

    // If 10s has passed and no new data is received then display no information
    if (!node->valid || (now_ms - node->last_update_ms > 10000)) {
        snprintf(buf, sizeof(buf), "Temp: --.-C\nHumid: --.-%%");
    } 
    // If new display is received then continuously update information
    else {
        snprintf(buf, sizeof(buf), "Temp: %.1fC\nHumid: %.1f%%", node->temp,node->humid);
    }

    // Update lvgl label
    lv_label_set_text(disp_data->label, buf);
}

// ─────────────────────────────────────────────────────────────
// NODE CARD
// ─────────────────────────────────────────────────────────────
// Array for all the nodes data
static disp_data_t s_node_data[6];
// Function to create a block to display the information on screen
static void create_node_card(lv_obj_t *parent, int node_id, int col, int row)
{   
    // Alignment of the cards
    int x = 5 + col * 120;
    int y = 57 + row * 70;

    // Card
    lv_obj_t *card = lv_obj_create(parent);
    lv_obj_set_pos(card, x, y);
    lv_obj_set_size(card, CARD_W, CARD_H);
    lv_obj_clear_flag(card, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_style_bg_color(card, lv_color_hex(COLOR_GREEN), LV_PART_MAIN);
    lv_obj_set_style_border_color(card, lv_color_hex(COLOR_WHITE), LV_PART_MAIN);
    lv_obj_set_style_border_width(card, 1, LV_PART_MAIN);
    lv_obj_set_style_radius(card, 8, LV_PART_MAIN);
    lv_obj_set_style_pad_all(card, 6, LV_PART_MAIN);

    // title
    static char title_buf[6][10];

    snprintf(title_buf[node_id - 1], sizeof(title_buf[0]), "ROOM %d", node_id);

    lv_obj_t *title = lv_label_create(card);
    lv_label_set_text(title, title_buf[node_id - 1]);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_20, LV_PART_MAIN);
    lv_obj_set_style_text_color(title, lv_color_hex(COLOR_WHITE), LV_PART_MAIN);
    lv_obj_align(title, LV_ALIGN_TOP_LEFT, 0, -2);

    // value label
    lv_obj_t *val = lv_label_create(card);

    lv_label_set_text(val, "Temp: --.-C\nHumid: --.-%");
    lv_obj_set_style_text_font(val, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(val, lv_color_hex(COLOR_WHITE), LV_PART_MAIN);
    lv_obj_align(val, LV_ALIGN_TOP_LEFT, 0, 22);

    // timer
    s_node_data[node_id - 1].label = val;
    s_node_data[node_id - 1].node_id = node_id;

    // timer task: call the timer call back to update the lvgl label every 1s
    lv_timer_create(timer_cb, 1000, &s_node_data[node_id - 1]);
}

esp_err_t gui_init(void)
{
    // Screen background
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(COLOR_WHITE), LV_PART_MAIN);

    // ─────────────────────────────────────────────────────────────
    // HEADER
    // ─────────────────────────────────────────────────────────────
    lv_obj_t *hdr = lv_obj_create(lv_scr_act());
    lv_obj_set_pos(hdr, 0, 0);
    lv_obj_set_size(hdr, 240, 42);
    lv_obj_clear_flag(hdr, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(hdr, lv_color_hex(COLOR_RED), LV_PART_MAIN);
    lv_obj_set_style_border_width(hdr, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(hdr, 0, LV_PART_MAIN);

    lv_obj_t *hdr_label = lv_label_create(hdr);
    lv_label_set_text(hdr_label, "HOME MONITOR");
    lv_obj_set_style_text_font(hdr_label, &lv_font_montserrat_24, LV_PART_MAIN);
    lv_obj_set_style_text_color(hdr_label, lv_color_hex(COLOR_WHITE), LV_PART_MAIN);
    lv_obj_center(hdr_label);

    // ─────────────────────────────────────────────────────────────
    // NODE CARDS
    // ─────────────────────────────────────────────────────────────

    create_node_card(lv_scr_act(), 1, 0, 0);
    create_node_card(lv_scr_act(), 2, 1, 0);

    create_node_card(lv_scr_act(), 3, 0, 1);
    create_node_card(lv_scr_act(), 4, 1, 1);

    create_node_card(lv_scr_act(), 5, 0, 2);
    create_node_card(lv_scr_act(), 6, 1, 2);

    // ─────────────────────────────────────────────────────────────
    // FOOTER
    // ─────────────────────────────────────────────────────────────
    lv_obj_t *ftr = lv_obj_create(lv_scr_act());
    lv_obj_set_pos(ftr, 0, 278);
    lv_obj_set_size(ftr, 240, 42);
    lv_obj_clear_flag(ftr, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ftr, lv_color_hex(COLOR_BLUE), LV_PART_MAIN);
    lv_obj_set_style_border_width(ftr, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(ftr, 0, LV_PART_MAIN);

    lv_obj_t *ftr_label = lv_label_create(ftr);
    lv_label_set_text(ftr_label, "Status: ACTIVE"); // Place holder for future updates
    lv_obj_set_style_text_font(ftr_label,&lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(ftr_label, lv_color_hex(COLOR_WHITE), LV_PART_MAIN);
    lv_obj_align(ftr_label, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    return ESP_OK;
}