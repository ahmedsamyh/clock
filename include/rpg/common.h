#ifndef _COMMON_H_
#define _COMMON_H_

#include <clock/clock_vector.h>
#include <clock/clock_sprite.h>
#include <rpg/config.h>
#include <stdbool.h>

Vector2f pos_in_tile_space(Vector2f pos);
bool Sprite_init_scaled(Sprite* spr, Texture* tex, int hframes, int vframes);

#endif /* _COMMON_H_ */
