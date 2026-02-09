#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "babydriver.h"
#include "fonts.h"

// Our 1-D array framebuffer
uint8_t BD_FRAMEBUFFER[BD_FB_SIZE] = {0};

void bd_print_framebuffer() {

  for (int y = 0; y < BD_FB_HEIGHT; y++) {
    for (int x = 0; x < BD_FB_WIDTH; x++) {

      if (bd_get_pixel(x, y)) {
        printf("1");
      } else {
        printf("0");
      }
    }

    printf("\n");
  }
}

void bd_fill_framebuffer(bool colour) {

  memset(BD_FRAMEBUFFER, colour ? 0xFF : 0x00, BD_FB_SIZE);
}

// Set a pixel in our monoframe buffer (simply "true" or "false")
void bd_set_pixel(int x, int y, bool state) {
  // position is equivalent to x + width * y
  // We also need to divide by 8 since we are overshooting by 8 times, since
  // there are 8 bits in a byte, and we want to access BITS not BYTES
  int pos = (BD_FB_WIDTH * y + x) >> 3;
  if (state) {
    BD_FRAMEBUFFER[pos] |= (0x80 >> (x & 0x7));
  } else {
    BD_FRAMEBUFFER[pos] &= ~(0x80 >> (x & 0x7));
  }
}

// Return a pixel from our monoframe buffer.
int bd_get_pixel(int x, int y) {
  int pos = (BD_FB_WIDTH * y + x) >> 3;
  // grab the pixel
  return BD_FRAMEBUFFER[pos] & (0x80 >> (x & 0x7));
}

void bd_draw_bitmap(int x, int y, int width, int height,
                    const uint8_t *bitmap) {
  // take a bit map and copy it into the framebuffer

  // What would that do? Loop through every bit in the bitmap and set the
  // equivalent position in the framebuffer
  uint8_t chunkMask = 0;
  int size = width * height >> 3;

  int origX = x;
  int origY = y;

  // loop through each byte
  for (int i = 0; i < size; i++) {
    // now loop through each bit in the byte
    for (int j = 0; j < 8; j++) {
      chunkMask = 0x80 >> j; // mask jth bit

      // set pixel (x, y) based on if our mask and i is true/false
      bd_set_pixel(x, y, bitmap[i] & chunkMask);
      x++;

      // since this is 1d, we need a boundary check to not landscape forever
      // if x original is equal width, (we've gone width ways)
      if ((x - origX) == width) {
        // increase y
        y++;
        // reset x to original position
        x = origX;
      }

      // finished the height of our image
      if ((y - origY) == height) {
        return;
      }

      if (y > BD_FB_HEIGHT) {
        return;
      }
    }
  }
}

void bd_draw_text(int x, int y, int size, char *text) {
  // Bitmap fonts are simply a bitmap representing each character;
  // it is then simply a matter of "drawing" that specific bitmap per character
  // you want to print on the screen.
  const uint8_t *font;     // ptr to font to use
  const uint8_t *drawChar; // ptr to char want to use

  // keep starting x and y since we will wrap text over to the beginning.
  int runningX = x; // keep starting x
  int runningY = y; // keep starting y

  int target;

  // prototype for switching from which font size a user would want
  switch (size) {
  case 16:
  default:
    font = TERMINUS16;
  }

  for (int i = 0; i < strlen(text); i++) {
    // bounds checking
    if (runningX >= BD_FB_WIDTH) {
      runningX = x;
      // where runningY is incr by y
      runningY += size; // move down size
    }

    if (runningY >= BD_FB_HEIGHT) {
      // out of bounds of screen
      return;
    }
    // This takes the full size in memory of our character,
    // (16 pixels here) divided by 8 (since we want bits)
    // multiply by the character we are at minus a space to get the
    // ASCII character.
    // Why? -> ASCII spacebar is FIRST character we have bitmaps to represent
    target = (size * size >> 3) * (text[i] - ' ');
    // we are left with target characters left to get the target we want
    drawChar = font + target;

    //
    bd_draw_bitmap(runningX, runningY, size, size, drawChar);

    // incr runningx by size since char by char
    runningX += size;
  }
}

/* Three dummy function pointers for SPI. User implements set_tx/cd/cs via their
 * own HAL/CMSIS for SPI. */
// Dummy SPI data transmit
void bd_dummy_tx(uint8_t byte) { ; }
// Dummy SPI CD/DC
void bd_dummy_dc(bool state) { ; }
// Dummy SPI chip select
void bd_dummy_cs(bool state) { ; }

// Register function pointer bd_spi_tx to use everytime SPI needs to tx.
// User implements bd_set_
// The function pointer that bd_set_x takes in.
// Originally takes in the dummy pointers
void (*bd_spi_tx)(uint8_t byte) = &bd_dummy_tx;
// "data command" (pin 6) (low = cmd, high = data)
// mode select pin to interpret incoming SPI data
void (*bd_set_dc)(bool state) = &bd_dummy_dc;
void (*bd_set_cs)(bool state) = &bd_dummy_cs;

