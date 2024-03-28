#include <rpg/stage.h>
#include <rpg/config.h>
#include <rpg/common.h>
#include <commonlib.h>
#include <assert.h>

void Stage_init(Stage* stage, Context* ctx, const char* name){
  stage->ctx = ctx;
  stage->name = name;
  stage->cols = SCREEN_WIDTH / TILE_SIZE;
  stage->rows = SCREEN_HEIGHT / TILE_SIZE;
  stage->tiles = (Tile*)calloc(stage->cols*stage->rows, sizeof(Tile));
}

void Stage_update(Stage* stage, Player* player) {
  bool will_check_collision = player != NULL;
  if (will_check_collision) player->hitting = false;
  // TODO: Maybe store an array of collidable tiles and loop over those instead
  for (int i = stage->cols*stage->rows-1; i >= 0; --i) {
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
  for (int i = arrlen(stage->tiles) - 1; i >= 0; --i) {
    Tile_draw(&stage->tiles[i], debug);
  }
}

// TODO: Checking collision of tiles is very slow!
// Maybe use the index as the position?
bool Stage_add_tile(Stage* stage, Vector2i type, bool collidable, Vector2f pos) {
  Context* ctx = stage->ctx;

  pos = v2f_div(pos, (Vector2f) {stage->cols, stage->rows});

  size_t idx = pos.y * stage->cols + pos.x;

  Texture* tiles_tex = load_texture_err_handled(ctx, "resources/gfx/tiles.png");

  Tile* t = &stage->tiles[idx];

  if (!Tile_init(t, type, ctx, tiles_tex)) return false;
  t->pos = pos_in_tile_space(pos);
  t->collidable = collidable;

  return true;
}

void Stage_remove_tile(Stage* stage, Vector2f pos) {
  Context* ctx = stage->ctx;
  bool contains = false;
  size_t containing_idx = 0;
  for (size_t i = 0; i < arrlenu(stage->tiles); ++i) {
    Rect tile_rect = (Rect) {
      .pos = stage->tiles[i].pos,
      .size = stage->tiles[i].size,
    };
    if (Rect_contains_point(tile_rect, pos)) {
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
  free(stage->tiles);
}

// IO

bool Stage_save_to_file(Stage* stage) {
  // TODO: Check if the file already exists and choose to whether overwrite or not...
  /* const char* filename; */
  /* temp_sprint(filename, STAGE_SAVE_PATH"/%s.lvl", stage->name); */

  /* FILE* f = fopen(filename, "wb"); */

  /* if (f == NULL) { */
  /*   return false; */
  /* } */

  /* fprintf(f, "%d|", (int)arrlen(stage->tiles)); */

  /* for (size_t i = 0; i < arrlenu(stage->tiles); ++i) { */
  /*   fprintf(f, "(%s)", Tile_serialize(&stage->tiles[i])); */
  /* } */

  /* temp_sprint(filename, STAGE_SAVE_PATH"/%s.lvl", stage->name); */

  /* log_f(LOG_INFO, "Stage '%s' saved to file '%s'!", stage->name, filename); */

  /* fclose(f); */
  return true;
}

bool Stage_load_from_file(Stage* stage) {
  /* const char* filename; */
  /* temp_sprint(filename, stage_save_path"/%s.lvl", stage->name); */

  /* const char* file = slurp_file(filename); */

  /* if (file == null) { */
  /*   return false; */
  /* } */

  /* string_view view = sv(file); */

  /* string_view tiles_count_sv = sv_lpop_until_char(&view, '|'); */
  /* sv_lremove(&view, 1); */

  /* int tiles_count = sv_to_int(tiles_count_sv); */

  /* tile* loading_tiles = null; */
  /* if (tiles_count) { */
  /*   loading_tiles = (tile*)calloc(tiles_count, sizeof(tile)); */
  /* } */

  /* for (size_t i = 0; i < tiles_count; ++i) { */
  /*   string_view tile_sv = sv_lpop_until_char(&view, ')'); */
  /*   sv_lremove(&view, 1); */
  /*   sv_lremove(&tile_sv, 1); */

  /*   tile t = {0}; */

  /*   string_view pos_x_sv = sv_lpop_until_char(&tile_sv, ','); */
  /*   sv_lremove(&tile_sv, 1); */
  /*   string_view pos_y_sv = sv_lpop_until_char(&tile_sv, '|'); */
  /*   sv_lremove(&tile_sv, 1); */

  /*   vector2f pos = {0}; */
  /*   pos.x = sv_to_float(pos_x_sv); */
  /*   pos.y = sv_to_float(pos_y_sv); */

  /*   string_view size_x_sv = sv_lpop_until_char(&tile_sv, ','); */
  /*   sv_lremove(&tile_sv, 1); */
  /*   string_view size_y_sv = sv_lpop_until_char(&tile_sv, '|'); */
  /*   sv_lremove(&tile_sv, 1); */

  /*   vector2f size = {0}; */
  /*   size.x = sv_to_float(size_x_sv); */
  /*   size.y = sv_to_float(size_y_sv); */

  /*   string_view type_x_sv = sv_lpop_until_char(&tile_sv, ','); */
  /*   sv_lremove(&tile_sv, 1); */
  /*   string_view type_y_sv = sv_lpop_until_char(&tile_sv, '|'); */
  /*   sv_lremove(&tile_sv, 1); */

  /*   vector2i type = {0}; */
  /*   type.x = sv_to_int(type_x_sv); */
  /*   type.y = sv_to_int(type_y_sv); */

  /*   texture* tiles_tex = load_texture_err_handled(stage->ctx, "resources/gfx/tiles.png"); */
  /*   if (!tile_init(&t, type, stage->ctx, tiles_tex)) { */
  /*     if (loading_tiles) free(loading_tiles); */
  /*     return false; */
  /*   } */

  /*   t.pos = pos; */
  /*   t.size = size; */

  /*   string_view collidable_sv = sv_lpop_until_char(&tile_sv, '|'); */
  /*   sv_lremove(&tile_sv, 1); */
  /*   t.collidable = (bool)sv_to_int(collidable_sv); */

  /*   assert(loading_tiles != null); */
  /*   loading_tiles[i] = t; */
  /* } */

  /* assert(view.count == 0); */

  /* if (stage->tiles) */
  /*   arrfree(stage->tiles); */

  /* if (loading_tiles) { */
  /*   // todo: can we just assign loading_tiles to stage->tiles and not free it? */

  /*   arrsetlen(stage->tiles, tiles_count); */

  /*   memcpy(stage->tiles, loading_tiles, sizeof(tile)*tiles_count); */

  /*   free(loading_tiles); */
  /* } */

  /* log_f(log_info, "stage '%s' loaded from file '%s'!", stage->name, filename); */

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
