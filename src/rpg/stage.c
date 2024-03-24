#include <rpg/stage.h>
#include <rpg/config.h>
#include <rpg/common.h>
#include <commonlib.h>
#include <assert.h>

void Stage_init(Stage* stage, Context* ctx, const char* name){
  stage->ctx = ctx;
  stage->name = name;
}

void Stage_update(Stage* stage, Player* player) {
  bool will_check_collision = player != NULL;
  if (will_check_collision) player->hitting = false;
  for (int i = arrlen(stage->tiles) - 1; i >= 0; --i) {
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

    Tile_update(&stage->tiles[i]);
  }
}

void Stage_draw(Stage* stage, bool debug) {
  for (int i = arrlen(stage->tiles) - 1; i >= 0; --i) {
    Tile_draw(&stage->tiles[i], debug);
  }
}

bool Stage_add_tile(Stage* stage, Vector2i type) {
  Context* ctx = stage->ctx;
  bool contains = false;
  for (size_t i = 0; i < arrlenu(stage->tiles); ++i) {
    Rect tile_rect = (Rect) {
      .pos = stage->tiles[i].pos,
      .size = stage->tiles[i].size,
    };
    if (Rect_contains_point(tile_rect, ctx->mpos)) {
      contains = true;
      break;
    }
  }
  Texture* tiles_tex = load_texture_err_handled(ctx, "resources/gfx/tiles.png");
  if (!contains) {
    Tile t = {0};
    if (!Tile_init(&t, type, ctx, tiles_tex)) return false;
    t.pos = pos_in_tile_space(ctx->mpos);
    arrput(stage->tiles, t);
    log_f(LOG_INFO, "Tile added!");
  }

  return true;
}

void Stage_remove_tile(Stage* stage) {
  Context* ctx = stage->ctx;
  bool contains = false;
  size_t containing_idx = 0;
  for (size_t i = 0; i < arrlenu(stage->tiles); ++i) {
    Rect tile_rect = (Rect) {
      .pos = stage->tiles[i].pos,
      .size = stage->tiles[i].size,
    };
    if (Rect_contains_point(tile_rect, ctx->mpos)) {
      contains = true;
      containing_idx = i;
      break;
    }
  }
  if (contains) {
    arrdel(stage->tiles, containing_idx);
  }
}

void Stage_deinit(Stage* stage) {
  if (stage->tiles)
    arrfree(stage->tiles);
}

// IO

bool Stage_save_to_file(Stage* stage) {
  // TODO: Check if the file already exists and choose to whether overwrite or not...
  const char* filename;
  temp_sprint(filename, "resources/stages/%s.lvl", stage->name);

  FILE* f = fopen(filename, "wb");

  if (f == NULL) {
    return false;
  }

  fprintf(f, "%d|", (int)arrlen(stage->tiles));

  for (size_t i = 0; i < arrlenu(stage->tiles); ++i) {
    fprintf(f, "(%s)", Tile_serialize(&stage->tiles[i]));
  }

  temp_sprint(filename, "resources/stages/%s.lvl", stage->name);

  log_f(LOG_INFO, "Stage '%s' saved to file '%s'!", stage->name, filename);

  fclose(f);
  return true;
}

bool Stage_load_from_file(Stage* stage) {
  const char* filename;
  temp_sprint(filename, "resources/stages/%s.lvl", stage->name);

  const char* file = slurp_file(filename);

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

  for (size_t i = 0; i < tiles_count; ++i) {
    String_view tile_sv = sv_lpop_until_char(&view, ')');
    sv_lremove(&view, 1);
    sv_lremove(&tile_sv, 1);

    Tile t = {0};

    String_view pos_x_sv = sv_lpop_until_char(&tile_sv, ',');
    sv_lremove(&tile_sv, 1);
    String_view pos_y_sv = sv_lpop_until_char(&tile_sv, '|');
    sv_lremove(&tile_sv, 1);

    Vector2f pos = {0};
    pos.x = sv_to_float(pos_x_sv);
    pos.y = sv_to_float(pos_y_sv);

    String_view size_x_sv = sv_lpop_until_char(&tile_sv, ',');
    sv_lremove(&tile_sv, 1);
    String_view size_y_sv = sv_lpop_until_char(&tile_sv, '|');
    sv_lremove(&tile_sv, 1);

    Vector2f size = {0};
    size.x = sv_to_float(size_x_sv);
    size.y = sv_to_float(size_y_sv);

    // TODO: Actually we don't need this
    String_view texture_ptr_sv = sv_lpop_until_char(&tile_sv, '|');
    sv_lremove(&tile_sv, 1);
    Texture* texture_ptr = (Texture*)sv_to_ptr(texture_ptr_sv);

    String_view type_x_sv = sv_lpop_until_char(&tile_sv, ',');
    sv_lremove(&tile_sv, 1);
    String_view type_y_sv = sv_lpop_until_char(&tile_sv, '|');
    sv_lremove(&tile_sv, 1);

    Vector2i type = {0};
    type.x = sv_to_int(type_x_sv);
    type.y = sv_to_int(type_y_sv);

    Texture* tiles_tex = load_texture_err_handled(stage->ctx, "resources/gfx/tiles.png");
    if (!Tile_init(&t, type, stage->ctx, tiles_tex)) {
      if (loading_tiles) free(loading_tiles);
      return false;
    }

    t.pos = pos;
    t.size = size;

    assert(loading_tiles != NULL);
    loading_tiles[i] = t;
  }

  assert(view.count == 0);

  if (stage->tiles)
    arrfree(stage->tiles);

  if (loading_tiles) {
    // TODO: Can we just assign loading_tiles to stage->tiles and not free it?

    arrsetlen(stage->tiles, tiles_count);

    memcpy(stage->tiles, loading_tiles, sizeof(Tile)*tiles_count);

    free(loading_tiles);
  }

  log_f(LOG_INFO, "Stage '%s' loaded from file '%s'!", stage->name, filename);

  return true;
}
