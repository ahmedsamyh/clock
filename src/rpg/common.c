#include <rpg/common.h>

Vector2f pos_in_tile_space(Vector2f pos) {
  Vector2f res;
  res.x = (float)((int)(pos.x / TILE_SIZE) * (int)TILE_SIZE);
  res.y = (float)((int)(pos.y / TILE_SIZE) * (int)TILE_SIZE);

  return res;
}

bool Sprite_init_scaled(Sprite* spr, Texture* tex, int hframes, int vframes) {
  if (!Sprite_init(spr, tex, hframes, vframes)) return false;

  spr->scale.x = SCALE;
  spr->scale.y = SCALE;

  return true;
}
