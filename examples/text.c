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

    cstr text = "The quick brown fox jumps over the lazy dog";
    draw_text(ctx, &font, text, ctx->mpos, 32, (Color){0.4f, 0.f, 0.5f, 1.f});

    Rect rect = {
      .pos = ctx->mpos,
      .size = get_text_size(ctx, &font, text, 32)
    };

    draw_rect(ctx, rect, color_alpha(COLOR_GREEN, 0.4f));

    clock_end_draw(ctx);
  }


  Font_deinit(&font);
  clock_deinit(ctx);

  return 0;
}
