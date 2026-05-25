



 static void lv_tick_task(void *arg)
{
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1));
        lv_tick_inc(1);
    }
}
    // ─────────────────────────────────────────────────────────────
    // Cycle colours using draw bitmap to check display functionality
    // ─────────────────────────────────────────────────────────────

    #define NUM_COLOURS 8

    const uint16_t colours[NUM_COLOURS] = {
        LCD_WHITE, LCD_BLACK, LCD_RED, LCD_GREEN, LCD_BLUE, LCD_YELLOW, LCD_CYAN, LCD_MAGENTA
    };
    const char *colour_names[NUM_COLOURS] = {
        "white", "black", "red", "green", "blue", "yellow", "cyan", "magenta"
    };

    ESP_LOGI(TAG, "Fill screen white");
    // Allocate a single row of white pixels (0xFFFF = white in RGB565)
    uint16_t *buffer = heap_caps_malloc(LCD_WIDTH * sizeof(uint16_t), MALLOC_CAP_DMA);
    assert(buffer);
    for (int i = 0; i < LCD_WIDTH; i++) {
        buffer[i] = LCD_WHITE;
    }
    for (int y = 0; y < LCD_HEIGHT; y++) {
        esp_lcd_panel_draw_bitmap(panel_handle, 0, y, LCD_WIDTH, y + 1, buffer);
    }

    ESP_LOGI(TAG, "Turn on LCD backlight");
    gpio_set_level(LCD_BL, LCD_BL_ON);

    while (true) {
        for (int i = 0; i < 8; i++) {
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

    void get_mac_address(){
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_STA);

    printf("%02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}
