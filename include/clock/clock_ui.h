#ifndef _CLOCK_UI_H_
#define _CLOCK_UI_H_

#include <clock/clock_core.h>
#include <clock/clock_timer.h>

typedef struct UI UI;
typedef struct UI_Layout UI_Layout;
typedef struct UI_Draw_element UI_Draw_element;

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

typedef enum {
  UI_DRAW_ELEMENT_TYPE_RECT,
  UI_DRAW_ELEMENT_TYPE_BOX,
  UI_DRAW_ELEMENT_TYPE_SPRITE,
  UI_DRAW_ELEMENT_TYPE_TEXT,
  UI_DRAW_ELEMENT_TYPE_COUNT,
} UI_Draw_element_type;

struct UI_Draw_element {
  UI_Draw_element_type type;
  Vector2f pos;
  Vector2f size;
  Color fill_color;
  Color out_color;
  Sprite* spr;
  Font* font;
  cstr text;
  int char_size;
};

typedef struct {
  Arena arena;
  UI_Draw_element* buff;
  size_t count;
} UI_Draw_element_stack;

UI_Draw_element_stack UI_Draw_element_stack_make(void);
void UI_Draw_element_stack_push(UI_Draw_element_stack* stack, UI_Draw_element val);
bool UI_Draw_element_stack_pop(UI_Draw_element_stack* stack, UI_Draw_element* popped);
void UI_Draw_element_stack_free(UI_Draw_element_stack* stack);

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
  Context* ctx;
  Font* font;
  Vector2f bg_padding;
  Color bg_color;
  Vector2f pos;
  Vector2f pos_offset;
  bool is_moving;
  Alarm text_input_cursor_blink_alarm;
  bool show_text_input_cursor;
  UI_Draw_element_stack draw_element_stack;
  float titlebar_height;
  Color titlebar_color;
};

UI UI_make(Context* ctx, Font* font, Vector2f pos);
void UI_push_layout(UI* this, UI_Layout layout);
UI_Layout UI_pop_layout(UI* this);
UI_Layout* UI_top_layout(UI* this);
void UI_begin_layout(UI* this, UI_Layout_kind kind);
void UI_end_layout(UI* this);
void UI_free(UI* this);

void UI_begin(UI* this, UI_Layout_kind kind);
bool UI_button(UI* this, cstr text, int char_size, Color color);
void UI_text(UI* this, cstr text, int char_size, Color color);
void UI_spacing(UI* this, float spacing);
void UI_sprite(UI* this, Sprite* spr);
bool UI_sprite_button(UI* this, Sprite* spr);
void UI_text_input(UI* this, char* text_buff, uint32 text_buff_size, uint32* cursor, int char_size, Color color);
void UI_background(UI* this);
void UI_end(UI* this);

#endif /* _CLOCK_UI_H_ */
