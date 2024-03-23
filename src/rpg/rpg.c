#include <clock/clock.h>
#include <rpg/player.h>
#include <rpg/enemy.h>
#include <rpg/tile.h>
#include <rpg/common.h>
#include <assert.h>

Texture* load_texture_err_handled(Context* ctx, const char* filepath) {
  Texture* tex = Resman_load_texture_from_file(ctx->resman, filepath);
  if (tex == NULL) exit(1);
  return tex;
}

#define add_tile(type)							\
  bool contains = false;						\
  for (size_t i = 0; i < arrlenu(tiles); ++i) {				\
    Rect tile_rect = (Rect) {						\
      .pos = tiles[i].pos,						\
      .size = tiles[i].size,						\
    };									\
    if (Rect_contains_point(tile_rect, ctx->mpos)) {			\
      contains = true;							\
      break;								\
    }									\
  }									\
  Texture* tiles_tex = load_texture_err_handled(ctx, "resources/gfx/tiles.png"); \
  if (!contains) {							\
    Tile t = {0};							\
    if (!Tile_init(&t, type, ctx, tiles_tex)) return false;		\
    t.pos = pos_in_tile_space(ctx->mpos);				\
    arrput(tiles, t);							\
    log_f(LOG_INFO, "Tile added!");					\
  }

#define remove_tile() \
  bool contains = false;						\
  size_t containing_idx = 0;						\
  for (size_t i = 0; i < arrlenu(tiles); ++i) {				\
    Rect tile_rect = (Rect) {						\
      .pos = tiles[i].pos,						\
      .size = tiles[i].size,						\
    };									\
    if (Rect_contains_point(tile_rect, ctx->mpos)) {			\
      contains = true;							\
      containing_idx = i;						\
      break;								\
    }									\
  }									\
  if (contains) {							\
    arrdel(tiles, containing_idx);					\
  }

typedef enum {
  STATE_PLAY,
  STATE_EDIT,
  STATE_COUNT
} State;

bool change_state(Context* ctx, State* current_state, State next_state, Sprite* state_spr, float* state_show_timer, float state_show_time) {
  switch (next_state) {
  case STATE_PLAY: {
    if (!Sprite_init_scaled(state_spr, load_texture_err_handled(ctx, "resources/gfx/play_state.png"), 1, 1)) return false;
  } break;
  case STATE_EDIT: {
    if (!Sprite_init_scaled(state_spr, load_texture_err_handled(ctx, "resources/gfx/edit_state.png"), 1, 1)) return false;
  } break;
  default: assert(0 && "Unreachable");
  }

  *current_state = next_state;
  *state_show_timer = state_show_time;

  return true;
}