// Set the SPI TX to the HAL TX for SPI
void bd_set_spi_tx(void (*spi_tx)(uint8_t byte)) { bd_spi_tx = spi_tx; }

// Same for these, but these are GPIO lines for SPI.
void bd_set_spi_dc(void (*set_cd)(bool state)) { bd_set_dc = set_cd; }
void bd_set_spi_cs(void (*set_cs)(bool state)) { bd_set_cs = set_cs; }

// Transmit SPI cmd
void bd_tx_cmd(uint8_t cmd) {
  bd_set_dc(false); // DC LOW = CMD MODE
  bd_set_cs(false); // CS LOW = CS chip
  bd_spi_tx(cmd);   // send byte
  bd_set_cs(true);  // CS HIGH = DS chip
}

// Transmit SPI data
void bd_tx_data(uint8_t data) {
  bd_set_dc(true); // DC HIGH = DATA MODE
  bd_set_cs(false);
  bd_spi_tx(data);
  bd_set_cs(true);
}

void bd_power_on() { bd_tx_cmd(Set_Display_ON_or_OFF_CMD + Display_ON); }

void bd_power_off() { bd_tx_cmd(Set_Display_ON_or_OFF_CMD); }

// Render the framebuffer on the display
void bd_render_framebuffer() {
  // 8 pages for the SSD1306
  for (uint8_t page = 0; page < 8; page++) {
    // set page address
    bd_tx_cmd(Set_Page_Start_Address_CMD | page);
    // set column address (lower start and upper start)
    // specified via 10.3
    //"For example, if the page address is set to B2h, lower column address is
    // 03h and upper column address is 10h,
    // then that means the starting column is SEG3 of PAGE2."
    bd_tx_cmd(Set_Lower_Column_Start_Address_CMD);
    bd_tx_cmd(Set_Higher_Column_Start_Address_CMD);
    // send 0-128 columns (bytes) for this page
    for (uint8_t col = 0; col < 128; col++) {
      // pg 0 = 0 - 127
      // ....
      // pg 7 = 896 - 1023
      // bd_tx_data(BD_FRAMEBUFFER[page * 128 + col]);
      bd_tx_data(BD_FRAMEBUFFER[page * 128 + col]);
    }
  }
}

void bd_clear_screen() {
  memset(BD_FRAMEBUFFER, 0x00, BD_FB_SIZE);
  bd_render_framebuffer();
}

// Initialize the SSD1309 according to 9.
void bd_init() {
  // Display OFF while configuring
  bd_tx_cmd(Set_Display_ON_or_OFF_CMD + Display_OFF);

  // Set display clock (DCLK)
  bd_tx_cmd(Set_Display_Clock_CMD);
  bd_tx_cmd(0x80); // default

  // Set multiplex ratio
  bd_tx_cmd(Set_Multiplex_Ratio_CMD);
  bd_tx_cmd(0x3F);

  // Set display offest
  bd_tx_cmd(Set_Display_Offset_CMD);
  bd_tx_cmd(0x00);

  // Set display start line
  bd_tx_cmd(Set_Display_Start_Line_CMD | 0x00);

  // Charge pump
  bd_tx_cmd(Set_Charge_Pump_CMD);
  bd_tx_cmd(0x14);

  // Memory addressing mode (page mode)
  bd_tx_cmd(Set_Memory_Addressing_Mode_CMD);
  bd_tx_cmd(Page_Addressing_Mode);

  // Segment remap
  bd_tx_cmd(Set_Segment_Remap_CMD | Column_Address_0_Mapped_to_SEG127);

  // COM scan direction (vertical flip)
  bd_tx_cmd(Set_COM_Output_Scan_Direction_CMD | Scan_from_COM63_to_0);

  // COM Pin config
  bd_tx_cmd(Set_Common_HW_Config_CMD);
  bd_tx_cmd(0x12);

  // Set contrast
  bd_tx_cmd(Set_Contrast_Control_CMD);
  bd_tx_cmd(0xCF);

  // Set pre-charge period
  bd_tx_cmd(Set_Pre_charge_Period_CMD);
  bd_tx_cmd(0xF1);

  // Set VCOMH level
  bd_tx_cmd(Set_VCOMH_Level_CMD);
  bd_tx_cmd(0x40);

  // Normal display (all pixels from RAM)
  bd_tx_cmd(Set_Entire_Display_ON_CMD | Normal_Display);

  // Non_Inverted_Display
  bd_tx_cmd(Set_Normal_or_Inverse_Display_CMD | Non_Inverted_Display);

  // Display ON
  bd_tx_cmd(Set_Display_ON_or_OFF_CMD + Display_ON);

  bd_fill_framebuffer(false);
  bd_render_framebuffer();
}
