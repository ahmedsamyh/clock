#ifndef _TILE_H_
#define _TILE_H_

#include <clock/clock_vector.h>
#include <clock/clock_core.h>

typedef struct Tile Tile;
typedef struct Warp_info Warp_info;

int tile_rows;
int tile_cols;

#define WARP_INFO_TEXTURE_TYPE (Vector2f) {0.f, 3.f}

void init_tiles_texture(Texture* texture);

struct Warp_info {
  bool     active;
  cstr     in_stage;
  cstr     out_stage;
  Vector2f in_pos;
  Vector2f out_pos;
};

struct Tile {
  Vector2f pos;
  Vector2f size;
  Sprite   spr;
  // type is just a vector offset to the texture map of tiles
  Vector2i type;
  bool     collidable;
  Context* ctx;
  Warp_info warp_info;
};

bool Tile_init(Tile* tile, Vector2i type, Context* ctx, Texture* tex, cstr stage);
bool Tile_set_invalid(Tile* tile);
bool Tile_set_type(Tile* tile, Vector2i type);
void Tile_draw(Tile* tile, bool debug);
void Tile_update(Tile* tile);
void Tile_set_pos(Tile* tile, Vector2f pos);

#endif /* _TILE_H_ */
