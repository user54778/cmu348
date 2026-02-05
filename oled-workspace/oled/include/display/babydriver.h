#ifndef BABYDRIVER_H
#define BABYDRIVER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define BD_FB_WIDTH 128
#define BD_FB_HEIGHT 64

// framebuffer size
// working with bits not bytes
#define BD_FB_SIZE (BD_FB_WIDTH * BD_FB_HEIGHT) / 8

// SSD1306 commands
#define BD_SSD1306_SET_CONTRAST 0x81
#define BD_SSD1306_DISPLAY_ALL_ON_RESUME 0xA4
#define BD_SSD1306_DISPLAY_ALL_ON 0xA5
#define BD_SSD1306_DISPLAY_NORMAL 0xA6
#define BD_SSD1306_DISPLAY_INVERSE 0xA7
#define BD_SSD1306_DISPLAY_OFF 0xAE
#define BD_SSD1306_DISPLAY_ON 0xAF

// NOTE: Rest of commands are AI-generated based on SSD1306 data sheet

// Scrolling Commands
#define BD_SSD1306_RIGHT_HORIZONTAL_SCROLL 0x26
#define BD_SSD1306_LEFT_HORIZONTAL_SCROLL 0x27
#define BD_SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define BD_SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2A
#define BD_SSD1306_DEACTIVATE_SCROLL 0x2E
#define BD_SSD1306_ACTIVATE_SCROLL 0x2F
#define BD_SSD1306_SET_VERTICAL_SCROLL_AREA 0xA3

// Addressing Setting Commands
#define BD_SSD1306_SET_LOWER_COLUMN 0x00  // (0x00-0x0F)
#define BD_SSD1306_SET_HIGHER_COLUMN 0x10 // (0x10-0x1F)
#define BD_SSD1306_MEMORY_ADDR_MODE 0x20
#define BD_SSD1306_SET_COLUMN_ADDR 0x21     // Triple byte command
#define BD_SSD1306_SET_PAGE_ADDR 0x22       // Triple byte command
#define BD_SSD1306_SET_PAGE_START_ADDR 0xB0 // (0xB0-0xB7)

// Hardware Configuration Commands
#define BD_SSD1306_SET_START_LINE 0x40 // (0x40-0x7F)
#define BD_SSD1306_SET_SEGMENT_REMAP_0 0xA0
#define BD_SSD1306_SET_SEGMENT_REMAP_127 0xA1
#define BD_SSD1306_SET_MULTIPLEX_RATIO 0xA8 // Double byte command
#define BD_SSD1306_COM_SCAN_DIR_NORMAL 0xC0
#define BD_SSD1306_COM_SCAN_DIR_REMAP 0xC8
#define BD_SSD1306_SET_DISPLAY_OFFSET 0xD3 // Double byte command
#define BD_SSD1306_SET_COM_PINS 0xDA       // Double byte command

// Timing & Driving Scheme Commands
#define BD_SSD1306_SET_DISPLAY_CLOCK 0xD5    // Double byte command
#define BD_SSD1306_SET_PRECHARGE_PERIOD 0xD9 // Double byte command
#define BD_SSD1306_SET_VCOMH_DESELECT 0xDB   // Double byte command
#define BD_SSD1306_NOP 0xE3

// Charge Pump Commands
#define BD_SSD1306_CHARGE_PUMP_SETTING 0x8D // Double byte command
#define BD_SSD1306_CHARGE_PUMP_ENABLE 0x14
#define BD_SSD1306_CHARGE_PUMP_DISABLE 0x10

// Memory Addressing Modes (for use with 0x20)
#define BD_SSD1306_ADDR_MODE_HORIZONTAL 0x00
#define BD_SSD1306_ADDR_MODE_VERTICAL 0x01
#define BD_SSD1306_ADDR_MODE_PAGE 0x02

extern uint8_t BD_FRAMEBUFFER[BD_FB_SIZE];

void bd_set_pixel(int x, int y, bool state);
int bd_get_pixel(int x, int y);
void bd_draw_bitmap(int x, int y, int width, int height, const uint8_t *bitmap);

/* NOTE: BEGIN PHYSICAL COMS */

// An abstraction that uses function pointers as a way for the user
// to register their own HAL SPI send function.
void bd_set_spi_tx(void (*spi_tx)(uint8_t byte));
void bd_set_spi_dc(void (*set_cd)(bool state));
void bd_set_spi_cs(void (*set_cs)(bool state));

void bd_power_on(void);
void bd_power_off(void);

void bd_render_framebuffer(void);
void bd_fill_framebuffer(bool);
void bd_init(void);
void bd_clear(void);
void *oled_memset(void *ptr, int c, size_t len);
#endif
