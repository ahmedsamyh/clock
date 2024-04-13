#include <clock/clock_font.h>
#include <stdlib.h>
#include <commonlib.h>
#include <clock/clock_core.h>
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb/stb_truetype.h>
#include <stb/stb_ds.h>


bool Font_init_from_file(Font* font, Context* ctx, const char* filepath) {
  uint8* ttf_buffer = (uint8*)slurp_file(filepath);

  if (ttf_buffer == NULL) {
    log_f(LOG_ERROR, "Could not load font file '%s'!", filepath);
    return false;
  }

  font->should_free_ttf_buffer = true;

  return Font_init_from_memory(font, ctx, ttf_buffer, filepath);
}

bool Font_init(Font* font, Context* ctx, const char* filepath) {
  return Font_init_from_file(font, ctx, filepath);
}

bool Font_init_from_memory(Font* font, Context* ctx, uint8* data, cstr name) {
  uint8* ttf_buffer = data;

  stbtt_InitFont(&font->font, (const unsigned char*)ttf_buffer, stbtt_GetFontOffsetForIndex((const uint8*)ttf_buffer,0));

  font->ttf_buffer = ttf_buffer;

  // TODO: Format the texture_name like '[ATLAS] filepath' when i have temp_sprintf in commonlib.h
  font->texture = Resman_load_texture_uninitialized(ctx->resman, name);

  Font_generate_atlas_tex(font, DEFAULT_FONT_CHARACTER_SIZE);

  return true;
}

bool Font_get_codepoint_rect(Font* font, int codepoint, Rect* rect) {
  Codepoint_rect_KV* kv = hmgetp_null(font->codepoint_rect_map, codepoint);

  if (kv == NULL) {
    log_f(LOG_ERROR, "Could not find the rect for codepoint '%d'(%c)", codepoint, codepoint);
    return false;
  }

  *rect = kv->value.rect;

  return true;
}

bool Font_generate_atlas_tex(Font* font, int character_size) {

  font->current_character_size = character_size;

  Vector2f tex_size = {0.f, (float32)character_size};

  int codepoint_min =  INT_MAX;
  int codepoint_max = -INT_MAX;

  Vector2i range = {0, 127};
  for (int i = range.x; i <= range.y; ++i) {
    if (stbtt_FindGlyphIndex(&font->font, i) != 0) {
      if (i < codepoint_min) codepoint_min = i;
      if (i > codepoint_max) codepoint_max = i;

      int w, h, xoff, yoff;
      uint8* single_channel_bitmap = stbtt_GetCodepointBitmap(&font->font, 0, stbtt_ScaleForPixelHeight(&font->font, (float32)character_size), i, &w, &h, &xoff, &yoff);
      stbtt_FreeBitmap(single_channel_bitmap, NULL);

      Rect rect = {
	.pos = (Vector2f){tex_size.x, 0.f},
	.size = (Vector2f){(float32)w, (float32)h}
      };

      Codepoint_rect crect = {
	.offset = (Vector2f){(float32)xoff, (float32)yoff},
	.rect = rect};
      hmput(font->codepoint_rect_map, i, crect);
      tex_size.x += w;

      /* log_f(LOG_INFO, "Codepoint %d(%c) exists", i, i); */
    }
  }

  if (codepoint_min <= 0 || codepoint_max <= 0) {
    log_f(LOG_ERROR, "Font doesn't have any codepoint in the range %d..%d", range.x, range.y);
    return false;
  }

  font->codepoint_start = codepoint_min;
  font->codepoint_end = codepoint_max;

  /* log_f(LOG_INFO, "Font codepoint range: %d..%d", font->codepoint_start, font->codepoint_end); */

  Texture_load_font_atlas(font->texture, (int)tex_size.x, (int)tex_size.y);

  gl(glBindTexture(GL_TEXTURE_2D, font->texture->id));

  for (int i = font->codepoint_start; i <= font->codepoint_end; ++i) {
    if (stbtt_FindGlyphIndex(&font->font, i) != 0) {
      int w, h, xoff, yoff;
      unsigned char* single_channel_bitmap = stbtt_GetCodepointBitmap(&font->font, 0, stbtt_ScaleForPixelHeight(&font->font, (float32)character_size), i, &w, &h, &xoff, &yoff);
      size_t single_channel_bitmap_size = sizeof(unsigned char)*w*h;
      unsigned char* bitmap = onechan_to_fourchan(single_channel_bitmap, single_channel_bitmap_size);
      stbtt_FreeBitmap(single_channel_bitmap, NULL);

      Rect codepoint_rect = {0};
      ASSERT(Font_get_codepoint_rect(font, i, &codepoint_rect));

      /* log_f(LOG_INFO, "Codepoint: %d '%c' -> Rect: %fx%f | %fx%f", i, i, codepoint_rect.pos.x, codepoint_rect.pos.y, codepoint_rect.size.x, codepoint_rect.size.y); */

      gl(glTexSubImage2D(GL_TEXTURE_2D, 0, (GLint)codepoint_rect.pos.x, (GLint)codepoint_rect.pos.y, (GLsizei)codepoint_rect.size.x, (GLsizei)codepoint_rect.size.y, GL_RGBA, GL_UNSIGNED_BYTE, bitmap));
      free(bitmap);
    }
  }

  gl(glBindTexture(GL_TEXTURE_2D, 0));

  return true;
}

void Font_deinit(Font* font) {
  if (font->should_free_ttf_buffer) {
    free(font->ttf_buffer);
  }
  hmfree(font->codepoint_rect_map);
}

unsigned char* onechan_to_fourchan(unsigned char* data, size_t data_size) {
  const int comps = 4;

  unsigned char* res = (unsigned char*)malloc(sizeof(unsigned char)*data_size*comps);

  for (size_t i = 0; i < data_size; ++i) {
    res[(i*comps)+0] = data[i];
    res[(i*comps)+1] = data[i];
    res[(i*comps)+2] = data[i];
    res[(i*comps)+3] = data[i];
  }

  return res;
}
