#ifndef BABYDRIVER_H
#define BABYDRIVER_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define BD_FB_WIDTH 128
#define BD_FB_HEIGHT 64

// framebuffer size
// working with bits not bytes
// NOTE: BD_FB_SIZE MUST BE NO LARGER THAN 1024 BYTES FOR SSD1309
#define BD_FB_SIZE (BD_FB_WIDTH * BD_FB_HEIGHT) / 8

// NOTE: These are taken from this library written here:
// https://libstock.mikroe.com/projects/view/1957/ssd1309-oled-library

/** BEGIN SSD1309 COMMANDS **/

#define Set_Lower_Column_Start_Address_CMD 0x00
#define Set_Higher_Column_Start_Address_CMD 0x10
#define Set_Memory_Addressing_Mode_CMD 0x20
#define Set_Column_Address_CMD 0x21
#define Set_Page_Address_CMD 0x22
#define Set_Display_Start_Line_CMD 0x40
#define Set_Contrast_Control_CMD 0x81
#define Set_Charge_Pump_CMD 0x8D
#define Set_Segment_Remap_CMD 0xA0
#define Set_Entire_Display_ON_CMD 0xA4
#define Set_Normal_or_Inverse_Display_CMD 0xA6
#define Set_Multiplex_Ratio_CMD 0xA8
#define Set_Display_ON_or_OFF_CMD 0xAE
#define Set_Page_Start_Address_CMD 0xB0
#define Set_COM_Output_Scan_Direction_CMD 0xC0
#define Set_Display_Offset_CMD 0xD3
#define Set_Display_Clock_CMD 0xD5
#define Set_Pre_charge_Period_CMD 0xD9
#define Set_Common_HW_Config_CMD 0xDA
#define Set_VCOMH_Level_CMD 0xDB
#define Set_NOP_CMD 0xE3

#define Horizontal_Addressing_Mode 0x00
#define Vertical_Addressing_Mode 0x01
#define Page_Addressing_Mode 0x02

#define Disable_Charge_Pump 0x00
#define Enable_Charge_Pump 0x04

#define Column_Address_0_Mapped_to_SEG0 0x00
#define Column_Address_0_Mapped_to_SEG127 0x01

#define Normal_Display 0x00
#define Entire_Display_ON 0x01
/** ----------------------------------------- **/
#define Non_Inverted_Display 0x00
#define Inverted_Display 0x01
/** ----------------------------------------- **/
#define Display_OFF 0x00
#define Display_ON 0x01

#define Scan_from_COM0_to_63 0x00
#define Scan_from_COM63_to_0 0x08

#define x_size 128
#define x_max x_size
#define x_min 0
#define y_size 64
#define y_max 8
#define y_min 0

extern uint8_t BD_FRAMEBUFFER[BD_FB_SIZE];

void bd_set_pixel(int x, int y, bool state);
int bd_get_pixel(int x, int y);
void bd_draw_bitmap(int x, int y, int width, int height, const uint8_t *bitmap);
void bd_draw_text(int x, int y, int size, char *text);
void bd_fill_framebuffer(bool colour);
void bd_print_framebuffer();

/** NOTE: BEGIN PHYSICAL COMS **/

// An abstraction that uses function pointers as a way for the user
// to register their own HAL SPI send function.
void bd_set_spi_tx(void (*spi_tx)(uint8_t byte));
void bd_set_spi_dc(void (*set_cd)(bool state));
void bd_set_spi_cs(void (*set_cs)(bool state));

void bd_power_on(void);
void bd_power_off(void);

void bd_render_framebuffer();
void bd_init(void);
void bd_clear_screen();
void bd_clear_buffer();

#endif
