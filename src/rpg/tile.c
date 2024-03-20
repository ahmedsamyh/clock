#include <rpg/tile.h>
#include <rpg/config.h>
#include <assert.h>

bool Tile_init(Tile* t, int type, Context* ctx, Texture* tex) {
  if (type < 0|| type > tiles_count-1) {
    log_f(LOG_INFO, "Invalid tile type '%d'", type);
    return false;
  }
  t->type = type;
  t->pos = (Vector2f){0.f, 0.f};
  t->size = (Vector2f){TILE_SIZE, TILE_SIZE};
  t->ctx = ctx;

  if (!Sprite_init(&t->spr, tex, tiles_count, 1)) return false;

  t->spr.scale.x = SCALE;
  t->spr.scale.y = SCALE;

  return true;
}

void Tile_update(Tile* t) {
  // update sprite pos
  t->spr.pos = t->pos;
}

void Tile_draw(Tile* t, bool debug) {
  assert(t->ctx);

  draw_sprite(t->ctx, &t->spr);

  if (debug) {
    // draw debug
    draw_rect(t->ctx, (Rect){t->pos, t->size}, color_alpha(COLOR_BLUE, 0.45f));
  }
}
