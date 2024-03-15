#include <clock/clock.h>
#include <rpg/player.h>

int main(void) {
  Context* ctx = clock_init(1280, 960, "RPG");

  if (ctx == NULL) {
    return 1;
  }

  Player player = {0};

  Player_init(&player, ctx);

  /* glfwSwapInterval(1); */

  while (!clock_should_quit(ctx)) {
    clock_begin_draw(ctx);
    clock_clear(ctx, COLOR_BLACK);

    Player_control(&player);
    Player_update (&player);
    Player_draw   (&player);

    clock_end_draw(ctx);
  }

  clock_deinit(ctx);

  return 0;
}
