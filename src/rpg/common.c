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

Texture* load_texture_err_handled(Context* ctx, const char* filepath) {
  Texture* tex = Resman_load_texture_from_file(ctx->resman, filepath);
  if (tex == NULL) exit(1);
  return tex;
}
