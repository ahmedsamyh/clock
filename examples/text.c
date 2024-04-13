#include <clock/clock.h>
#include <assert.h>

int main(void) {
  Context* ctx = clock_init(1280, 720, 1.f, 1.f, "Text", 0);
  if (ctx == NULL) {
    return 1;
  }

  Font font = {0};
  if (!Font_init_from_file(&font, ctx, "resources/fonts/iosevka-regular.ttf")) {
    return 1;
  }

  cstr text = "The quick brown fox jumps over the lazy dog";

  while (!clock_should_quit(ctx)) {
    clock_begin_draw(ctx);
    clock_clear(ctx, color_from_hex(0xFF141414));

    float y = 0.f;
    int char_size = 32;
    if (clock_key_held(ctx, KEY_SPACE)) {
      for (size_t i = 0; i < 4; ++i) {
	draw_text(ctx, &font, text, (Vector2f) {ctx->mpos.x / char_size, y}, char_size, COLOR_MAGENTA);
	y += char_size;
	char_size -= 1;
	draw_text(ctx, &font, text, (Vector2f) {ctx->mpos.x / char_size, y}, char_size, COLOR_GOLD);
	y += char_size;
	char_size -= 1;
	draw_text(ctx, &font, text, (Vector2f) {ctx->mpos.x / char_size, y}, char_size, COLOR_ORANGE);
	y += char_size;
	char_size -= 1;
	draw_text(ctx, &font, text, (Vector2f) {ctx->mpos.x / char_size, y}, char_size, COLOR_CYAN);
      }
    }

    clock_end_draw(ctx);
  }


  Font_deinit(&font);
  clock_deinit(ctx);

  return 0;
}
