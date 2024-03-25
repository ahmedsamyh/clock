#include <clock/clock.h>
#include <rpg/player.h>
#include <rpg/stage.h>
#include <rpg/enemy.h>
#include <rpg/tile.h>
#include <rpg/common.h>
#include <assert.h>

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
  Texture* font_tex = load_texture_err_handled(ctx, "resources/gfx/spr_font.png");

  Sprite font_spr = {0};
  if (!Sprite_init_scaled(&font_spr, font_tex, 95, 1)) return 1;
  cstr font_map = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^ _`abcdefghijklmnopqrstuvwxyz{|}~";
  Vector2f p = {0};

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
  Stage stage = {0};
  Stage_init(&stage, ctx, "stage 1");

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

    clock_clear(ctx, COLOR_BLACK);

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

      // TEMP:
      const float speed = 400.f;
      if (ctx->k[KEY_LEFT].held) {
	p.x -= speed * ctx->delta;
      }
      if (ctx->k[KEY_RIGHT].held) {
	p.x += speed * ctx->delta;
      }
      if (ctx->k[KEY_UP].held) {
	p.y -= speed * ctx->delta;
      }
      if (ctx->k[KEY_DOWN].held) {
	p.y += speed * ctx->delta;
      }

      for (int i = arrlen(enemies) - 1; i >= 0; --i) {
	Enemy_update(&enemies[i]);
      }

      Stage_update(&stage, &player);

    } break;
    case STATE_EDIT: {
      Stage_update(&stage, NULL);

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
	  Stage_add_tile(&stage, tile_type);
	}

	if (ctx->m[MOUSE_BUTTON_RIGHT].pressed) {
	  Stage_remove_tile(&stage);
	}
      }

      if (ctx->k[KEY_LEFT_CONTROL].held && ctx->k[KEY_S].pressed) {
	if (!Stage_save_to_file(&stage)) return 1;
      }

      if (ctx->k[KEY_LEFT_CONTROL].held && ctx->k[KEY_L].pressed) {
	if (!Stage_load_from_file(&stage)) return 1;
      }

    } break;
    default: assert(0 && "Unreachable");
    }

    //
    // Draw
    //

    switch (current_state) {
    case STATE_PLAY: {
      clock_clear(ctx, hex_to_color(0xFF555555));
      Stage_draw(&stage, DEBUG_DRAW);

      for (int i = arrlen(enemies) - 1; i >= 0; --i) {
	Enemy_draw(&enemies[i], DEBUG_DRAW);
      }

      Player_draw(&player, DEBUG_DRAW);

      draw_spr_text(ctx, &font_spr, "Hello, World", p, COLOR_WHITE, font_map);

    } break;
    case STATE_EDIT: {
      clock_clear(ctx, hex_to_color(0xFF5555AA));
      Stage_draw(&stage, DEBUG_DRAW);

      if (ctx->k[KEY_LEFT_SHIFT].held) {
	draw_rect(ctx, (Rect){(Vector2f){0.f, 0.f}, screen_size}, color_alpha(COLOR_BLACK, 0.8f));

	draw_sprite(ctx, &tiles_spr);
	Color color = COLOR_RED;

	Vector2f p0, p1, p2, p3;
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

  clock_deinit(ctx);

  return 0;
}
