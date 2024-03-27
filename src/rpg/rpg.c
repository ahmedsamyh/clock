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

bool allocate_new_stage(Context* ctx, cstr new_stage_name, Stage_KV** stage_map) {
  Stage_KV* stage_kv = shgetp_null(*stage_map, new_stage_name);

  // The stage we are trying to add already exists...
  if (stage_kv != NULL) {
    log_f(LOG_ERROR, "Adding stage aleady exists!");
    return false;
  }

  Stage stage = {0};
  Stage_init(&stage, ctx, new_stage_name);

  if (Stage_was_saved(&stage)) {
    if (!Stage_load_from_file(&stage)) {
      return false;
    }
  }

  shput(*stage_map, new_stage_name, stage);

  return true;
}

int main(void) {
  Context* ctx = clock_init(1280, 960, 1.f, 1.f, "RPG", RENDER_MODE_2D);

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

  init_tiles_texture(tiles_tex);

  /* Sprite font_spr = {0}; */
  /* if (!Sprite_init_scaled(&font_spr, font_tex, 95, 1)) return 1; */
  /* cstr font_map = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^ _`abcdefghijklmnopqrstuvwxyz{|}~"; */

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
  Stage_KV* stage_map = NULL;  // hashmap
  cstr current_stage_name = "stage 1";

  // We can assert because we are SURE that there is no other stages in the stage_map
  assert(allocate_new_stage(ctx, current_stage_name, &stage_map));

  Sprite state_spr = {0};

  float state_show_timer = 0.f;
  float state_show_time  = 4.f; // seconds

  State current_state;
  if (!change_state(ctx, &current_state, STATE_PLAY, &state_spr, &state_show_timer, state_show_time)) return 1;

  // Edit
  Vector2i tile_type          = {0};
  Vector2i hovering_tile_type = {0};
  bool tile_collidable = true;
  Sprite tiles_spr = {0};
  if (!Sprite_init_scaled(&tiles_spr, tiles_tex, 1, 1)) return 1;

  Rect tiles_rect = {
    .pos  = tiles_spr.pos,
    .size = v2f_mul(tiles_spr.size, tiles_spr.scale),
  };

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

    Stage_KV* current_stage_kv = shgetp_null(stage_map, current_stage_name);
    assert(current_stage_kv != NULL);
    Stage* current_stage = &current_stage_kv->value;

    switch (current_state) {
    case STATE_PLAY: {
      Player_control(&player);
      Player_update (&player);

      for (int i = arrlen(enemies) - 1; i >= 0; --i) {
	Enemy_update(&enemies[i]);
      }

      Stage_update(current_stage, &player);

    } break;
    case STATE_EDIT: {
      Stage_update(current_stage, NULL);

      if (ctx->k[KEY_LEFT_SHIFT].held) {
	if (Rect_contains_point(tiles_rect, ctx->mpos)) {
	  hovering_tile_type.x = (int)ctx->mpos.x / (int)TILE_SIZE;
	  hovering_tile_type.y = (int)ctx->mpos.y / (int)TILE_SIZE;

	  if (ctx->m[MOUSE_BUTTON_LEFT].pressed) {
	    tile_type = hovering_tile_type;
	  }
	}
      } else {
	if (ctx->m[MOUSE_BUTTON_LEFT].held) {
	  Stage_add_tile(current_stage, tile_type, tile_collidable, ctx->mpos);
	}

	// TODO: Have some sort of action history for undo-ing
	if (ctx->m[MOUSE_BUTTON_RIGHT].held) {
	  Stage_remove_tile(current_stage, ctx->mpos);
	}
      }

      if (ctx->k[KEY_C].pressed) {
	tile_collidable = !tile_collidable;
      }

      if (ctx->k[KEY_LEFT_CONTROL].held && ctx->k[KEY_S].pressed) {
	if (!Stage_save_to_file(current_stage)) return 1;
      }

      if (ctx->k[KEY_LEFT_CONTROL].held && ctx->k[KEY_L].pressed) {
	if (!Stage_load_from_file(current_stage)) return 1;
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
      Stage_draw(current_stage, DEBUG_DRAW);

      for (int i = arrlen(enemies) - 1; i >= 0; --i) {
	Enemy_draw(&enemies[i], DEBUG_DRAW);
      }

      Player_draw(&player, DEBUG_DRAW);
    } break;
    case STATE_EDIT: {
      clock_clear(ctx, hex_to_color(0xFF5555AA));
      Stage_draw(current_stage, DEBUG_DRAW);

      if (ctx->k[KEY_LEFT_SHIFT].held) {
	bool prev_state = ctx->use_camera_view;
	clock_use_camera_view(ctx, false);
	draw_rect(ctx, (Rect){(Vector2f){0.f, 0.f}, screen_size}, color_alpha(COLOR_BLACK, 0.8f));
	// TODO: Make draw_sprite_at() func
	Sprite tiles_spr_copy = tiles_spr;
	/* tiles_spr_copy.pos = clock_screen_to_world(ctx, tiles_spr.pos); */
	draw_sprite(ctx, &tiles_spr_copy);
	Color color = COLOR_RED;

	Vector2f p0, p1, p2, p3;
	Rect_get_points((Rect){tiles_spr.pos, v2f_mul(tiles_spr.size, tiles_spr.scale)}, &p0, &p1, &p2, &p3);
	draw_imm_box(ctx, p0, p1, p2, p3, color, color, color, color);

	Rect r = {
	  .pos =  v2f_muls((Vector2f){tile_type.x, tile_type.y}, TILE_SIZE),
	  .size = (Vector2f){TILE_SIZE, TILE_SIZE}
	};
	/* r.pos = clock_screen_to_world(ctx, r.pos); */
	draw_rect(ctx, r, color_alpha(COLOR_GREEN, 0.1f));

	r.pos = v2f_muls((Vector2f){hovering_tile_type.x, hovering_tile_type.y}, TILE_SIZE);
	/* r.pos = clock_screen_to_world(ctx, r.pos); */
	draw_rect(ctx, r, color_alpha(COLOR_RED, 0.1f));
	clock_use_camera_view(ctx, prev_state);
      }

      if (tile_collidable) {
	/* draw_spr_text(ctx, &font_spr, "C", clock_screen_to_world(ctx, (Vector2f){0.f, height - font_spr.tex_rect.size.y}), COLOR_WHITE, font_map); */
	draw_rect(ctx, (Rect){{}, {4,4}}, COLOR_RED);
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
  shfree(stage_map);

  clock_deinit(ctx);

  return 0;
}
