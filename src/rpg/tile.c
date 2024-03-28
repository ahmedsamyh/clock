#include <rpg/tile.h>
#include <rpg/config.h>
#include <rpg/common.h>
#include <assert.h>

void init_tiles_texture(Texture* t) {
  tile_cols = (int)t->size.x / (int)UNSCALED_TILE_SIZE;
  tile_rows = (int)t->size.y / (int)UNSCALED_TILE_SIZE;

  log_f(LOG_INFO, "tile_rows: %d", tile_rows);
  log_f(LOG_INFO, "tile_cols: %d", tile_cols);
}

cstr Warp_info_serialize(Warp_info* wi) {
  cstr res;

  if (!wi->active) {
    res = "";
  } else {
    temp_sprint(res, "%d|%s|%f,%f|%f,%f", wi->active, wi->stage_name,
		wi->in_pos.x, wi->in_pos.y, wi->out_pos.x, wi->out_pos.y);
  }

  return res;
}

bool Tile_init(Tile* t, Vector2i type, Context* ctx, Texture* tex) {
  assert(tile_cols > 0);
  assert(tile_rows > 0);

  if (type.x < 0 || type.x >= tile_cols) {
    log_f(LOG_INFO, "Invalid tile type col '%d', col count: %d", type.x, tile_cols);
    return false;
  }

  if (type.y < 0 || type.y >= tile_rows) {
    log_f(LOG_INFO, "Invalid tile type row '%d', row count: %d", type.y, tile_rows);
    return false;
  }

  t->type = type;
  t->pos = (Vector2f){0.f, 0.f};
  t->size = (Vector2f){TILE_SIZE, TILE_SIZE};
  t->ctx = ctx;

  if (!Sprite_init(&t->spr, tex, tile_cols, tile_rows)) return false;

  Sprite_set_hframe(&t->spr, type.x);
  Sprite_set_vframe(&t->spr, type.y);

  t->spr.scale.x = SCALE;
  t->spr.scale.y = SCALE;

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

const char* Tile_serialize(Tile* tile) {
  const char* res;

  temp_sprint(res, "%.2f,%.2f|%.2f,%.2f|%d,%d|%d|%s", tile->pos.x, tile->pos.y,
	      tile->size.x, tile->size.y, tile->type.x, tile->type.y, tile->collidable, Warp_info_serialize(&tile->warp_info));

  return res;
}
