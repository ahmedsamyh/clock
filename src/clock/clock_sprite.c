
#include <clock/clock_sprite.h>
#include <commonlib.h>
#include <assert.h>
#include <stdlib.h>

bool Sprite_init(Sprite* spr, Texture* texture, size_t hframes, size_t vframes) {
  assert(texture != NULL);
  spr->texture = texture;
  spr->hframes = hframes;
  spr->vframes = vframes;
  spr->pos = (Vector2f) {0.f, 0.f};
  spr->size.x = (float)spr->texture->size.x;
  spr->size.y = (float)spr->texture->size.y;
  spr->scale = (Vector2f) {1.f, 1.f};
  spr->tex_rect.size = (Vector2f) {spr->size.x / (float)spr->hframes, spr->size.y / (float)spr->vframes};
  Sprite_set_hframe(spr, 1);
  Sprite_set_vframe(spr, 0);
  spr->tint = COLOR_WHITE;

  spr->time_per_frame = SPRITE_DEFAULT_TIME_PER_FRAME;
  spr->accumulated_time = 0.f;
  return true;
}

void Sprite_update_tex_rect(Sprite *spr) {
  spr->tex_rect = (Rect) {
    .pos  = {spr->tex_rect.size.x * (float)spr->hframe, spr->tex_rect.size.y * (float)spr->vframe},
    .size = {spr->tex_rect.size.x, spr->tex_rect.size.y}
  };
}

void Sprite_set_hframe(Sprite* spr, size_t hframe) {
  if (hframe > spr->hframes-1) hframe = 0;
  spr->hframe = hframe;
  Sprite_update_tex_rect(spr);
}

void Sprite_set_vframe(Sprite* spr, size_t vframe) {
  if (vframe > spr->vframes-1) vframe = 0;
  spr->vframe = vframe;
  Sprite_update_tex_rect(spr);
}

void Sprite_center_origin(Sprite* spr) {
  spr->origin.x = spr->tex_rect.size.x / 2.f;
  spr->origin.y = spr->tex_rect.size.y / 2.f;
}


void Sprite_animate_hframe(Sprite* spr, float delta) {
  spr->accumulated_time += delta;
  if (spr->accumulated_time >= spr->time_per_frame) {
    spr->accumulated_time -= spr->time_per_frame;
    Sprite_set_hframe(spr, spr->hframe+1);
  }
}

void Sprite_deinit(Sprite* spr) {
  Texture_deinit(spr->texture);
}
