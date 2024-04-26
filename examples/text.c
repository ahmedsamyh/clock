#include <clock/clock.h>
#include <assert.h>

int main(void) {
  Context* ctx = clock_init(1280, 720, 1.f, 1.f, "Text", 0);
  if (ctx == NULL) {
    return 1;
  }

  Font font = {0};
  if (!Font_init_from_file(&font, ctx, "resources/fonts/WayfarersToyBoxRegular-gxxER.ttf")) {
    return 1;
  }

  cstr text = "The quick brown fox jumps over the lazy dog";

  int char_size = 32;

  while (!clock_should_quit(ctx)) {
    clock_begin_draw(ctx);
    clock_clear(ctx, color_from_hex(0xFF141414));

    if (clock_key_pressed(ctx, KEY_W)) {
      char_size += 1;
    }
    if (clock_key_pressed(ctx, KEY_W)) {
      char_size -= 1;
      if (char_size <= 2) char_size = 2;
    }
    draw_text(ctx, &font, text, (Vector2f) {50.f, 50.f}, char_size, COLOR_WHITE);

    clock_end_draw(ctx);
  }


  Font_deinit(&font);
  clock_deinit(ctx);

  return 0;
}
