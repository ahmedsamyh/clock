#include <ft2build.h>
#include <freetype/freetype.h>

#define FT_PRINT_HAS_FACE_ATTR(ftface, macro, msg) \
  log_f(LOG_INFO, "Font face %s %s", (macro(ftface) ? "has" : "does not have"), msg)

#define FT_PRINT_IS_FACE_ATTR(ftface, macro, msg) \
  log_f(LOG_INFO, "Font face %s %s", (macro(ftface) ? "is" : "isnt"), msg)

#define FT_CALL(func, ...) \
  err = func(__VA_ARGS__);\
  if (err != 0) {\
    log_f(LOG_ERROR, "%s() -> %s", #func, FT_Error_String(err));\
    return 1;\
  }

const char* ft_pixel_mode_str(unsigned char pixel_mode) {
  switch (pixel_mode) {
  case FT_PIXEL_MODE_NONE: return "FT_PIXEL_MODE_NONE"; break;
  case FT_PIXEL_MODE_MONO: return "FT_PIXEL_MODE_MONO"; break;
  case FT_PIXEL_MODE_GRAY: return "FT_PIXEL_MODE_GRAY"; break;
  case FT_PIXEL_MODE_GRAY2: return "FT_PIXEL_MODE_GRAY2"; break;
  case FT_PIXEL_MODE_GRAY4: return "FT_PIXEL_MODE_GRAY4"; break;
  case FT_PIXEL_MODE_LCD: return "FT_PIXEL_MODE_LCD"; break;
  case FT_PIXEL_MODE_LCD_V: return "FT_PIXEL_MODE_LCD_V"; break;
  case FT_PIXEL_MODE_BGRA: return "FT_PIXEL_MODE_BGRA"; break;
  }
  return "THIS SHOULD NOT BE RETURNED";
}

const char* ft_encoding_str(FT_Face face) {
  switch (face->charmap->encoding) {
  case FT_ENCODING_NONE:	   return "None";
  case FT_ENCODING_MS_SYMBOL:	   return "Ms_symbol";
  case FT_ENCODING_UNICODE:	   return "Unicode";
  case FT_ENCODING_SJIS:	   return "Sjis";
  case FT_ENCODING_PRC:		   return "Prc";
  case FT_ENCODING_BIG5:	   return "Big5";
  case FT_ENCODING_WANSUNG:	   return "Wansung";
  case FT_ENCODING_JOHAB:	   return "Johab";
  case FT_ENCODING_ADOBE_STANDARD: return "Adobe_standard";
  case FT_ENCODING_ADOBE_EXPERT:   return "Adobe_expert";
  case FT_ENCODING_ADOBE_CUSTOM:   return "Adobe_custom";
  case FT_ENCODING_ADOBE_LATIN_1:  return "Adobe_latin_1";
  case FT_ENCODING_OLD_LATIN_2:	   return "Old_latin_2";
  case FT_ENCODING_APPLE_ROMAN:	   return "Apple_roman";
  }
  return "THIS SHOULD NOT BE RETURNED";
}
