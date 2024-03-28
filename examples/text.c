#include <clock/clock.h>
#include <assert.h>

int main(void) {
  Context* ctx = clock_init(1280, 720, 1.f, 1.f, "Text", RENDER_MODE_2D);
  if (ctx == NULL) {
    return 1;
  }

  Font font = {0};
  if (!Font_init(&font, ctx, "resources/fonts/WayfarersToyBoxRegular-gxxER.ttf")) {
    return 1;
  }

  while (!clock_should_quit(ctx)) {
    clock_begin_draw(ctx);
    clock_clear(ctx, COLOR_RED);

    draw_text(ctx, &font, "The quick brown fox jumps over the lazy dog", ctx->mpos, 32, (Color){0.4f, 0.f, 0.5f, 1.f});

    clock_end_draw(ctx);
  }


  Font_deinit(&font);
  clock_deinit(ctx);

  return 0;
}