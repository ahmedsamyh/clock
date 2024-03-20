#include <rpg/common.h>

Vector2f pos_in_tile_space(Vector2f pos) {
  Vector2f res;
  res.x = (float)((int)(pos.x / TILE_SIZE) * (int)TILE_SIZE);
  res.y = (float)((int)(pos.y / TILE_SIZE) * (int)TILE_SIZE);

  return res;
}
