#pragma once

// Prototypes
//void cycle_colours(uint16_t *buffer, uint16_t *colours, const char *colour_names[], esp_lcd_panel_handle_t panel_handle, int n);

// Using SPI2
#define LCD_HOST SPI2_HOST

// LCD pins for ST7789 driver
#define LCD_MISO    12
#define LCD_MOSI    13
#define LCD_SCLK    14
#define LCD_CS      15
#define LCD_DC      2
#define LCD_RST     -1
#define LCD_BL      27

// Touch pins for CST820 driver
#define LCD_TOUCH_SDA 33
#define LCD_TOUCH_SCL 32
#define LCD_TOUCH_RST 25
#define LCD_TOUCH_INT 21

// Bit number used to represend command and parameter
#define LCD_CMD_BITS 8
#define LCD_PARAM_BITS 8

// Screen Size
#define LCD_WIDTH   240
#define LCD_HEIGHT  320

#define LCD_PIXEL_CLOCK_HZ (40 * 1000 * 1000)
#define LCD_BL_ON 1
#define LCD_BL_OFF !LCD_BL_ON

// LCD colours
#define LCD_WHITE   0xFFFF 
#define LCD_BLACK   0x0000  
#define LCD_RED     0x00F8  // was 0xF800
#define LCD_GREEN   0xE007  // was 0x07E0
#define LCD_BLUE    0x1F00  // was 0x001F
#define LCD_YELLOW  0xE0FF  // was 0xFFE0
#define LCD_CYAN    0xFF1F  // was 0x07FF
#define LCD_MAGENTA 0x1FF8  // was 0xF81F

// Colours
#define NUM_COLOURS 8

// Prototypes
void lcd_init(void);