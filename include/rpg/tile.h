#ifndef _TILE_H_
#define _TILE_H_

#include <clock/clock_vector.h>
#include <clock/clock_core.h>

typedef struct Tile Tile;

int tile_rows;
int tile_cols;

void init_tiles_texture(Texture* texture);

struct Tile {
  Vector2f pos;
  Vector2f size;
  Sprite   spr;
  Vector2i type;
  Context* ctx;
};

bool Tile_init(Tile* tile, Vector2i type, Context* ctx, Texture* tex);
void Tile_draw(Tile* tile, bool debug);
void Tile_update(Tile* tile);

#endif /* _TILE_H_ */
