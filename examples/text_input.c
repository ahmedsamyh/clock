#include <clock/clock.h>

int main(void) {
  Context* ctx = clock_init(1280, 720, 1.f, 1.f, "Text Input", RENDER_MODE_2D);
  if (!ctx) {
    return 1;
  }

  while (!clock_should_quit(ctx)) {
    clock_begin_draw(ctx);
    clock_clear(ctx, COLOR_BLACK);

    if (ctx->text_entered) {
      printf("%c", (char)ctx->last_entered_character);
    }

    clock_end_draw(ctx);
  }

  clock_deinit(ctx);

  return 0;
}
