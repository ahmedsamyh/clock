#include <clock/clock.h>

int main(void) {
  Context* ctx = clock_init(1280, 720, 1.f, 1.f, "UI", RENDER_MODE_2D);

  if (!ctx) return 1;

  Font font = {0};
  if (!Font_init(&font, ctx, "resources/fonts/WayfarersToyBoxRegular-gxxER.ttf")) return 1;

  UI ui = UI_make(ctx, &font);

  Sprite spr = {0};
  if (!Sprite_init(&spr, Resman_load_texture_from_file(ctx->resman, "resources/gfx/flower.png"), 1, 1)) return 1;

  char* text_input = "Hello, this is a long text.";
  int s = strlen((cstr)text_input);

  while (!clock_should_quit(ctx)) {
    clock_begin_draw(ctx);
    clock_clear(ctx, COLOR_BLACK);

    if (ctx->k[KEY_SPACE].pressed) {
      if (s >= 0) {
	text_input[s--] = '\0';
      }
    }

    UI_begin(&ui, (Vector2f) {100.f, 0.f}, UI_LAYOUT_KIND_VERT);

    UI_text(&ui, "Buttons", 32, COLOR_WHITE);
    UI_spacing(&ui, 32.f);

    {
      UI_begin_layout(&ui, UI_LAYOUT_KIND_HORZ);
      if (UI_button(&ui, "Press me 1", 24, COLOR_RED)) {
	log_f(LOG_INFO, "Pressed 1");
      }

      if (UI_button(&ui, "Press me 2", 24, COLOR_GREEN)) {
	log_f(LOG_INFO, "Pressed 2");
      }
      UI_end_layout(&ui);
    }

    if (UI_button(&ui, "Press me 3", 24, COLOR_YELLOW)) {
      log_f(LOG_INFO, "Pressed 3");
    }

    {
      UI_begin_layout(&ui, UI_LAYOUT_KIND_HORZ);

      UI_sprite(&ui, &spr);

      if (UI_sprite_button(&ui, &spr)) {
	log_f(LOG_INFO, "Sprite pressed");
      }

      UI_end_layout(&ui);
    }

    UI_text_input(&ui, &text_input, 24, COLOR_YELLOW);

    UI_end(&ui);

    clock_end_draw(ctx);
  }

  Sprite_deinit(&spr);
  Font_deinit(&font);
  clock_deinit(ctx);

  return 0;
}
