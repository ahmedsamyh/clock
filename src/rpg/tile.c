#include <rpg/tile.h>
#include <rpg/config.h>
#include <rpg/common.h>
#include <assert.h>

void init_tiles_texture(Texture* t) {
  tile_cols = (int)t->size.x / (int)UNSCALED_TILE_SIZE;
  tile_rows = (int)t->size.y / (int)UNSCALED_TILE_SIZE;

  /* log_f(LOG_INFO, "tile_rows: %d", tile_rows); */
  /* log_f(LOG_INFO, "tile_cols: %d", tile_cols); */
}

cstr Warp_info_serialize(Warp_info* wi) {
  cstr res;

  if (!wi->active) {
    res = "";
  } else {
    temp_sprint(res, "%d|%s|%s|%f,%f|%f,%f", wi->active,
		wi->in_stage, wi->out_stage,
		wi->in_pos.x, wi->in_pos.y, wi->out_pos.x, wi->out_pos.y);
  }

  return res;
}

bool Tile_init(Tile* t, Vector2i type, Context* ctx, Texture* tex) {
  assert(tile_cols > 0);
  assert(tile_rows > 0);

  t->type = type;
  t->pos = (Vector2f){0.f, 0.f};
  t->size = (Vector2f){TILE_SIZE, TILE_SIZE};
  t->ctx = ctx;

  if (!Sprite_init_scaled(&t->spr, tex, tile_cols, tile_rows)) return false;

  /* t->spr.scale.x = SCALE; */
  /* t->spr.scale.y = SCALE; */

  Tile_set_type(t, type);

  return true;
}

bool Tile_set_invalid(Tile* tile) {
  tile->collidable = false;
  tile->warp_info.active = false;
  if (!Tile_set_type(tile, (Vector2i) {0, 0})) return false;
  return true;
}

bool Tile_set_type(Tile* t, Vector2i type) {
  if (type.x < 0 || type.x >= tile_cols) {
    log_f(LOG_INFO, "Invalid tile type col '%d', col count: %d", type.x, tile_cols);
    return false;
  }

  if (type.y < 0 || type.y >= tile_rows) {
    log_f(LOG_INFO, "Invalid tile type row '%d', row count: %d", type.y, tile_rows);
    return false;
  }

  t->type = type;

  Sprite_set_hframe(&t->spr, type.x);
  Sprite_set_vframe(&t->spr, type.y);

  return true;
}

void Tile_update(Tile* t) {
  // update sprite pos
  t->spr.pos = t->pos;
}

void Tile_draw(Tile* t, bool debug) {

  draw_sprite(t->ctx, &t->spr);

  if (debug) {
    // draw debug
    Color color = color_alpha(COLOR_BLUE, (t->collidable ? 0.45f : 0.1f));
    draw_rect(t->ctx, (Rect){t->pos, t->size}, color);
    if (t->warp_info.active) {
      color = color_alpha(COLOR_MAGENTA, 0.45f);
      draw_rect(t->ctx, (Rect){t->pos, t->size}, color);
    }
  }
}

cstr Tile_serialize(Tile* tile) {
  cstr res;

  temp_sprint(res, "%.2f,%.2f|%.2f,%.2f|%d,%d|%d|%s", tile->pos.x, tile->pos.y,
	      tile->size.x, tile->size.y, tile->type.x, tile->type.y, tile->collidable, Warp_info_serialize(&tile->warp_info));

  return res;
}

bool Tile_deserialize(Tile* t, String_view tile_sv) {
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

  String_view type_x_sv = sv_lpop_until_char(&tile_sv, ',');
  sv_lremove(&tile_sv, 1);
  String_view type_y_sv = sv_lpop_until_char(&tile_sv, '|');
  sv_lremove(&tile_sv, 1);

  Vector2i type = {0};
  type.x = sv_to_int(type_x_sv);
  type.y = sv_to_int(type_y_sv);

  if (!Tile_set_type(t, type)) {
    return false;
  }

  t->pos = pos;
  t->size = size;

  String_view collidable_sv = sv_lpop_until_char(&tile_sv, '|');
  sv_lremove(&tile_sv, 1);
  t->collidable = (bool)sv_to_int(collidable_sv);

  return true;
}
