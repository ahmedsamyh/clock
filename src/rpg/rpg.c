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
  clock_set_vsync(false);

  bool DEBUG_DRAW = false;
#ifdef DEBUG
  DEBUG_DRAW = true;
#endif

  int current_blendmode = (int)BLENDMODE_NORMAL;

  while (!clock_should_quit(ctx)) {

    clock_begin_draw(ctx);

    clock_clear(ctx, hex_to_color(0xFF141414));

    if (ctx->keys[GLFW_KEY_GRAVE_ACCENT].pressed) DEBUG_DRAW = !DEBUG_DRAW;

    if (ctx->keys[GLFW_KEY_SPACE].pressed) {
      current_blendmode = (current_blendmode + 1) % BLENDMODE_COUNT;
    }

    /* Player_control(&player); */

    /* for (size_t i = 0; i < 2; ++i) { */
    /*   Vector2f arm_pos = player.arm_spr[i].pos; */
    /*   Vector2f diff = v2f_sub(arm_pos, ctx->mpos); */
    /*   player.arm_spr[i].rotation.z = v2f_degree(diff) + 90.f; */
    /* } */

    /* Player_update (&player); */
    /* Player_draw   (&player, DEBUG_DRAW); */

    set_blend_mode(BLENDMODE_ALPHA);

    draw_rect(ctx, (Rect){
	.pos = {10.f, 100.f},
	.size = {100.f, 100.f}},
      COLOR_RED);

    set_blend_mode((Blendmode)current_blendmode);

    spr.pos = ctx->mpos;
    draw_sprite(ctx, &spr);

    clock_end_draw(ctx);
  }

  Sprite_deinit(&spr);
  clock_deinit(ctx);

  return 0;
}
