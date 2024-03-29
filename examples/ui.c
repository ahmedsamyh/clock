#include <clock/clock.h>

int main(void) {
  Context* ctx = clock_init(1280, 720, 1.f, 1.f, "UI", RENDER_MODE_2D);

  if (!ctx) return 1;

  Font font = {0};
  if (!Font_init(&font, ctx, "resources/fonts/WayfarersToyBoxRegular-gxxER.ttf")) return 1;

  UI ui = UI_make(ctx, &font);

  while (!clock_should_quit(ctx)) {
    clock_begin_draw(ctx);
    clock_clear(ctx, COLOR_BLACK);

    UI_begin(&ui, (Vector2f) {10.f, 0.f}, UI_LAYOUT_KIND_VERT);

    UI_text(&ui, "Buttons", 32, COLOR_WHITE);
    UI_spacing(&ui, 32.f);

    if (UI_button(&ui, "Press me 1", 24, COLOR_RED)) {
      log_f(LOG_INFO, "Pressed 1");
    }

    if (UI_button(&ui, "Press me 2", 24, COLOR_GREEN)) {
      log_f(LOG_INFO, "Pressed 2");
    }

    if (UI_button(&ui, "Press me 3", 24, COLOR_YELLOW)) {
      log_f(LOG_INFO, "Pressed 3");
    }

    UI_end(&ui);

    clock_end_draw(ctx);
  }

  Font_deinit(&font);
  clock_deinit(ctx);

  return 0;
}
