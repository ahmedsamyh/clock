#include <clock/clock.h>

int main(void) {
  Context* ctx = clock_init(800, 800, 1.f, 1.f, "UI", WINDOW_RESIZABLE_ASPECT);

  if (!ctx) return 1;

  Font font = {0};
  if (!Font_init(&font, ctx, "resources/fonts/WayfarersToyBoxRegular-gxxER.ttf")) return 1;

  UI ui = UI_make(ctx, &font, (Vector2f) {100.f, 100.f}, "UI window");

  Sprite spr = {0};
  if (!Sprite_init(&spr, Resman_load_texture_from_file(ctx->resman, "resources/gfx/flower.png"), 1, 1)) return 1;

#define text_input_size (1024)
  char text_input[text_input_size] = {0};
  uint32 text_cursor = 0;

  while (!clock_should_quit(ctx)) {
    clock_begin_draw(ctx);
    clock_clear(ctx, COLOR_MIDNIGHT_BLUE);

    UI_begin(&ui, UI_LAYOUT_KIND_VERT);

    UI_text(&ui, "Big Text", 32, COLOR_WHITE);
    UI_text(&ui, "Smol Text", 12, COLOR_WHITE);
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

    /* cstr a; */
    /* temp_sprintf(a, "ui->show_text_input_cursor: %s", (ui.show_text_input_cursor ? "true" : "false")); */
    /* UI_text(&ui, a, 24, COLOR_WHITE); */

    UI_end(&ui);

    if (clock_mouse_pressed(ctx, MOUSE_BUTTON_LEFT)) {
      log_f(LOG_INFO, "Left mouse button pressed!");
    }

    clock_end_draw(ctx);
  }

  UI_free(&ui);
  Sprite_deinit(&spr);
  Font_deinit(&font);
  clock_deinit(ctx);

  return 0;
}
