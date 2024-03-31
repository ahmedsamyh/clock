#include <clock/clock_ui.h>
#include <assert.h>

Vector2f UI_Layout_available_pos(UI_Layout* this) {
  switch (this->kind) {
  case UI_LAYOUT_KIND_HORZ: {
    return (Vector2f) {
      .x = this->pos.x + this->size.x + this->padding.x,
      .y = this->pos.y,
    };
  } break;
  case UI_LAYOUT_KIND_VERT: {
    return (Vector2f) {
      .x = this->pos.x,
      .y = this->pos.y + this->size.y + this->padding.y,
    };
  } break;
  default: assert(0 && "Unreachable");
  }
  assert(0 && "Unreachable");

  return (Vector2f) {0.f, 0.f};
}

void UI_Layout_push_widget(UI_Layout* this, Vector2f size) {
  switch (this->kind) {
  case UI_LAYOUT_KIND_HORZ: {
    this->size.x += size.x + this->padding.x;
    this->size.y = max(this->size.y, size.y);
  } break;
  case UI_LAYOUT_KIND_VERT: {
    this->size.x = max(this->size.x, size.x);
    this->size.y += size.y + this->padding.y;
  } break;
  default: assert(0 && "Unreachable");
  }
}

UI UI_make(Context* ctx, Font* font) {
  UI res;
  res.active_id = -1;
  res.layouts_count = 0;
  res.ctx = ctx;
  res.font = font;
  res.btn_padding = (Vector2f) {4.f, 4.f};
  res.text_input_width = 12;

  return res;
}

void UI_push_layout(UI* this, UI_Layout layout) {
  assert(this->layouts_count < LAYOUTS_CAP);
  this->layouts[this->layouts_count++] = layout;
}

UI_Layout UI_pop_layout(UI* this) {
  assert(this->layouts_count > 0);
  return this->layouts[--this->layouts_count];
}

UI_Layout* UI_top_layout(UI* this) {
  if (this->layouts_count > 0)
    return &this->layouts[this->layouts_count - 1];
  return NULL;
}

void UI_begin_layout(UI* this, UI_Layout_kind kind) {
  UI_Layout* prev = UI_top_layout(this);
  if (prev == NULL) {
    log_f(LOG_ERROR, "This function must be used between 'begin' and 'end'!");
    return;
  }

  UI_Layout next = {0};
  next.kind = kind;
  next.pos = UI_Layout_available_pos(prev);
  next.size = (Vector2f) {0.f, 0.f};
  UI_push_layout(this, next);
}

void UI_end_layout(UI* this) {
  UI_Layout child = UI_pop_layout(this);
  UI_Layout* parent = UI_top_layout(this);
  if (parent == NULL) {
    log_f(LOG_ERROR, "This function must be used between 'begin' and 'end'!");
    return;
  }
  UI_Layout_push_widget(parent, child.size);
}

void UI_begin(UI* this, Vector2f pos, UI_Layout_kind kind) {
  UI_Layout layout = {0};
  layout.pos = pos;
  layout.kind = kind;
  UI_push_layout(this, layout);
}

bool UI_button(UI* this, cstr text, int char_size, Color color) {
  int id = this->last_used_id++;
  UI_Layout* top = UI_top_layout(this);
  if (top == NULL) {
    log_f(LOG_ERROR, "This function must be used between 'begin' and 'end'!");
    return false;
  }
  assert(this->ctx);
  Context* ctx = this->ctx;

  const Vector2f pos = UI_Layout_available_pos(top);
  const Vector2f size = v2f_add(get_text_size(this->ctx, this->font, text, char_size), v2f_muls(this->btn_padding, 2.f));
  const Rect rect = {pos, size};
  bool click = false;
  bool hovering = Rect_contains_point(rect, ctx->mpos);
  if (this->active_id == id) {
    if (ctx->m[MOUSE_BUTTON_LEFT].released) {
      this->active_id = -1;
      if (hovering) {
	click = true;
      }
    }
  } else {
    if (hovering && ctx->m[MOUSE_BUTTON_LEFT].pressed) {
      this->active_id = id;
    }
  }

  float alpha = 0.4f;
  if (hovering) {
    alpha = 0.5f;
  }

  bool is_clicked = (hovering && ctx->m[MOUSE_BUTTON_LEFT].held);
  if (is_clicked) {
    alpha = 1.f;
  }

  draw_rect(ctx, rect, color_alpha(color, alpha));

  Vector2f draw_pos = v2f_add(pos, this->btn_padding);
  if (is_clicked) {
    draw_pos = v2f_adds(draw_pos, 1);
  }
  draw_text(ctx, this->font, text, draw_pos, char_size, COLOR_WHITE);

  UI_Layout_push_widget(top, size);

  return click;
}

