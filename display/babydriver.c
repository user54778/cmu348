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
