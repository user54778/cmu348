#include "babydriver.h"
#include "fonts.h"
#include "gojo.h"
#include "kuromi.h"
#include <stdio.h>

int main() {
  // bd_draw_bitmap(0, 0, GOJO_WIDTH, GOJO_HEIGHT, gojo);
  /*
  bd_draw_bitmap(20, 0, KUROMI_SMALL_WIDTH, KUROMI_SMALL_HEIGHT, kuromi_small);
  bd_draw_bitmap(120, 0, KUROMI_SMALL_WIDTH, KUROMI_SMALL_HEIGHT, kuromi_small);
  bd_draw_bitmap(60, 40, KUROMI_SMALL_WIDTH, KUROMI_SMALL_HEIGHT, kuromi_small);
  */
  // bd_draw_bitmap(20, 0, KUROMI_SMALL_WIDTH, KUROMI_SMALL_HEIGHT,
  // kuromi_small);

  bd_draw_bitmap(0, 0, KUROMI_SMALL_WIDTH, KUROMI_SMALL_HEIGHT, List_kuromi);

  // bd_draw_text(0, 0, 16, "hello twin");

  bd_print_framebuffer();

  bd_init();
}
