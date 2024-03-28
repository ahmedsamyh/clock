#include <rpg/debug_box.h>

void Debug_box_default(Debug_box* dbox, Context* ctx, Font* font) {
  dbox->ctx = ctx;
  dbox->font = font;
  dbox->char_size = 24;
  dbox->pos = (Vector2f) {0.f, 0.f};
  dbox->pos = (Vector2f) {0.f, 0.f};
  dbox->text_ptrs = NULL; // dynamic-array
  dbox->vpadding = 2.f;
}

void Debug_box_push_text(Debug_box* dbox, cstr* text) {
  Vector2f text_size = get_text_size(dbox->ctx, dbox->font, *text, dbox->char_size);
  if (dbox->size.x < text_size.x) dbox->size.x = text_size.x;
  dbox->size.y += text_size.y + dbox->vpadding;
  arrput(dbox->text_ptrs, text);
}

cstr Debug_box_pop_text(Debug_box* dbox) {
  cstr res = *(dbox->text_ptrs[arrlenu(dbox->text_ptrs)-1]);
  arrdel(dbox->text_ptrs, arrlenu(dbox->text_ptrs)-1);

  Vector2f text_size = get_text_size(dbox->ctx, dbox->font, res, dbox->char_size);

  float max_width = 0.f;
  for (size_t i = 0; i < arrlenu(dbox->text_ptrs); ++i) {
    Vector2f ts = get_text_size(dbox->ctx, dbox->font, *dbox->text_ptrs[i], dbox->char_size);

    if (max_width < ts.x) max_width = ts.x;
  }
  dbox->size.x = max_width;

  dbox->size.y -= text_size.y + dbox->vpadding;
  return res;
}

void Debug_box_draw(Debug_box* dbox) {
  Vector2f pos = dbox->pos;

  float max_width = 0.f;
  for (size_t i = 0; i < arrlenu(dbox->text_ptrs); ++i) {
    Vector2f ts = get_text_size(dbox->ctx, dbox->font, *dbox->text_ptrs[i], dbox->char_size);

    if (max_width < ts.x) max_width = ts.x;
  }
  dbox->size.x = max_width;

  draw_rect(dbox->ctx, (Rect) {pos, dbox->size}, color_alpha(COLOR_BLACK, 0.5f));

  for (size_t i = 0; i < arrlenu(dbox->text_ptrs); ++i) {
    cstr text = *(dbox->text_ptrs[i]);
    draw_text_outlined(dbox->ctx, dbox->font, text, pos, dbox->char_size, COLOR_WHITE, COLOR_BLACK);
    pos.y += dbox->font->current_character_size + dbox->vpadding;
  }
}
