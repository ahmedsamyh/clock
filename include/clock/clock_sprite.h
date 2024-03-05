#ifndef _CLOCK_SPRITE_H_
#define _CLOCK_SPRITE_H_

#include <clock/clock_texture.h>
#include <clock/clock_rect.h>

typedef struct {
  Texture texture;
  Rect tex_rect;
  Vector2f pos;
  Vector2f size;
  Vector2f scale;
  float rotation; // in degrees
  size_t vframes, hframes;
  size_t vframe, hframe;
} Sprite;

bool Sprite_load(Sprite* spr, const char* filepath, size_t hframes, size_t vframes);
void Sprite_update_tex_rect(Sprite *spr);
void Sprite_set_hframe(Sprite* spr, size_t hframe);
void Sprite_set_vframe(Sprite* spr, size_t vframe);
void Sprite_deinit(Sprite* spr);

#endif /* _CLOCK_SPRITE_H_ */
