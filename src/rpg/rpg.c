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

bool change_state(Context* ctx, State* current_state, State next_state, cstr* current_state_text) {
  switch (next_state) {
  case STATE_PLAY: {
    *current_state_text = "Play";
  } break;
  case STATE_EDIT: {
    *current_state_text = "Edit";
  } break;
  default: assert(0 && "Unreachable");
  }

  *current_state = next_state;

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

  init_tiles_texture(tiles_tex);

  Font font = {0};
  if (!Font_init(&font, ctx, "resources/fonts/WayfarersToyBoxRegular-gxxER.ttf")) {
    return 1;
  }

  Player player = {0};

  if (!Player_init(&player, ctx, player_tex)) return 1;

  player.pos.x = width  * 0.5f;
  player.pos.y = height * 0.5f;

  // vsync
  clock_set_vsync(false);

  bool DEBUG_DRAW = false;

  Enemy* enemies = NULL; // dynamic array
  Stage_KV* stage_map = NULL;  // hashmap
  cstr current_stage_name = "stage 1";

  // We can assert because we are SURE that there is no other stages in the stage_map
  assert(allocate_new_stage(ctx, current_stage_name, &stage_map));

  cstr current_state_text = "";

  State current_state;
  if (!change_state(ctx, &current_state, STATE_PLAY, &current_state_text)) return 1;

  // Edit
  Vector2f ui_pos = {width/2.f, 0.f};
  cstr collidable_text = "Collidable: on";
  bool WARP_MODE = false;
  Rect edit_cursor = {
    .pos = (Vector2f) {0.f, 0.f},
    .size = (Vector2f) {TILE_SIZE, TILE_SIZE}
  };
  Vector2i tile_type          = {0};
  Vector2i hovering_tile_type = {0};
  bool tile_collidable = true;
  Warp_info tile_warp_info = {0};
  Sprite tiles_spr = {0};
  if (!Sprite_init_scaled(&tiles_spr, tiles_tex, 1, 1)) return 1;

  Rect tiles_rect = {
    .pos  = tiles_spr.pos,
    .size = v2f_mul(tiles_spr.size, tiles_spr.scale),
  };

  UI ui = UI_make(ctx, &font);

  while (!clock_should_quit(ctx)) {

    clock_begin_draw(ctx);

    clock_clear(ctx, COLOR_BLACK);

    //
    // Draw
    //

    Stage_KV* current_stage_kv = shgetp_null(stage_map, current_stage_name);
    assert(current_stage_kv != NULL);
    Stage* current_stage = &current_stage_kv->value;

    switch (current_state) {
    case STATE_PLAY: {
      clock_clear(ctx, color_from_hex(0xFF555555));
      Stage_draw(current_stage, DEBUG_DRAW);

      for (int i = arrlen(enemies) - 1; i >= 0; --i) {
	Enemy_draw(&enemies[i], DEBUG_DRAW);
      }

      Player_draw(&player, DEBUG_DRAW);
    } break;
    case STATE_EDIT: {
      clock_clear(ctx, color_from_hex(0xFF5555AA));
      Stage_draw(current_stage, DEBUG_DRAW);

      draw_rect(ctx, edit_cursor, color_alpha(COLOR_GREEN, 0.5f));

      if (ctx->k[KEY_LEFT_SHIFT].held) {
	bool prev_state = ctx->use_camera_view;
	clock_use_camera_view(ctx, false);
	draw_rect(ctx, (Rect){(Vector2f){0.f, 0.f}, screen_size}, color_alpha(COLOR_BLACK, 0.8f));
	Sprite tiles_spr_copy = tiles_spr;
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
	draw_rect(ctx, r, color_alpha(COLOR_GREEN, 0.4f));

	r.pos = v2f_muls((Vector2f){hovering_tile_type.x, hovering_tile_type.y}, TILE_SIZE);
	/* r.pos = clock_screen_to_world(ctx, r.pos); */
	draw_rect(ctx, r, color_alpha(COLOR_RED, 0.4f));
	clock_use_camera_view(ctx, prev_state);
      }

      if (!tile_collidable) {
	collidable_text = "Collidable: off";
      } else {
	collidable_text = "Collidable: on";
      }

      Player_draw(&player, DEBUG_DRAW);

    } break;
    default: assert(0 && "Unreachable");
    }

    //
    // UI
    //
    UI_begin(&ui, &ui_pos, UI_LAYOUT_KIND_VERT);
    cstr full_state_text;
    temp_sprint(full_state_text, "State: %s", current_state_text);
    UI_text(&ui, full_state_text, 24, COLOR_WHITE);

    switch (current_state) {
    case STATE_PLAY: {
    } break;
    case STATE_EDIT: {
      cstr full_stage_name;
      temp_sprint(full_stage_name, "Stage: %s", current_stage_name);
      UI_text(&ui, full_stage_name, 24, COLOR_WHITE);

      UI_text(&ui, collidable_text, 24, COLOR_WHITE);
      cstr warp_mode_text;
      temp_sprint(warp_mode_text, "Warp mode: %s", (WARP_MODE ? "On" : "Off"));
      UI_text(&ui, warp_mode_text, 24, COLOR_WHITE);
    } break;
    default: assert(0 && "Unreachable");
    }

    UI_end(&ui);

    //
    // Update
    //
    if (ctx->k[KEY_GRAVE_ACCENT].pressed) DEBUG_DRAW = !DEBUG_DRAW;
    if (ctx->k[KEY_TAB].pressed) {
      if (!change_state(ctx, &current_state, (current_state + 1) % STATE_COUNT, &current_state_text)) return 1;
    }

    switch (current_state) {
    case STATE_PLAY: {
      Player_update(&player);

      for (int i = arrlen(enemies) - 1; i >= 0; --i) {
	Enemy_update(&enemies[i]);
      }

      Stage_update(current_stage, &player);

    } break;
    case STATE_EDIT: {
      Stage_update(current_stage, NULL);
      edit_cursor.pos = pos_in_tile_space(ctx->mpos);

      if (WARP_MODE) {
	if (clock_mouse_pressed(ctx, MOUSE_BUTTON_LEFT)) {
	  if (!tile_warp_info.in_stage) {
	    tile_warp_info.in_stage = current_stage_name;
	    tile_warp_info.in_pos = edit_cursor.pos;
	  } else {
	    tile_warp_info.out_stage = current_stage_name;
	    tile_warp_info.out_pos = edit_cursor.pos;
	    WARP_MODE = false;
	  }
	}
      } else {
	// Increase cursor size
	if (ctx->k[KEY_Z].pressed) {
	  if (ctx->k[KEY_LEFT_ALT].held) {
	    edit_cursor.size.x -= TILE_SIZE;
	    if (edit_cursor.size.x < TILE_SIZE) edit_cursor.size.x = TILE_SIZE;
	  } else {
	    edit_cursor.size.x += TILE_SIZE;
	  }
	}

	if (ctx->k[KEY_X].pressed) {
	  if (ctx->k[KEY_LEFT_ALT].held) {
	    edit_cursor.size.y -= TILE_SIZE;
	    if (edit_cursor.size.y < TILE_SIZE) edit_cursor.size.y = TILE_SIZE;
	  } else {
	    edit_cursor.size.y += TILE_SIZE;
	  }
	}

	// Choose tile type
	if (ctx->k[KEY_LEFT_SHIFT].held) {
	  if (Rect_contains_point(tiles_rect, ctx->mpos)) {
	    hovering_tile_type.x = (int)ctx->mpos.x / (int)TILE_SIZE;
	    hovering_tile_type.y = (int)ctx->mpos.y / (int)TILE_SIZE;

	    if (clock_mouse_pressed(ctx, MOUSE_BUTTON_LEFT)) {
	      tile_type = hovering_tile_type;
	    }
	  }
	} else {
	  if (clock_mouse_held(ctx, MOUSE_BUTTON_LEFT)) {
	    for (float x = edit_cursor.pos.x; x < edit_cursor.pos.x + edit_cursor.size.x; x += TILE_SIZE) {
	      for (float y = edit_cursor.pos.y; y < edit_cursor.pos.y + edit_cursor.size.y; y += TILE_SIZE) {
		Vector2f p = {x, y};
		Stage_add_tile(current_stage, tile_type, tile_collidable, p);
	      }
	    }
	  }

	  // TODO: Have some sort of action history for undo-ing
	  if (clock_mouse_held(ctx, MOUSE_BUTTON_RIGHT)) {
	    for (float x = edit_cursor.pos.x; x < edit_cursor.pos.x + edit_cursor.size.x; x += TILE_SIZE) {
	      for (float y = edit_cursor.pos.y; y < edit_cursor.pos.y + edit_cursor.size.y; y += TILE_SIZE) {
		Vector2f p = {x, y};
		Stage_remove_tile(current_stage, p);
	      }
	    }
	  }
	}

	if (ctx->k[KEY_C].pressed) {
	  tile_collidable = !tile_collidable;
	}
      }

      if (ctx->k[KEY_W].pressed) {
	if (!tile_warp_info.active) {
	  WARP_MODE = true;
	} else {
	  WARP_MODE = false;
	}
	tile_warp_info.active = !tile_warp_info.active;
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

    clock_end_draw(ctx);
  }

  arrfree(enemies);
  shfree(stage_map);

  Font_deinit(&font);
  clock_deinit(ctx);

  return 0;
}
