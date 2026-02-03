#include "babydriver.h"
#include "gojo.h"
#include <stdio.h>

int main() {
  bd_draw_bitmap(0, 0, GOJO_WIDTH, GOJO_HEIGHT, gojo);

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
