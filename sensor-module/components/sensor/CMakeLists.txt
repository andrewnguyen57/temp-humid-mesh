idf_component_register(
    SRCS "sensor.c"
    INCLUDE_DIRS "." "../../../common"
    REQUIRES esp-idf-lib__sht4x
)