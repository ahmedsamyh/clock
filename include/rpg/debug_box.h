#ifndef _DEBUG_BOX_H_
#define _DEBUG_BOX_H_

#include <clock/clock_core.h>

typedef struct Debug_box Debug_box;

struct Debug_box {
  Vector2f pos;
  Vector2f size;
  int char_size;
  Context* ctx;
  cstr** text_ptrs; // dynamic-array
  Font* font;
  float vpadding;
};

void Debug_box_default(Debug_box* dbox, Context* ctx, Font* font);
void Debug_box_push_text(Debug_box* dbox, cstr* text);
cstr Debug_box_pop_text(Debug_box* dbox);
void Debug_box_draw(Debug_box* dbox);

#endif /* _DEBUG_BOX_H_ */
