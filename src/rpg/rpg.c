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

  Texture* player_tex = load_texture_err_handled(ctx, "resources/gfx/player.png");

  Player player = {0};

  if (!Player_init(&player, ctx, player_tex)) return 1;

  player.pos.x = width  * 0.5f;
  player.pos.y = height * 0.5f;

  // vsync
  clock_set_vsync(false);

  bool DEBUG_DRAW = false;
#ifdef DEBUG
  DEBUG_DRAW = true;
#endif

  while (!clock_should_quit(ctx)) {

    clock_begin_draw(ctx);

    clock_clear(ctx, hex_to_color(0xFF141414));

    if (ctx->keys[GLFW_KEY_GRAVE_ACCENT].pressed) DEBUG_DRAW = !DEBUG_DRAW;

    Player_control(&player);

    Player_update (&player);
    Player_draw   (&player, DEBUG_DRAW);

    clock_end_draw(ctx);
 }

  clock_deinit(ctx);

  return 0;
}
