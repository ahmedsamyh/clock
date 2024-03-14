#include <clock/clock.h>

int main(void) {
  Context* ctx = clock_init(800, 800, "Clock");

  if (ctx == NULL) {
    return 1;
  }

  while (!clock_should_quit(ctx)) {
    clock_begin_draw(ctx);
    clock_clear(ctx, COLOR_BLACK);

    clock_end_draw(ctx);
  }

  clock_deinit(ctx);

  return 0;
}
