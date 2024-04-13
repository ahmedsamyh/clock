#ifndef _CLOCK_FONT_H_
#define _CLOCK_FONT_H_

#include <stb/stb_truetype.h>
#include <clock/clock_rect.h>
#include <clock/clock_vector.h>
#include <clock/clock_texture.h>
#include <stdbool.h>
#include <commonlib.h>

typedef struct Font Font;
typedef struct Context Context;

typedef struct {
  Vector2f offset;
  Rect rect;       // rect.pos is the offset in the texture to get to the top left of the glyph
                   // rect.size is the bounding box of the glyph
} Codepoint_rect;

typedef struct {
  int key;              // codepoint
  Codepoint_rect value; // rect of the codepoint
} Codepoint_rect_KV;

#define DEFAULT_FONT_CHARACTER_SIZE 18

struct Font {
  Codepoint_rect_KV* codepoint_rect_map;
  int current_character_size;
  uint8* ttf_buffer;
  bool should_free_ttf_buffer;
  stbtt_fontinfo font;
  int codepoint_start, codepoint_end;
  Texture* texture;
  Context* ctx;
};

bool Font_init(Font* font, Context* ctx, const char* filepath);
bool Font_init_from_file(Font* font, Context* ctx, const char* filepath);
bool Font_init_from_memory(Font* font, Context* ctx, uint8* data, cstr name);
bool Font_generate_atlas_tex(Font* font, int character_size);
bool Font_get_codepoint_rect(Font* font, int codepoint, Rect* rect);
void Font_deinit(Font* font);

uint8* onechan_to_fourchan(uint8* data, size_t data_size);

#endif /* _CLOCK_FONT_H_ */
