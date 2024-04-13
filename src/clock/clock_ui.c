#include <clock/clock_ui.h>
#include <float.h>

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
  default: ASSERT(0 && "Unreachable");
  }
  ASSERT(0 && "Unreachable");

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
  default: ASSERT(0 && "Unreachable");
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
  res.bg_padding = (Vector2f) {10.f, 10.f};
  res.text_input_cursor_blink_alarm.alarm_time = 0.5f;
  res.show_text_input_cursor = true;

  return res;
}

void UI_push_layout(UI* this, UI_Layout layout) {
  ASSERT(this->layouts_count < LAYOUTS_CAP);
  this->layouts[this->layouts_count++] = layout;
}

UI_Layout UI_pop_layout(UI* this) {
  ASSERT(this->layouts_count > 0);
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

void UI_begin(UI* this, Vector2f* pos, UI_Layout_kind kind) {
  UI_Layout layout = {0};
  this->active_pos = pos;
  layout.pos = *pos;
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
  ASSERT(this->ctx);
  Context* ctx = this->ctx;

  const Vector2f pos = UI_Layout_available_pos(top);
  const Vector2f size = v2f_add(get_text_size(this->ctx, this->font, text, char_size), v2f_muls(this->btn_padding, 2.f));
  const Rect rect = {pos, size};
  bool click = false;
  bool hovering = Rect_contains_point(rect, ctx->mpos);
  if (this->active_id == id) {
    if (clock_mouse_released(ctx, MOUSE_BUTTON_LEFT)) {
      this->active_id = -1;
      if (hovering) {
	click = true;
      }
    }
  } else {
    if (hovering && clock_mouse_pressed(ctx, MOUSE_BUTTON_LEFT)) {
      this->active_id = id;
      clock_eat_mouse_input(ctx);
    }
  }

  float alpha = 0.4f;
  if (hovering) {
    alpha = 0.5f;
  }

  bool is_clicked = (hovering && clock_mouse_held(ctx, MOUSE_BUTTON_LEFT));
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
  (void)id;
  UI_Layout* top = UI_top_layout(this);
  if (top == NULL) {
    log_f(LOG_ERROR, "This function must be used between 'begin' and 'end'!");
    return;
  }
  ASSERT(this->ctx);
  Context* ctx = this->ctx;

  const Vector2f pos = UI_Layout_available_pos(top);
  const Vector2f size = v2f_add(get_text_size(this->ctx, this->font, text, char_size), v2f_muls(this->btn_padding, 2.f));
  draw_text(ctx, this->font, text, pos, char_size, color);
  UI_Layout_push_widget(top, size);
}

void UI_sprite(UI* this, Sprite* spr) {
  int id = this->last_used_id++;
  (void)id;
  UI_Layout* top = UI_top_layout(this);
  if (top == NULL) {
    log_f(LOG_ERROR, "This function must be used between 'begin' and 'end'!");
    return;
  }
  ASSERT(this->ctx);
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
  ASSERT(this->ctx);
  Context* ctx = this->ctx;

  const Vector2f pos = UI_Layout_available_pos(top);
  const Vector2f size = spr->tex_rect.size;
  const Rect rect = {pos, size};
  bool click = false;
  bool hovering = Rect_contains_point(rect, ctx->mpos);
  if (this->active_id == id) {
    if (clock_mouse_released(ctx, MOUSE_BUTTON_LEFT)) {
      this->active_id = -1;
      if (hovering) {
	click = true;
      }
    }
  } else {
    if (hovering && clock_mouse_pressed(ctx, MOUSE_BUTTON_LEFT)) {
      this->active_id = id;
    }
  }

  float alpha = 0.4f;
  if (hovering) {
    alpha = 0.5f;
  }

  bool is_clicked = (hovering && clock_mouse_held(ctx, MOUSE_BUTTON_LEFT));
  if (is_clicked) {
    alpha = 1.f;
  }

  Color previous_tint = spr->tint;
  spr->tint.a = alpha;
  draw_sprite_at(ctx, spr, pos);
  spr->tint = previous_tint;

  UI_Layout_push_widget(top, size);

  if (click) clock_eat_mouse_input(ctx);

  return click;
}

void UI_spacing(UI* this, float spacing) {
  int id = this->last_used_id++;
  (void)id;
  UI_Layout* top = UI_top_layout(this);
  if (top == NULL) {
    log_f(LOG_ERROR, "This function must be used between 'begin' and 'end'!");
    return;
  }
  ASSERT(this->ctx);

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
  ASSERT(this->ctx);
  Context* ctx = this->ctx;

  const Vector2f pos = UI_Layout_available_pos(top);
  // TODO: maybe have text input padding?
  const Vector2f size = v2f_add((Vector2f) {this->text_input_width * (real32)char_size, (real32)char_size}, v2f_muls(this->btn_padding, 2.f));
  const Rect rect = {pos, size};
  bool hovering = Rect_contains_point(rect, ctx->mpos);
  if (this->active_id == id) {
    bool pressed = false;

    // Backspace
    if (clock_key_pressed(ctx, KEY_BACKSPACE) && cursor > 0) {
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
      pressed = true;
    }

    // Delete
    if (clock_key_pressed(ctx, KEY_DELETE) &&
	cursor < (uint32)strlen(text_buff)) {
      uint32 n = text_buff_size - cursor;

      if (n == 0) {
	text_buff[cursor--] = '\0';
      } else {
	memcpy((uint8*)text_buff+(cursor), (uint8*)text_buff+(cursor+1), n-1);
	// TODO: Check for edge-case
      }
      pressed = true;
    }

    // enter
    if (clock_key_pressed(ctx, KEY_ENTER)) {
      if (clock_key_held(ctx, KEY_LEFT_CONTROL)) {
	this->active_id = -1;
      } else {
	/* 	text_buff[cursor] = '\n'; */
	/* 	cursor++; */
      }
      pressed = true;
    }

    if (ctx->text_entered) {
      text_buff[cursor] = (char)ctx->last_entered_character;
      cursor++;
      pressed = true;
    }

    // Pasting
    if (clock_key_held(ctx, KEY_LEFT_CONTROL)) {
      if (clock_key_pressed(ctx, KEY_V)) {
	cstr pasted_text = get_clipboard();
	size_t pasted_text_len = strlen(pasted_text);
	ASSERT((pasted_text_len + cursor) <= text_buff_size);
	memcpy((uint8*)text_buff+cursor, pasted_text, pasted_text_len);
	cursor += (uint32)pasted_text_len;
      }
      pressed = true;
    }

    // Home/End
    if (clock_key_pressed(ctx, KEY_HOME)) {
      cursor = 0;
      pressed = true;
    } else if (clock_key_pressed(ctx, KEY_END)) {
      cursor = (uint32)strlen(text_buff);
      pressed = true;
    }

    // Cursor movement
    if (clock_key_pressed(ctx, KEY_LEFT)) {
      if (cursor > 0) cursor--;
      pressed = true;
    }

    if (clock_key_pressed(ctx, KEY_RIGHT)) {
      size_t text_len = strlen(text_buff);
      if (cursor < text_len) cursor++;
      pressed = true;
    }

    clock_eat_key_input(ctx);

    if (!hovering && clock_mouse_released(ctx, MOUSE_BUTTON_LEFT)) {
      this->active_id = -1;
    }

    if (!pressed) {
      if (Alarm_on_alarm(&this->text_input_cursor_blink_alarm, ctx->delta)) {
	this->show_text_input_cursor = !this->show_text_input_cursor;
      }
    } else {
      this->text_input_cursor_blink_alarm.time = 0.f;
      this->show_text_input_cursor = true;
    }

  } else {
    this->show_text_input_cursor = true;
    if (hovering && clock_mouse_pressed(ctx, MOUSE_BUTTON_LEFT)) {
      this->active_id = id;
      clock_eat_mouse_input(ctx);
    }
  }

  Color fill_col = color_alpha(COLOR_WHITE, this->active_id == id ? 0.2f : 0.f);
  Vector2f text_box_pos = pos;
  draw_box(ctx, rect, COLOR_WHITE, fill_col);
  // TODO: look previous todo...
  Vector2f text_pos = v2f_add(pos, this->btn_padding);
  float text_width = get_text_size(this->ctx, this->font, text_buff, char_size).x;
  float text_box_width = ((real32)this->text_input_width * (real32)char_size);
  if (text_width > text_box_width) {
    text_pos.x -= text_width - text_box_width;
  }

  // offset by cursor
  ASSERT(cursor <= text_buff_size);
  float text_width_until_cursor = get_text_sizen(this->ctx, this->font, text_buff, cursor, char_size).x;
  Rect cursor_rect = {
    .pos = (Vector2f) {text_pos.x + text_width_until_cursor, text_pos.y},
    .size = (Vector2f) {char_size*0.2f, (real32)char_size}
  };

  if (cursor_rect.pos.x < text_box_pos.x) {
    text_pos.x += text_box_pos.x - cursor_rect.pos.x;
    cursor_rect.pos.x = text_box_pos.x;
  }

  clock_begin_scissor(ctx, rect);
  color.a = 0.5f;
  if (this->active_id == id) {
    color.a = 1.f;
  }

  draw_text(ctx, this->font, text_buff, text_pos, char_size, color);
  clock_end_scissor(ctx);

  // cursor

  if (this->show_text_input_cursor) {
    draw_rect(ctx, cursor_rect, color_alpha(COLOR_WHITE, (this->active_id == id ? 0.85f : 0.45f)));
  }

  UI_Layout_push_widget(top, size);
  *cursor_ptr = cursor;
}

void UI_background(UI* this) {
  Vector2f min = {FLT_MAX, FLT_MAX};
  Vector2f max = {FLT_MIN, FLT_MIN};
  for (size_t i = 0; i < this->layouts_count; ++i) {
    if (this->layouts[i].pos.x < min.x) min.x = this->layouts[i].pos.x;
    if (this->layouts[i].pos.y < min.y) min.y = this->layouts[i].pos.y;

    if (this->layouts[i].pos.x + this->layouts[i].size.x > max.x) max.x = this->layouts[i].pos.x + this->layouts[i].size.x;
    if (this->layouts[i].pos.y + this->layouts[i].size.y > max.y) max.y = this->layouts[i].pos.y + this->layouts[i].size.y;
  }

  Rect rect = {
    .pos = v2f_sub(min, this->bg_padding),
    .size = v2f_add(v2f_sub(max, min), v2f_muls(this->bg_padding, 2.f))
  };

  draw_box(this->ctx, rect, COLOR_WHITE, color_alpha(COLOR_BLACK, 0.f));
}

void UI_end(UI* this) {
  UI_background(this);

  Context* ctx = this->ctx;

  Vector2f min = {FLT_MAX, FLT_MAX};
  Vector2f max = {FLT_MIN, FLT_MIN};
  for (size_t i = 0; i < this->layouts_count; ++i) {
    if (this->layouts[i].pos.x < min.x) min.x = this->layouts[i].pos.x;
    if (this->layouts[i].pos.y < min.y) min.y = this->layouts[i].pos.y;

    if (this->layouts[i].pos.x + this->layouts[i].size.x > max.x) max.x = this->layouts[i].pos.x + this->layouts[i].size.x;
    if (this->layouts[i].pos.y + this->layouts[i].size.y > max.y) max.y = this->layouts[i].pos.y + this->layouts[i].size.y;
  }

  Rect rect = {
    .pos = v2f_sub(min, this->bg_padding),
    .size = v2f_add(v2f_sub(max, min), v2f_muls(this->bg_padding, 2.f))
  };

  if (!clock_mouse_held(ctx, MOUSE_BUTTON_LEFT)) {
    this->is_moving = false;
  }

  if (clock_key_held(ctx, KEY_LEFT_ALT) &&
      clock_mouse_pressed(ctx, MOUSE_BUTTON_LEFT) &&
      Rect_contains_point(rect, ctx->mpos)) {
    this->active_pos_offset = v2f_sub(ctx->mpos, rect.pos);
    this->is_moving = true;
  }

  if (this->is_moving) {
    clock_eat_mouse_input(ctx);
    *this->active_pos = v2f_add(v2f_sub(ctx->mpos, this->active_pos_offset), this->bg_padding);
  }

  this->last_used_id = 0;
  UI_pop_layout(this);
}
