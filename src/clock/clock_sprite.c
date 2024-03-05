#include <clock/clock_sprite.h>
#include <commonlib.h>

bool Sprite_load(Sprite* spr, const char* filepath, size_t hframes, size_t vframes){
  if (!Texture_load_from_file(&spr->texture, filepath)) return false;
  spr->hframes = hframes;
  spr->vframes = vframes;
  spr->pos = (Vector2f){0.f, 0.f};
  spr->size.x = (float)spr->texture.size.x;
  spr->size.y = (float)spr->texture.size.y;
  spr->scale = (Vector2f){1.f, 1.f};
  spr->tex_rect.size = (Vector2f){spr->size.x / (float)spr->hframes, spr->size.y / (float)spr->vframes};
  Sprite_set_hframe(spr, 1);
  Sprite_set_vframe(spr, 0);

  log_f(LOG_INFO, "size: %f, %f", spr->size.x, spr->size.y);
  log_f(LOG_INFO, "frames: h: %u, v: %u", spr->hframes, spr->vframes);
  log_f(LOG_INFO, "tex_rect: %f, %f | %f, %f", spr->tex_rect.pos.x, spr->tex_rect.pos.y, spr->tex_rect.size.x, spr->tex_rect.size.y);

  return true;
}

void Sprite_set_hframe(Sprite* spr, size_t hframe){
  if (hframe > spr->hframes-1) hframe = 0;
  spr->hframe = hframe;
  Sprite_update_tex_rect(spr);
}

void Sprite_set_vframe(Sprite* spr, size_t vframe){
  if (vframe > spr->vframes-1) vframe = 0;
  spr->vframe = vframe;
  Sprite_update_tex_rect(spr);
}

void Sprite_update_tex_rect(Sprite *spr){
  spr->tex_rect = (Rect){
    .pos  = {spr->tex_rect.size.x * (float)spr->hframe, spr->tex_rect.size.y * (float)spr->vframe},
    .size = {spr->tex_rect.size.x, spr->tex_rect.size.y}
  };
}

void Sprite_deinit(Sprite* spr){
  Texture_deinit(&spr->texture);
}
