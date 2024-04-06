#include <clock/clock.h>

int main(void) {
  Context* ctx = clock_init(1280, 720, 1.f, 1.f, "Text Input", RENDER_MODE_2D);
  if (!ctx) {
    return 1;
  }

  Color colors[] = {
    COLOR_BLACK,
    COLOR_BLANK,
    COLOR_WHITE,
    COLOR_RED,
    COLOR_GREEN,
    COLOR_BLUE,
    COLOR_MAGENTA,
    COLOR_YELLOW,
    COLOR_CYAN,
    COLOR_GOLD,
    COLOR_ORANGE,
    COLOR_GREY,
    COLOR_MIDNIGHT_BLUE,
  };

  uint32 i = 0;

  while (!clock_should_quit(ctx)) {
    clock_begin_draw(ctx);
    ASSERT(i <= ARRAY_LEN(colors)-1);
    clock_clear(ctx, colors[i]);

    if (clock_key_pressed(ctx, KEY_SPACE)) {
      i = (i + 1) % (ARRAY_LEN(colors)-1);
    }

    if (ctx->mscroll.y >= 1.f) {
      i = (i + 1) % (ARRAY_LEN(colors)-1);
    }

    clock_end_draw(ctx);
  }

  clock_deinit(ctx);

  return 0;
}
