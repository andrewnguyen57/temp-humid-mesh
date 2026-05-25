idf_component_register(
    SRCS "network.c"
    INCLUDE_DIRS "." "../../../common"  
    REQUIRES esp_wifi nvs_flash
)