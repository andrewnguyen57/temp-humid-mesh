idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    REQUIRES lcd driver lvgl esp_lcd lvgl__lvgl
)