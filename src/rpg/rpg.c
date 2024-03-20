#include <clock/clock.h>
#include <rpg/player.h>
#include <rpg/enemy.h>
#include <rpg/tile.h>
#include <rpg/common.h>

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
  Texture* tiles_tex  = load_texture_err_handled(ctx, "resources/gfx/tiles.png");

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

  Enemy* enemies = NULL; // dynamic array
  Tile*  tiles   = NULL; // dynamic array

  while (!clock_should_quit(ctx)) {

    clock_begin_draw(ctx);

    clock_clear(ctx, hex_to_color(0xFF555555));

    //
    // Update
    //

    if (ctx->keys[GLFW_KEY_GRAVE_ACCENT].pressed) DEBUG_DRAW = !DEBUG_DRAW;

    Player_control(&player);
    Player_update (&player);

    // TEMP
    if (ctx->keys[GLFW_KEY_SPACE].pressed) {
      Enemy e = {0};
      if (!Enemy_init(&e, ctx, player_tex)) return 1;
      e.pos = ctx->mpos;
      arrput(enemies, e);
    }

    // TEMP
    if (ctx->keys[GLFW_KEY_X].pressed) {
      bool contains = false;
      for (size_t i = 0; i < arrlenu(tiles); ++i) {
	Rect tile_rect = (Rect) {
	  .pos = tiles[i].pos,
	  .size = tiles[i].size,
	};

	if (Rect_contains_point(tile_rect, ctx->mpos)) {
	  contains = true;
	  break;
	}
      }

      if (!contains) {
	Tile t = {0};
	if (!Tile_init(&t, 0, ctx, tiles_tex)) return 1;
	t.pos = pos_in_tile_space(ctx->mpos);
	arrput(tiles, t);
	log_f(LOG_INFO, "Tile added!");
      }
    }

    for (int i = arrlen(enemies) - 1; i >= 0; --i) {
      Enemy_update(&enemies[i]);
    }

    player.hitting = false;
    for (int i = arrlen(tiles) - 1; i >= 0; --i) {
      Rect tile_rect = (Rect) {
	.pos = tiles[i].pos,
	.size = tiles[i].size,
      };

      if (Rect_resolve_rect_collision(&player.hitbox, tile_rect)) {
	Player_set_pos_to_hitbox_pos(&player);
	player.hitting = true;
      }

      Tile_update(&tiles[i]);
    }

    //
    // Draw
    //

    for (int i = arrlen(enemies) - 1; i >= 0; --i) {
      Enemy_draw(&enemies[i], DEBUG_DRAW);
    }

    for (int i = arrlen(tiles) - 1; i >= 0; --i) {
      Tile_draw(&tiles[i], DEBUG_DRAW);
    }

    Player_draw(&player, DEBUG_DRAW);

    clock_end_draw(ctx);
  }

  arrfree(enemies);
  arrfree(tiles);

  clock_deinit(ctx);

  return 0;
}