void UI_text(UI* this, cstr text, int char_size, Color color) {
  int id = this->last_used_id++;
  UI_Layout* top = UI_top_layout(this);
  if (top == NULL) {
    log_f(LOG_ERROR, "This function must be used between 'begin' and 'end'!");
    return;
  }
  assert(this->ctx);
  Context* ctx = this->ctx;

  const Vector2f pos = UI_Layout_available_pos(top);
  const Vector2f size = v2f_add(get_text_size(this->ctx, this->font, text, char_size), v2f_muls(this->btn_padding, 2.f));
  draw_text(ctx, this->font, text, pos, char_size, color);
  UI_Layout_push_widget(top, size);
}

void UI_sprite(UI* this, Sprite* spr) {
  int id = this->last_used_id++;
  UI_Layout* top = UI_top_layout(this);
  if (top == NULL) {
    log_f(LOG_ERROR, "This function must be used between 'begin' and 'end'!");
    return;
  }
  assert(this->ctx);
  Context* ctx = this->ctx;

  const Vector2f pos = UI_Layout_available_pos(top);
  const Vector2f size = spr->tex_rect.size;
  draw_sprite_at(ctx, spr, pos);
  UI_Layout_push_widget(top, size);
}

bool UI_sprite_button(UI* this, Sprite* spr) {
  int id = this->last_used_id++;
  UI_Layout* top = UI_top_layout(this);
  if (top == NULL) {
    log_f(LOG_ERROR, "This function must be used between 'begin' and 'end'!");
    return false;
  }
  assert(this->ctx);
  Context* ctx = this->ctx;

  const Vector2f pos = UI_Layout_available_pos(top);
  const Vector2f size = spr->tex_rect.size;
  const Rect rect = {pos, size};
  bool click = false;
  bool hovering = Rect_contains_point(rect, ctx->mpos);
  if (this->active_id == id) {
    if (ctx->m[MOUSE_BUTTON_LEFT].released) {
      this->active_id = -1;
      if (hovering) {
	click = true;
      }
    }
  } else {
    if (hovering && ctx->m[MOUSE_BUTTON_LEFT].pressed) {
      this->active_id = id;
    }
  }

  float alpha = 0.4f;
  if (hovering) {
    alpha = 0.5f;
  }

  bool is_clicked = (hovering && ctx->m[MOUSE_BUTTON_LEFT].held);
  if (is_clicked) {
    alpha = 1.f;
  }
  Color previous_tint = spr->tint;
  spr->tint.a = alpha;
  draw_sprite_at(ctx, spr, pos);
  spr->tint = previous_tint;

  UI_Layout_push_widget(top, size);

  return click;
}

void UI_spacing(UI* this, float spacing) {
  int id = this->last_used_id++;
  UI_Layout* top = UI_top_layout(this);
  if (top == NULL) {
    log_f(LOG_ERROR, "This function must be used between 'begin' and 'end'!");
    return;
  }
  assert(this->ctx);
  Context* ctx = this->ctx;

  Vector2f size = {
    .x = spacing,
    .y = 0.f,
  };

  if (top->kind == UI_LAYOUT_KIND_VERT) {
    size.x = 0.f;
    size.y = spacing;
  }

  UI_Layout_push_widget(top, size);
}

