#ifndef _CLOCK_UI_H_
#define _CLOCK_UI_H_

#include <clock/clock_core.h>

typedef struct UI UI;
typedef struct UI_Layout UI_Layout;

typedef enum {
  UI_LAYOUT_KIND_HORZ,
  UI_LAYOUT_KIND_VERT,
  UI_LAYOUT_KIND_COUNT
} UI_Layout_kind;

struct UI_Layout {
  UI_Layout_kind kind;
  Vector2f pos;
  Vector2f size;
  Vector2f padding;
};

Vector2f UI_Layout_available_pos(UI_Layout* this);
void UI_Layout_push_widget(UI_Layout* this, Vector2f size);

struct UI {
  int active_id;
  int last_used_id;
#define LAYOUTS_CAP 256
  UI_Layout layouts[LAYOUTS_CAP];
  size_t layouts_count;
  Vector2f btn_padding;
  int text_input_width; // in characters, so depends on the char_size
  /* int hover_time; */
  /* int hover_time_max; */
  Context* ctx;
  Font* font;
};

UI UI_make(Context* ctx, Font* font);
void UI_push_layout(UI* this, UI_Layout layout);
UI_Layout UI_pop_layout(UI* this);
UI_Layout* UI_top_layout(UI* this);
void UI_begin_layout(UI* this, UI_Layout_kind kind);
void UI_end_layout(UI* this);

void UI_begin(UI* this, Vector2f pos, UI_Layout_kind kind);
bool UI_button(UI* this, cstr text, int char_size, Color color);
void UI_text(UI* this, cstr text, int char_size, Color color);
void UI_spacing(UI* this, float spacing);
void UI_sprite(UI* this, Sprite* spr);
bool UI_sprite_button(UI* this, Sprite* spr);
void UI_text_input(UI* this, cstr* text, int char_size, Color color);
void UI_end(UI* this);

#endif /* _CLOCK_UI_H_ */
