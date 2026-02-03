#include "babydriver.h"

// Our 1-D array framebuffer
uint8_t BD_FRAMEBUFFER[BD_FB_SIZE] = {0};

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
void bd_set_spi_dc(void (*spi_cd)(bool state)) { bd_set_dc = spi_cd; }
void bd_set_spi_cs(void (*spi_cs)(bool state)) { bd_set_cs = spi_cs; }

// Transmit SPI cmd
void bd_tx_cmd(uint8_t cmd) {
  bd_set_dc(false);
  bd_set_cs(false);
  bd_spi_tx(cmd);
  bd_set_cs(true);
}

// Transmit SPI data
void bd_tx_data(uint8_t data) {
  bd_set_dc(true);
  bd_set_cs(false);
  bd_spi_tx(data);
  bd_set_cs(true);
}

void bd_power_on() { bd_tx_cmd(BD_SSD1306_DISPLAY_ON); }

void bd_power_off() { bd_tx_cmd(BD_SSD1306_DISPLAY_OFF); }

void bd_render_framebuffer() {
  // 8 pages for the SSD1306
  for (uint8_t page = 0; page < 8; page++) {
    // set page address
    bd_tx_cmd(BD_SSD1306_SET_PAGE_START_ADDR | page);
    // set column address (lower start and upper start)
    // specified via 10.3
    //"For example, if the page address is set to B2h, lower column address is
    // 03h and upper column address is 10h,
    // then that means the starting column is SEG3 of PAGE2."
    bd_tx_cmd(BD_SSD1306_SET_LOWER_COLUMN | 0x00);
    bd_tx_cmd(BD_SSD1306_SET_HIGHER_COLUMN | 0x00);
    // send 0-128 columns (bytes) for this page
    for (uint8_t col = 0; col < 128; col++) {
      // pg 0 = 0 - 127
      // ....
      // pg 7 = 896 - 1023
      bd_tx_data(BD_FRAMEBUFFER[page * 128 + col]);
    }
  }
}

void bd_clear() {
  memset(BD_FRAMEBUFFER, 0x00, BD_FB_SIZE);
  bd_render_framebuffer();
}

void bd_init(void) {
  // Hardware reset (if you have a reset pin)
  // bd_set_reset(false);
  // HAL_Delay(10);
  // bd_set_reset(true);
  // HAL_Delay(10);

  // Send initialization sequence
  bd_tx_cmd(BD_SSD1306_DISPLAY_OFF);

  // Timing & Driving Scheme
  bd_tx_cmd(BD_SSD1306_SET_DISPLAY_CLOCK);
  bd_tx_cmd(0x80);

  bd_tx_cmd(BD_SSD1306_SET_MULTIPLEX_RATIO);
  bd_tx_cmd(0x3F); // 64 lines

  bd_tx_cmd(BD_SSD1306_SET_DISPLAY_OFFSET);
  bd_tx_cmd(0x00);

  bd_tx_cmd(BD_SSD1306_SET_START_LINE | 0x00);

  // Charge Pump
  bd_tx_cmd(BD_SSD1306_CHARGE_PUMP_SETTING);
  bd_tx_cmd(BD_SSD1306_CHARGE_PUMP_ENABLE);

  // Memory Addressing Mode
  bd_tx_cmd(BD_SSD1306_MEMORY_ADDR_MODE);
  bd_tx_cmd(BD_SSD1306_ADDR_MODE_PAGE); // Page addressing mode

  // Hardware Configuration
  bd_tx_cmd(BD_SSD1306_SET_SEGMENT_REMAP_127); // Flip horizontal
  bd_tx_cmd(BD_SSD1306_COM_SCAN_DIR_REMAP);    // Flip vertical

  bd_tx_cmd(BD_SSD1306_SET_COM_PINS);
  bd_tx_cmd(0x12);

  // Display Settings
  bd_tx_cmd(BD_SSD1306_SET_CONTRAST);
  bd_tx_cmd(0x7F); // Mid contrast

  bd_tx_cmd(BD_SSD1306_SET_PRECHARGE_PERIOD);
  bd_tx_cmd(0xF1);

  bd_tx_cmd(BD_SSD1306_SET_VCOMH_DESELECT);
  bd_tx_cmd(0x40);

  bd_tx_cmd(BD_SSD1306_DISPLAY_ALL_ON_RESUME);
  bd_tx_cmd(BD_SSD1306_DISPLAY_NORMAL);

  // Turn on display
  bd_tx_cmd(BD_SSD1306_DISPLAY_ON);

  // Clear display
  bd_clear();
}
