#ifndef _COMMON_H_
#define _COMMON_H_

#include <clock/clock_vector.h>
#include <clock/clock_sprite.h>
#include <clock/clock_core.h>
#include <rpg/config.h>
#include <stdbool.h>

Vector2f pos_in_tile_space(Vector2f pos);
bool Sprite_init_scaled(Sprite* spr, Texture* tex, int hframes, int vframes);
Texture* load_texture_err_handled(Context* ctx, const char* filepath);
void draw_spr_text(Context* ctx, Sprite* spr_font, cstr text, Vector2f pos, Color color, cstr font_map);
void draw_text_outlined(Context* ctx, Font* font, cstr text, Vector2f pos, int char_size, Color color, Color out_color);

#endif /* _COMMON_H_ */
