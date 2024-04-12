#include <rpg/tile.h>
#include <rpg/config.h>
#include <rpg/common.h>

void init_tiles_texture(Texture* t) {
  tile_cols = (int)t->size.x / (int)UNSCALED_TILE_SIZE;
  tile_rows = (int)t->size.y / (int)UNSCALED_TILE_SIZE;
}

bool Tile_init(Tile* t, Vector2i type, Context* ctx, Texture* tex, cstr stage) {
  ASSERT(tile_cols > 0);
  ASSERT(tile_rows > 0);

  t->type = type;
  Tile_set_pos(t, (Vector2f) {0.f, 0.f});
  t->size = (Vector2f) {TILE_SIZE, TILE_SIZE};
  t->ctx = ctx;
  t->warp_info.in_stage = stage;

  if (!Sprite_init_scaled(&t->spr, tex, tile_cols, tile_rows)) return false;

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
    log_error("Invalid tile type col '%d', col count: %d", type.x, tile_cols);
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

void Tile_set_pos(Tile* tile, Vector2f pos) {
  tile->pos = pos;
  tile->warp_info.in_pos = pos;
}