void UI_text_input(UI* this, char* text_buff, uint32 text_buff_size, uint32* cursor_ptr, int char_size, Color color) {
  uint32 cursor = (*cursor_ptr);
  int id = this->last_used_id++;
  UI_Layout* top = UI_top_layout(this);
  if (top == NULL) {
    log_f(LOG_ERROR, "This function must be used between 'begin' and 'end'!");
    return;
  }
  assert(this->ctx);
  Context* ctx = this->ctx;

  const Vector2f pos = UI_Layout_available_pos(top);
  // TODO: maybe have text input padding?
  const Vector2f size = v2f_add((Vector2f) {this->text_input_width * char_size, char_size}, v2f_muls(this->btn_padding, 2.f));
  const Rect rect = {pos, size};
  bool hovering = Rect_contains_point(rect, ctx->mpos);
  if (this->active_id == id) {
    // backspace
    if (clock_key_pressed(ctx, KEY_BACKSPACE)) {
      if (cursor > 0) {
	uint32 n = text_buff_size - cursor;
	if (n == 0) {
	  text_buff[--cursor] = '\0';
	} else {
	  memcpy((uint8*)text_buff+(cursor-1), (uint8*)text_buff+cursor, n);
	  cursor--;
	  // Edge case: text buffer is full, cursor is not at the end of text buffer
	  if (text_buff[text_buff_size-1] != '\0') {
	    memset((uint8*)text_buff+cursor+n, 0, text_buff_size - (cursor + n));
	  }
	}
      }
    }

    // enter
    if (clock_key_pressed(ctx, KEY_ENTER)) {
      if (clock_key_held(ctx, KEY_LEFT_CONTROL)) {
	this->active_id = -1;
      } else {
	text_buff[cursor] = '\n';
	cursor++;
      }
    }

    if (ctx->text_entered) {
      text_buff[cursor] = ctx->last_entered_character;
      cursor++;
    }

    // cursor movement
        if (clock_key_pressed(ctx, KEY_LEFT)) {
      if (cursor > 0) cursor--;
    }

    if (clock_key_pressed(ctx, KEY_RIGHT)) {
      uint32 text_len = strlen(text_buff);
      if (cursor < text_len) cursor++;
    }

    clock_eat_input(ctx);

    if (!hovering && ctx->m[MOUSE_BUTTON_LEFT].released) {
      this->active_id = -1;
    }
  } else {
    if (hovering && ctx->m[MOUSE_BUTTON_LEFT].pressed) {
      this->active_id = id;
    }
  }

  Color fill_col = color_alpha(COLOR_WHITE, this->active_id == id ? 0.2f : 0.f);
  draw_box(ctx, rect, COLOR_WHITE, fill_col);
  // TODO: look previous todo...
  Vector2f text_pos = v2f_add(pos, this->btn_padding);
  float text_width = get_text_size(this->ctx, this->font, text_buff, char_size).x;
  float text_box_width = (this->text_input_width * char_size);
  if (text_width > text_box_width) {
    text_pos.x -= text_width - text_box_width;
  }

  clock_begin_scissor(ctx, rect);
  float alpha = 0.5f;
  if (this->active_id == id) {
    alpha = 1.f;
  }
  color.a = alpha;
  draw_text(ctx, this->font, text_buff, text_pos, char_size, color);
  clock_end_scissor(ctx);

  // cursor
  assert(cursor <= text_buff_size);
  float text_width_until_cursor = get_text_sizen(this->ctx, this->font, text_buff, cursor, char_size).x;
  Rect cursor_rect = {
    .pos = (Vector2f) {text_pos.x + text_width_until_cursor, text_pos.y},
    .size = (Vector2f) {char_size*0.2f, char_size}
  };
  draw_rect(ctx, cursor_rect, color_alpha(COLOR_WHITE, 0.5f));

  UI_Layout_push_widget(top, size);
  *cursor_ptr = cursor;
}

void UI_end(UI* this) {
  this->last_used_id = 0;
  UI_pop_layout(this);
}
