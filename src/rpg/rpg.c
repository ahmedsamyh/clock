#include <clock/clock.h>
#include <rpg/player.h>

int main(void) {
  Context* ctx = clock_init(1280, 960, "RPG");

  if (ctx == NULL) {
    return 1;
  }

  const Texture* player_tex = Resman_load_texture(ctx->resman, "resources/gfx/player.png");

  Player player = {0};

  if (!Player_init(&player, ctx, player_tex)) return 1;

  // vsync
  /* glfwSwapInterval(1); */

  gl(glEnable(GL_BLEND));

  while (!clock_should_quit(ctx)) {
    clock_begin_draw(ctx);
    clock_clear(ctx, COLOR_BLACK);

    // alpha blending
    gl(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    gl(glBlendEquation(GL_FUNC_ADD));

    Player_control(&player);
    Player_update (&player);
    Player_draw   (&player);

    clock_end_draw(ctx);
  }

  clock_deinit(ctx);

  return 0;
}
