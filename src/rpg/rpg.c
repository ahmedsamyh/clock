#include <clock/clock.h>
#include <rpg/player.h>

Texture* load_texture_err_handled(Context* ctx, const char* filepath) {
  Texture* tex = Resman_load_texture(ctx->resman, filepath);
  if (tex == NULL) exit(1);
  return tex;
}

int main(void) {
  Context* ctx = clock_init(1280, 960, 1.f, 1.f, "RPG");

  float width = (float)ctx->win->width;
  float height = (float)ctx->win->height;

  if (ctx == NULL) {
    return 1;
  }

  Texture* flower_tex = load_texture_err_handled(ctx, "resources/gfx/flower.png");

  Sprite spr = {0};
  if (!Sprite_init(&spr, flower_tex, 1, 1)) return 1;
  Sprite_center_origin(&spr);

  Player player = {0};

  /* if (!Player_init(&player, ctx, player_head_tex, player_torso_tex, player_arm_tex, player_leg_tex)) return 1; */

  player.pos.x = width  * 0.5f;
  player.pos.y = height * 0.5f;

  // vsync
  /* glfwSwapInterval(1); */

  int viewport[4] = {0};
  gl(glGetIntegerv(GL_VIEWPORT, (int*)&viewport));

  log_f(LOG_INFO, "viewport: (%d, %d) (%d, %d)", viewport[0], viewport[1], viewport[2], viewport[3]);

  gl(glEnable(GL_BLEND));

  bool DEBUG_DRAW = false;
#ifdef DEBUG
  DEBUG_DRAW = true;
#endif

  while (!clock_should_quit(ctx)) {
    clock_begin_draw(ctx);

    clock_clear(ctx, COLOR_BLACK);

    if (ctx->keys[GLFW_KEY_GRAVE_ACCENT].pressed) DEBUG_DRAW = !DEBUG_DRAW;

    // alpha blending
    gl(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    gl(glBlendEquation(GL_FUNC_ADD));

    /* Player_control(&player); */

    /* for (size_t i = 0; i < 2; ++i) { */
    /*   Vector2f arm_pos = player.arm_spr[i].pos; */
    /*   Vector2f diff = v2f_sub(arm_pos, ctx->mpos); */
    /*   player.arm_spr[i].rotation.z = v2f_degree(diff) + 90.f; */
    /* } */

    /* Player_update (&player); */
    /* Player_draw   (&player, DEBUG_DRAW); */

    spr.pos = ctx->mpos;
    draw_sprite(ctx, &spr);

    clock_end_draw(ctx);
  }

  Sprite_deinit(&spr);
  clock_deinit(ctx);

  return 0;
}
