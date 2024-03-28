#include <rpg/common.h>

Vector2f pos_in_tile_space(Vector2f pos) {
  Vector2f res;
  res.x = (float)((int)(pos.x / TILE_SIZE) * (int)TILE_SIZE);
  res.y = (float)((int)(pos.y / TILE_SIZE) * (int)TILE_SIZE);

  return res;
}

bool Sprite_init_scaled(Sprite* spr, Texture* tex, int hframes, int vframes) {
  if (!Sprite_init(spr, tex, hframes, vframes)) return false;

  spr->scale.x = SCALE;
  spr->scale.y = SCALE;

  return true;
}

Texture* load_texture_err_handled(Context* ctx, const char* filepath) {
  Texture* tex = Resman_load_texture_from_file(ctx->resman, filepath);
  if (tex == NULL) exit(1);
  return tex;
}

static int32 get_index_of_char(cstr str, char ch) {
  int32 idx = 0;
  while (*str != '\0') {
    if (*str++ == ch) {
      return idx;
    }
    idx++;
  }
  return -1;
}

void draw_spr_text(Context* ctx, Sprite* spr_font, cstr text, Vector2f pos, Color color, cstr font_map) {
  while (*text != '\0') {
    char ch = *text++;

    Sprite_set_hframe(spr_font, get_index_of_char(font_map, ch));
    spr_font->tint = color;

    spr_font->pos = pos;
    draw_sprite(ctx, spr_font);
    pos.x += spr_font->tex_rect.size.x * spr_font->scale.x;
  }
}

void draw_text_outlined(Context* ctx, Font* font, cstr text, Vector2f pos, int char_size, Color color, Color out_color) {
  Color col = out_color;
  float offset = 4.f;
  Vector2f p = v2f_adds(pos, offset);
  draw_text(ctx, font, text, p, char_size, col);
  col = color;
  p = v2f_subs(p, offset);
  draw_text(ctx, font, text, p, char_size, col);
}
