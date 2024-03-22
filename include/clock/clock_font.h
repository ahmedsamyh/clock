#ifndef _CLOCK_FONT_H_
#define _CLOCK_FONT_H_

#include <stb/stb_truetype.h>
#include <clock/clock_rect.h>
#include <clock/clock_vector.h>
#include <clock/clock_texture.h>
#include <stdbool.h>

typedef struct Font Font;

typedef struct {
  Rect rect;
  Vector2f offset;
} Codepoint_metrics;

typedef struct {
  int key;
  Codepoint_metrics value;
} Codepoint_rect_KV;

struct Font {
  Codepoint_rect_KV* codepoint_rect_map;
  unsigned char* ttf_buffer;
  stbtt_fontinfo font;
};

bool Font_load(Font* font, const char* filepath);
bool Font_get_atlas_tex(Font* font, Texture* tex, int character_size);
Codepoint_metrics Font_get_codepoint_metrics(Font* font, int codepoint);
void Font_unload(Font* font);

unsigned char* onechan_to_fourchan(unsigned char* data, size_t data_size);

#endif /* _CLOCK_FONT_H_ */