int main(void) {
  Context* ctx = clock_init(1280, 960, 1.f, 1.f, "RPG");

  float width = (float)ctx->win->width;
  float height = (float)ctx->win->height;
  Vector2f screen_size = {width, height};

  if (ctx == NULL) {
    return 1;
  }

  Texture* player_tex = load_texture_err_handled(ctx, "resources/gfx/player.png");
  Texture* tiles_tex  = load_texture_err_handled(ctx, "resources/gfx/tiles.png");
  Texture* edit_state_tex = load_texture_err_handled(ctx, "resources/gfx/edit_state.png");
  Texture* play_state_tex = load_texture_err_handled(ctx, "resources/gfx/play_state.png");

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

  Sprite state_spr = {0};

  float state_show_timer = 0.f;
  float state_show_time  = 4.f; // seconds

  State current_state;
  if (!change_state(ctx, &current_state, STATE_PLAY, &state_spr, &state_show_timer, state_show_time)) return 1;

  // Edit
  Vector2i tile_type          = {0};
  Vector2i hovering_tile_type = {0};
  Sprite tiles_spr = {0};
  if (!Sprite_init_scaled(&tiles_spr, tiles_tex, 1, 1)) return 1;

  Rect tiles_rect = {
    .pos  = tiles_spr.pos,
    .size = v2f_mul(tiles_spr.size, tiles_spr.scale),
  };

  init_tiles_texture(tiles_tex);

  while (!clock_should_quit(ctx)) {

    clock_begin_draw(ctx);

    clock_clear(ctx, hex_to_color(0xFF555555));

    //
    // Update
    //
    if (ctx->k[KEY_GRAVE_ACCENT].pressed) DEBUG_DRAW = !DEBUG_DRAW;
    if (ctx->k[KEY_TAB].pressed) {
      if (!change_state(ctx, &current_state, (current_state + 1) % STATE_COUNT, &state_spr, &state_show_timer, state_show_time)) return 1;
    }

    switch (current_state) {
    case STATE_PLAY: {
      Player_control(&player);
      Player_update (&player);

      /* ctx->camera.x = width/2.f -  player.pos.x; */
      /* ctx->camera.y = height/2.f - player.pos.y; */

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
    } break;
    case STATE_EDIT: {
      for (int i = arrlen(tiles) - 1; i >= 0; --i) {
	Tile_update(&tiles[i]);
      }

      if (ctx->k[KEY_LEFT_SHIFT].held) {
	if (Rect_contains_point(tiles_rect, ctx->mpos)) {
	  hovering_tile_type.x = (int)ctx->mpos.x / (int)TILE_SIZE;
	  hovering_tile_type.y = (int)ctx->mpos.y / (int)TILE_SIZE;

	  if (ctx->m[MOUSE_BUTTON_LEFT].pressed) {
	    tile_type = hovering_tile_type;
	  }
	}
      } else {
	if (ctx->m[MOUSE_BUTTON_LEFT].pressed) {
	  add_tile(tile_type);
	}

	if (ctx->m[MOUSE_BUTTON_RIGHT].pressed) {
	  remove_tile();
	}
      }

    } break;
    default: assert(0 && "Unreachable");
    }

    //
    // Draw
    //

    switch (current_state) {
    case STATE_PLAY: {
      for (int i = arrlen(enemies) - 1; i >= 0; --i) {
	Enemy_draw(&enemies[i], DEBUG_DRAW);
      }

      for (int i = arrlen(tiles) - 1; i >= 0; --i) {
	Tile_draw(&tiles[i], DEBUG_DRAW);
      }

      Player_draw(&player, DEBUG_DRAW);
    } break;
    case STATE_EDIT: {
      for (int i = arrlen(tiles) - 1; i >= 0; --i) {
	Tile_draw(&tiles[i], DEBUG_DRAW);
      }

      if (ctx->k[KEY_LEFT_SHIFT].held) {
	draw_rect(ctx, (Rect){(Vector2f){0.f, 0.f}, screen_size}, color_alpha(COLOR_BLACK, 0.8f));

	draw_sprite(ctx, &tiles_spr);
	Color color = COLOR_RED;

	Vector3f p0, p1, p2, p3;
	Rect_get_points((Rect){tiles_spr.pos, v2f_mul(tiles_spr.size, tiles_spr.scale)}, &p0, &p1, &p2, &p3);
	draw_imm_box(ctx, p0, p1, p2, p3, color, color, color, color);

	Rect r = {
	  .pos =  v2f_muls((Vector2f){tile_type.x, tile_type.y}, TILE_SIZE),
	  .size = (Vector2f){TILE_SIZE, TILE_SIZE}
	};
	draw_rect(ctx, r, color_alpha(COLOR_GREEN, 0.1f));

	r.pos = v2f_muls((Vector2f){hovering_tile_type.x, hovering_tile_type.y}, TILE_SIZE);
	draw_rect(ctx, r, color_alpha(COLOR_RED, 0.1f));
      }

      Player_draw(&player, DEBUG_DRAW);

    } break;
    default: assert(0 && "Unreachable");
    }

    if (state_show_timer > 0.f) {
      state_spr.tint.a = (state_show_timer / state_show_time);
      draw_sprite(ctx, &state_spr);
      state_show_timer -= ctx->delta;
    }

    clock_end_draw(ctx);
  }

  arrfree(enemies);
  arrfree(tiles);

  clock_deinit(ctx);

  return 0;
}
