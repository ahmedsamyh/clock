#ifndef _TILE_H_
#define _TILE_H_

#include <clock/clock_vector.h>
#include <clock/clock_core.h>

typedef struct Tile Tile;

static size_t tiles_count = 1;

struct Tile {
  Vector2f pos;
  Vector2f size;
  Sprite   spr;
  int      type;
  Context* ctx;
};

bool Tile_init(Tile* tile, int type, Context* ctx, Texture* tex);
void Tile_draw(Tile* tile, bool debug);
void Tile_update(Tile* tile);

#endif /* _TILE_H_ */
