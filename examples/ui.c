#include <clock/clock.h>

int main(void) {
  Context* ctx = clock_init(1280, 720, 1.f, 1.f, "UI", RENDER_MODE_2D);

  if (!ctx) return 1;

  Font font = {0};
  if (!Font_init(&font, ctx, "resources/fonts/WayfarersToyBoxRegular-gxxER.ttf")) return 1;

  UI ui = UI_make(ctx, &font);

  Sprite spr = {0};
  if (!Sprite_init(&spr, Resman_load_texture_from_file(ctx->resman, "resources/gfx/flower.png"), 1, 1)) return 1;

#define text_input_size (1024)
  char text_input[text_input_size] = {0};
  uint32 text_cursor = 0;


  Vector2f ui_pos = {100.f, 100.f};

  while (!clock_should_quit(ctx)) {
    clock_begin_draw(ctx);
    clock_clear(ctx, COLOR_MIDNIGHT_BLUE);

    UI_begin(&ui, &ui_pos, UI_LAYOUT_KIND_VERT);

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

    UI_text_input(&ui, text_input, text_input_size, &text_cursor, 24, COLOR_YELLOW);

    if (UI_button(&ui, "Submit", 24, COLOR_WHITE)) {
      log_f(LOG_INFO, "%s", text_input);
    }

    UI_background(&ui);

    UI_end(&ui);

    clock_end_draw(ctx);
  }

  Sprite_deinit(&spr);
  Font_deinit(&font);
  clock_deinit(ctx);

  return 0;
}
