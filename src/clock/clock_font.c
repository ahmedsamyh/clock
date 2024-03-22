#include <clock/clock_font.h>
#include <stdlib.h>
#include <commonlib.h>
#include <clock/clock_core.h>
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb/stb_truetype.h>
#include <stb/stb_ds.h>

bool Font_load(Font* font, const char* filepath) {
  unsigned char* ttf_buffer = (char*)slurp_file(filepath);

  if (ttf_buffer == NULL) {
    log_f(LOG_ERROR, "Could not load font file '%s'!", filepath);
    return false;
  }

  stbtt_InitFont(&font->font, (const unsigned char*)ttf_buffer, stbtt_GetFontOffsetForIndex((const unsigned char *)ttf_buffer,0));

  font->ttf_buffer = ttf_buffer;

  return true;
}

Codepoint_metrics Font_get_codepoint_metrics(Font* font, int codepoint) {
    Codepoint_rect_KV* crkv= hmgetp_null(font->codepoint_rect_map, codepoint);

    assert(crkv != NULL);

    return crkv->value;
}

bool Font_get_atlas_tex(Font* font, Texture* tex, int character_size) {

  font->codepoint_rect_map = NULL;

  Vector2f tex_size = {0.f, character_size};
  for (int i = 33; i <= 127; ++i) {
    if (stbtt_FindGlyphIndex(&font->font, i) != 0) {
      int w, h, xoff, yoff;
      unsigned char* single_channel_bitmap = stbtt_GetCodepointBitmap(&font->font, 0, stbtt_ScaleForPixelHeight(&font->font, character_size), i, &w, &h, &xoff, &yoff);
      stbtt_FreeBitmap(single_channel_bitmap, NULL);

      Codepoint_metrics cp_metrics = {
     	.rect = (Rect){
	  .pos = (Vector2f){tex_size.x, 0.f},
	  .size = (Vector2f){w, h}},
	.offset = (Vector2f){xoff, yoff}
      };

      hmput(font->codepoint_rect_map, i, cp_metrics);

      tex_size.x += w;
    }
  }

  Texture_load_empty(tex, tex_size.x, tex_size.y);

  gl(glBindTexture(GL_TEXTURE_2D, tex->id));

  // TODO (speed): Maybe store the codepoints that exist and loop over those?
  for (int i = 33; i <= 127; ++i) {
    if (stbtt_FindGlyphIndex(&font->font, i) != 0) {
      int w, h, xoff, yoff;
      unsigned char* single_channel_bitmap = stbtt_GetCodepointBitmap(&font->font, 0, stbtt_ScaleForPixelHeight(&font->font, character_size), i, &w, &h, &xoff, &yoff);
      size_t single_channel_bitmap_size = sizeof(unsigned char)*w*h;
      unsigned char* bitmap = onechan_to_fourchan(single_channel_bitmap, single_channel_bitmap_size);
      stbtt_FreeBitmap(single_channel_bitmap, NULL);

      Codepoint_rect_KV* crkv= hmgetp_null(font->codepoint_rect_map, i);

      assert(crkv != NULL);

      Rect codepoint_rect = crkv->value.rect;

      log_f(LOG_INFO, "Codepoint: %d '%c' -> Rect: %fx%f | %fx%f", i, i, codepoint_rect.pos.x, codepoint_rect.pos.y, codepoint_rect.size.x, codepoint_rect.size.y);

      gl(glTexSubImage2D(GL_TEXTURE_2D, 0, codepoint_rect.pos.x, codepoint_rect.pos.y, codepoint_rect.size.x, codepoint_rect.size.y, GL_RGBA, GL_UNSIGNED_BYTE, bitmap));
      free(bitmap);
    }
  }

  gl(glBindTexture(GL_TEXTURE_2D, 0));

  return true;
}

void Font_unload(Font* font) {
  free(font->ttf_buffer);
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
