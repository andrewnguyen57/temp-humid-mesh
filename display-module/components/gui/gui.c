#include <stdio.h>

#include "lvgl.h"

#include "gui.h"

typedef struct {
    lv_obj_t *label;
    int node_id;
    int temp;
    int humid;
} disp_data_t;

static void timer_cb(lv_timer_t *timer)
{
    disp_data_t *data = timer->user_data;

    // Placeholder for future updates
    data->temp++;
    data->humid++;

    // temp and humid cannot be greater than 100
    if (data->temp > 100 || data->humid > 100) {
        data->temp = 0;
        data->humid = 0;
    }

    static char buf[64];
    snprintf(buf, sizeof(buf), "Node %d: temp: %d humid: %d", data->node_id, data->temp, data->humid);
    lv_label_set_text(data->label, buf);
}

void gui_init(void)
{
    // ─────────────────────────────────────────────────────────────
    // CREATE LVGL OBJECTS
    // ─────────────────────────────────────────────────────────────
    

    lv_obj_t *hdr = lv_obj_create(lv_scr_act());
    lv_obj_set_pos(hdr, 10, 10);
    lv_obj_set_size(hdr, 220, 40);
    lv_obj_set_style_bg_color(hdr, lv_color_hex(0xFF0000), LV_PART_MAIN);
    lv_obj_clear_flag(hdr, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *hdr_label = lv_label_create(hdr);
    lv_label_set_text(hdr_label, "HOME");
    lv_obj_set_style_text_font(hdr_label, &lv_font_montserrat_24, LV_PART_MAIN);
    lv_obj_set_style_text_color(hdr_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_center(hdr_label);

    lv_obj_t *node1 = lv_obj_create(lv_scr_act());
    lv_obj_set_pos(node1, 10, 60);
    lv_obj_set_size(node1, 220, 35);

    lv_obj_t *node1_label = lv_label_create(node1);
    lv_label_set_text(node1_label, "Node 1: Temp: 0 Humid: 0");
    lv_obj_align(node1_label, LV_ALIGN_LEFT_MID, -10, 0);
    lv_obj_clear_flag(node1, LV_OBJ_FLAG_SCROLLABLE);

    static disp_data_t node1_data;
    node1_data.label = node1_label;
    node1_data.node_id = 1;
    node1_data.temp = 0;
    node1_data.humid = 0;

    lv_timer_create(timer_cb, 1000, &node1_data);

    lv_obj_t *node2 = lv_obj_create(lv_scr_act());
    lv_obj_set_pos(node2, 10, 100);
    lv_obj_set_size(node2, 220, 35);

    lv_obj_t *node2_label = lv_label_create(node2);
    lv_label_set_text(node2_label, "Node 2: Temp: 0 Humid: 0");
    lv_obj_align(node2_label, LV_ALIGN_LEFT_MID, -10, 0);
    lv_obj_clear_flag(node2, LV_OBJ_FLAG_SCROLLABLE);

    static disp_data_t node2_data;
    node2_data.label = node2_label;
    node2_data.node_id = 2;
    node2_data.temp = 0;
    node2_data.humid = 0;

    lv_timer_create(timer_cb, 1000, &node2_data);
    // ─────────────────────────────────────────────────────────────

    lv_obj_t *node3 = lv_obj_create(lv_scr_act());
    lv_obj_set_pos(node3, 10, 140);
    lv_obj_set_size(node3, 220, 35);

    lv_obj_t *node3_label = lv_label_create(node3);
    lv_label_set_text(node3_label, "Node 3: Temp: 0 Humid: 0");
    lv_obj_align(node3_label, LV_ALIGN_LEFT_MID, -10, 0);
    lv_obj_clear_flag(node3, LV_OBJ_FLAG_SCROLLABLE);

    static disp_data_t node3_data;
    node3_data.label = node3_label;
    node3_data.node_id = 3;
    node3_data.temp = 0;
    node3_data.humid = 0;

    lv_timer_create(timer_cb, 1000, &node3_data);
    // ─────────────────────────────────────────────────────────────

    lv_obj_t *node4 = lv_obj_create(lv_scr_act());
    lv_obj_set_pos(node4, 10, 180);
    lv_obj_set_size(node4, 220, 35);

    lv_obj_t *node4_label = lv_label_create(node4);
    lv_label_set_text(node4_label, "Node 4: Temp: 0 Humid: 0");
    lv_obj_align(node4_label, LV_ALIGN_LEFT_MID, -10, 0);
    lv_obj_clear_flag(node4, LV_OBJ_FLAG_SCROLLABLE);

    static disp_data_t node4_data;
    node4_data.label = node4_label;
    node4_data.node_id = 4;
    node4_data.temp = 0;
    node4_data.humid = 0;

    lv_timer_create(timer_cb, 1000, &node4_data);

    // ─────────────────────────────────────────────────────────────

    lv_obj_t *node5 = lv_obj_create(lv_scr_act());
    lv_obj_set_pos(node5, 10, 220);
    lv_obj_set_size(node5, 220, 35);

    lv_obj_t *node5_label = lv_label_create(node5);
    lv_label_set_text(node5_label, "Node 5: Temp: 0 Humid: 0");
    lv_obj_align(node5_label, LV_ALIGN_LEFT_MID, -10, 0);
    lv_obj_clear_flag(node5, LV_OBJ_FLAG_SCROLLABLE);

    static disp_data_t node5_data;
    node5_data.label = node5_label;
    node5_data.node_id = 5;
    node5_data.temp = 0;
    node5_data.humid = 0;

    lv_timer_create(timer_cb, 1000, &node5_data);

    // ─────────────────────────────────────────────────────────────

    lv_obj_t *node6 = lv_obj_create(lv_scr_act());
    lv_obj_set_pos(node6, 10, 260);
    lv_obj_set_size(node6, 220, 35);

    lv_obj_t *node6_label = lv_label_create(node6);
    lv_label_set_text(node6_label, "Node 6: Temp: 0 Humid: 0");
    lv_obj_align(node6_label, LV_ALIGN_LEFT_MID, -10, 0);
    lv_obj_clear_flag(node6, LV_OBJ_FLAG_SCROLLABLE);

    static disp_data_t node6_data;
    node6_data.label = node6_label;
    node6_data.node_id = 6;
    node6_data.temp = 0;
    node6_data.humid = 0;

    lv_timer_create(timer_cb, 1000, &node6_data);
}