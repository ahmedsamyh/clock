#include <rpg/stage.h>
#include <rpg/config.h>
#include <rpg/common.h>
#include <commonlib.h>

void Stage_init(Stage* stage, Context* ctx, const char* name){
  stage->ctx = ctx;
  stage->name = name;
  stage->cols = (size_t)(SCREEN_WIDTH / TILE_SIZE);
  stage->rows = (size_t)(SCREEN_HEIGHT / TILE_SIZE);
  stage->tiles = (Tile*)calloc(stage->cols*stage->rows, sizeof(Tile));

  Texture* tiles_tex = load_texture_err_handled(ctx, "resources/gfx/tiles.png");
  for (float y = 0.f; y < stage->rows; ++y) {
    for (float x = 0.f; x < stage->cols; ++x) {
      size_t idx = (size_t)y * stage->cols + (size_t)x;
      Tile_init(&stage->tiles[idx], (Vector2i) {0, 0}, ctx, tiles_tex);
      stage->tiles[idx].pos.x = x*TILE_SIZE;
      stage->tiles[idx].pos.y = y*TILE_SIZE;
      Tile_set_invalid(&stage->tiles[idx]);

      ASSERT(stage->tiles[idx].ctx);
    }
  }

  log_f(LOG_INFO, "tiles.len: %u", stage->rows*stage->cols);
}

void Stage_update(Stage* stage, Player* player) {
  bool will_check_collision = player != NULL;
  if (will_check_collision) player->hitting = false;
  // TODO: Maybe store an array of collidable tiles and loop over those instead
  for (int i = (int)(stage->cols*stage->rows-1); i >= 0; --i) {
    if (stage->tiles[i].collidable) {
      if (will_check_collision) {
	Rect tile_rect = (Rect) {
	  .pos = stage->tiles[i].pos,
	  .size = stage->tiles[i].size,
	};

	if (Rect_resolve_rect_collision(&player->hitbox, tile_rect)) {
	  Player_set_pos_to_hitbox_pos(player);
	  player->hitting = true;
	}
      }
    }
    Tile_update(&stage->tiles[i]);
  }
}

void Stage_draw(Stage* stage, bool debug) {
  for (int i = (int)(stage->cols*stage->rows-1); i >= 0; --i) {
    Tile_draw(&stage->tiles[i], debug);
  }
}

bool Stage_add_tile(Stage* stage, Vector2i type, bool collidable, Vector2f pos) {
  pos = v2f_divs(pos, TILE_SIZE);
  size_t idx = (size_t)(pos.y * stage->cols + pos.x);
  if (!(0 <= idx && idx < stage->cols*stage->rows)) return false;
  Tile* t = &stage->tiles[idx];
  t->collidable = collidable;
  if (!Tile_set_type(t, type)) {
    return false;
  }
  return true;
}

bool Stage_remove_tile(Stage* stage, Vector2f pos) {
  pos = v2f_divs(pos, TILE_SIZE);
  size_t idx = (size_t)(pos.y * stage->cols + pos.x);
  if (!(0 <= idx && idx < stage->cols*stage->rows)) return false;
  Tile* t = &stage->tiles[idx];
  if (!Tile_set_invalid(t)) {
    return false;
  }
  return true;
}

void Stage_deinit(Stage* stage) {
  free(stage->tiles);
}

// IO

bool Stage_save_to_file(Stage* stage) {
  // TODO: Check if the file already exists and choose to whether overwrite or not...
  const char* filename;
  temp_sprint(filename, STAGE_SAVE_PATH"/%s.lvl", stage->name);

  FILE* f = fopen(filename, "wb");

  if (f == NULL) {
    return false;
  }

  fprintf(f, "%zu|", stage->cols * stage->rows);

  for (size_t i = 0; i < stage->cols * stage->rows; ++i) {
    fprintf(f, "(%s)", Tile_serialize(&stage->tiles[i]));
  }

  temp_sprint(filename, STAGE_SAVE_PATH"/%s.lvl", stage->name);

  log_f(LOG_INFO, "stage '%s' saved to file '%s'!", stage->name, filename);

  fclose(f);
  return true;
}

bool Stage_load_from_file(Stage* stage) {
  const char* filename;
  temp_sprint(filename, STAGE_SAVE_PATH"/%s.lvl", stage->name);

  cstr file = slurp_file(filename);

  if (file == NULL) {
    return false;
  }

  String_view view = SV(file);

  String_view tiles_count_sv = sv_lpop_until_char(&view, '|');
  sv_lremove(&view, 1);

  int tiles_count = sv_to_int(tiles_count_sv);

  Tile* loading_tiles = NULL;
  if (tiles_count) {
    loading_tiles = (Tile*)calloc(tiles_count, sizeof(Tile));
  }

  Texture* tiles_tex = load_texture_err_handled(stage->ctx, "resources/gfx/tiles.png");
  for (size_t i = 0; i < tiles_count; ++i) {
    String_view tile_sv = sv_lpop_until_char(&view, ')');
    sv_lremove(&view, 1);
    sv_lremove(&tile_sv, 1);

    Tile t = {0};
    if (!Tile_init(&t, (Vector2i) {0, 0}, stage->ctx, tiles_tex)) return false;

    if (!Tile_deserialize(&t, tile_sv)) {
      if (loading_tiles) free(loading_tiles);
      return false;
    }

    ASSERT(loading_tiles != NULL);
    loading_tiles[i] = t;
  }
  ASSERT(view.count == 0);

  if (loading_tiles) {
    // todo: can we just assign loading_tiles to stage->tiles and not free it?

    memcpy(stage->tiles, loading_tiles, sizeof(Tile)*tiles_count);

    free(loading_tiles);
  }

  log_f(LOG_INFO, "stage '%s' loaded from file '%s'!", stage->name, filename);

  return true;
}

bool Stage_was_saved(Stage* stage) {
  const char* filename;
  temp_sprint(filename, STAGE_SAVE_PATH"/%s.lvl", stage->name);
  FILE* f = fopen(filename, "r");
  if (f == NULL && errno == ENOENT) {
    return false;
  }

  fclose(f);
  return true;
}
