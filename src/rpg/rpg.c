#include <clock/clock.h>
#include <rpg/player.h>

Texture* load_texture_err_handled(Context* ctx, const char* filepath) {
  Texture* tex = Resman_load_texture(ctx->resman, filepath);
  if (tex == NULL) exit(1);
  return tex;
}
int main(void) {
  Context* ctx = clock_init(1280, 960, "RPG");

  if (ctx == NULL) {
    return 1;
  }

  Texture* player_head_tex  = load_texture_err_handled(ctx, "resources/gfx/player_head.png");
  Texture* player_torso_tex = load_texture_err_handled(ctx, "resources/gfx/player_torso.png");
  Texture* player_arm_tex   = load_texture_err_handled(ctx, "resources/gfx/player_arm.png");
  Texture* player_leg_tex   = load_texture_err_handled(ctx, "resources/gfx/player_leg.png");

  Player player = {0};

  if (!Player_init(&player, ctx, player_head_tex, player_torso_tex, player_arm_tex, player_leg_tex)) return 1;

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
