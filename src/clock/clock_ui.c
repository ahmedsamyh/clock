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

UI UI_make(Context* ctx, Font* font, Vector2f pos, cstr title) {
  UI res;
  res.active_id = -1;
  res.layouts_count = 0;
  res.ctx = ctx;
  res.pos = pos;
  res.font = font;
  res.btn_padding = (Vector2f) {4.f, 4.f};
  res.text_input_width = 12;
  res.bg_padding = (Vector2f) {10.f, 10.f};
  res.bg_color = COLOR_GRAY12;
  res.text_input_cursor_blink_alarm.alarm_time = 0.5f;
  res.show_text_input_cursor = true;
  res.draw_element_stack = UI_Draw_element_stack_make();
  res.titlebar_color = COLOR_BLACK;
  res.title = title;
  res.title_char_size = 16;
  res.titlebar_padding = 4;
  res.titlebar_height = (float)(res.title_char_size + (res.titlebar_padding*2.f));
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

UI_Draw_element_stack UI_Draw_element_stack_make(void) {
  UI_Draw_element_stack res = {0};

  const size_t draw_element_count = 100;
  res.arena = Arena_make(sizeof(UI_Draw_element)*(draw_element_count+1));
  res.buff = (UI_Draw_element*)Arena_alloc(&res.arena, sizeof(UI_Draw_element)*draw_element_count);

  return res;
}

void UI_Draw_element_stack_push(UI_Draw_element_stack* stack, UI_Draw_element val) {
  stack->buff[stack->count++] = val;
}

bool UI_Draw_element_stack_pop(UI_Draw_element_stack* stack, UI_Draw_element* popped) {
  if (stack->count == 0) {
    return false;
  } else {
    *popped = stack->buff[--stack->count];
  }
  return true;
}


void UI_Draw_element_stack_free(UI_Draw_element_stack* stack) {
  Arena_free(&stack->arena);
}

void UI_begin(UI* this, UI_Layout_kind kind) {
  UI_Layout layout = {0};
  layout.pos = v2f_add(this->pos, (Vector2f) {0.f, (this->titlebar_height*2.f)});
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

  /* draw_rect(ctx, rect, color_alpha(color, alpha)); */

  UI_Draw_element_stack_push(&this->draw_element_stack, (UI_Draw_element) {
      .type = UI_DRAW_ELEMENT_TYPE_RECT,
      .pos = rect.pos,
      .size = rect.size,
      .fill_color = color_alpha(color, alpha),
    });

  Vector2f draw_pos = v2f_add(pos, this->btn_padding);
  if (is_clicked) {
    draw_pos = v2f_adds(draw_pos, 1);
  }

  /* draw_text(ctx, this->font, text, draw_pos, char_size, COLOR_WHITE); */

  UI_Draw_element_stack_push(&this->draw_element_stack, (UI_Draw_element) {
      .type = UI_DRAW_ELEMENT_TYPE_TEXT,
      .pos = draw_pos,
      .fill_color = COLOR_WHITE,
      .out_color = COLOR_WHITE,
      .spr = NULL,
      .font = this->font,
      .text = text,
      .char_size = char_size,
    });

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

  const Vector2f pos = UI_Layout_available_pos(top);
  const Vector2f size = v2f_add(get_text_size(this->ctx, this->font, text, char_size), v2f_muls(this->btn_padding, 2.f));
  /* draw_text(ctx, this->font, text, pos, char_size, color); */
  UI_Draw_element_stack_push(&this->draw_element_stack, (UI_Draw_element) {
      .type = UI_DRAW_ELEMENT_TYPE_TEXT,
      .pos = pos,
      .fill_color = color,
      .out_color = color,
      .spr = NULL,
      .font = this->font,
      .text = text,
      .char_size = char_size,
    });

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

  const Vector2f pos = UI_Layout_available_pos(top);
  const Vector2f size = spr->tex_rect.size;
  /* draw_sprite_at(ctx, spr, pos); */
  UI_Draw_element_stack_push(&this->draw_element_stack, (UI_Draw_element) {
      .type = UI_DRAW_ELEMENT_TYPE_SPRITE,
      .pos = pos,
      .fill_color = COLOR_WHITE,
      .out_color = COLOR_WHITE,
      .spr = spr,
      .font = NULL,
      .text = NULL,
      .char_size = 0,
    });

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

  Color color = spr->tint;
  color.a = alpha;
  /* draw_sprite_at(ctx, spr, pos); */
  UI_Draw_element_stack_push(&this->draw_element_stack, (UI_Draw_element) {
      .type = UI_DRAW_ELEMENT_TYPE_SPRITE,
      .pos = pos,
      .fill_color = color,
      .out_color = color,
      .spr = spr,
      .font = NULL,
      .text = NULL,
      .char_size = 0,
    });

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
  const Vector2f size = v2f_add((Vector2f) {this->text_input_width * (float32)char_size, (float32)char_size}, v2f_muls(this->btn_padding, 2.f));
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
  /* draw_box(ctx, rect, COLOR_WHITE, fill_col); */
  UI_Draw_element_stack_push(&this->draw_element_stack, (UI_Draw_element) {
      .type = UI_DRAW_ELEMENT_TYPE_BOX,
      .pos = rect.pos,
      .size = rect.size,
      .fill_color = fill_col,
      .out_color = COLOR_WHITE,
      .spr = NULL,
      .font = NULL,
      .text = NULL,
      .char_size = 0,
    });

  // TODO: look previous todo...
  Vector2f text_pos = v2f_add(pos, this->btn_padding);
  float text_width = get_text_size(this->ctx, this->font, text_buff, char_size).x;
  float text_box_width = ((float32)this->text_input_width * (float32)char_size);
  if (text_width > text_box_width) {
    text_pos.x -= text_width - text_box_width;
  }

  // offset by cursor
  ASSERT(cursor <= text_buff_size);
  float text_width_until_cursor = get_text_sizen(this->ctx, this->font, text_buff, cursor, char_size).x;
  Rect cursor_rect = {
    .pos = (Vector2f) {text_pos.x + text_width_until_cursor, text_pos.y},
    .size = (Vector2f) {char_size*0.2f, (float32)char_size}
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

  /* draw_text(ctx, this->font, text_buff, text_pos, char_size, color); */
  UI_Draw_element_stack_push(&this->draw_element_stack, (UI_Draw_element) {
      .type = UI_DRAW_ELEMENT_TYPE_TEXT,
      .pos = text_pos,
      .fill_color = color,
      .out_color = color,
      .spr = NULL,
      .font = this->font,
      .text = text_buff,
      .char_size = char_size,
    });

  clock_end_scissor(ctx);

  // cursor

  if (this->show_text_input_cursor) {
    /* draw_rect(ctx, cursor_rect, color_alpha(COLOR_WHITE, (this->active_id == id ? 0.85f : 0.45f))); */
    UI_Draw_element_stack_push(&this->draw_element_stack, (UI_Draw_element) {
      .type = UI_DRAW_ELEMENT_TYPE_RECT,
      .pos = cursor_rect.pos,
      .size = cursor_rect.size,
      .fill_color = color_alpha(COLOR_WHITE, (this->active_id == id ? 0.85f : 0.45f)),
    });
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


  Rect titlebar = {
    .pos = v2f_sub(this->pos, (Vector2f) {this->bg_padding.x, 0.f}),
    .size = (Vector2f) {rect.size.x, this->titlebar_height},
  };

  draw_box(this->ctx, titlebar, COLOR_WHITE, this->titlebar_color);
  draw_text(this->ctx, this->font, this->title, (Vector2f) {titlebar.pos.x, titlebar.pos.y + this->titlebar_padding}, this->title_char_size, COLOR_WHITE);
  draw_box(this->ctx, rect, COLOR_WHITE, this->bg_color);
}

void UI_end(UI* this) {
  UI_background(this);

  // draw ui elements

  UI_Draw_element elm = {0};
  while (UI_Draw_element_stack_pop(&this->draw_element_stack, &elm)) {
    switch (elm.type) {
    case UI_DRAW_ELEMENT_TYPE_RECT: {
      draw_rect(this->ctx, (Rect) {elm.pos, elm.size}, elm.fill_color);
    } break;
    case UI_DRAW_ELEMENT_TYPE_BOX: {
      draw_box(this->ctx, (Rect) {elm.pos, elm.size}, elm.out_color, elm.fill_color);
    } break;
    case UI_DRAW_ELEMENT_TYPE_SPRITE: {
      Color previous_tint = elm.spr->tint;
      elm.spr->tint = elm.fill_color;
      draw_sprite_at(this->ctx, elm.spr, elm.pos);
      elm.spr->tint = previous_tint;
    } break;
    case UI_DRAW_ELEMENT_TYPE_TEXT: {
      draw_text(this->ctx, elm.font, elm.text, elm.pos, elm.char_size, elm.fill_color);
    } break;
    default: ASSERT(0 && "Unreachable!");
    }
  }
  Context* ctx = this->ctx;

  Vector2f min = {FLT_MAX, FLT_MAX};
  Vector2f max = {FLT_MIN, FLT_MIN};
  for (size_t i = 0; i < this->layouts_count; ++i) {
    if (this->layouts[i].pos.x < min.x) min.x = this->layouts[i].pos.x;
    if (this->layouts[i].pos.y < min.y) min.y = this->layouts[i].pos.y;

    if (this->layouts[i].pos.x + this->layouts[i].size.x > max.x) max.x = this->layouts[i].pos.x + this->layouts[i].size.x;
    if (this->layouts[i].pos.y + this->layouts[i].size.y > max.y) max.y = this->layouts[i].pos.y + this->layouts[i].size.y;
  }

  Rect titlebar = {
    .pos = v2f_sub(this->pos, (Vector2f) {this->bg_padding.x, 0.f}),
    .size = (Vector2f) {max.x - min.x, this->titlebar_height},
  };

  if (!clock_mouse_held(ctx, MOUSE_BUTTON_LEFT)) {
    this->is_moving = false;
  }

  if (clock_mouse_pressed(ctx, MOUSE_BUTTON_LEFT) &&
      Rect_contains_point(titlebar, ctx->mpos)) {
    this->pos_offset = v2f_sub(v2f_sub(ctx->mpos, titlebar.pos), (Vector2f) {this->bg_padding.x, 0.f});
    this->is_moving = true;
  }

  if (this->is_moving) {
    clock_eat_mouse_input(ctx);
    this->pos = v2f_sub(ctx->mpos, this->pos_offset);
  }

  // eat mouse input if clicked on ui rect
  {
    Rect rect = {
      .pos = v2f_sub(min, this->bg_padding),
      .size = v2f_add(v2f_sub(max, min), v2f_muls(this->bg_padding, 2.f))
    };

    if (clock_mouse_pressed(ctx, MOUSE_BUTTON_LEFT) &&
	Rect_contains_point(rect, ctx->mpos)) {
      clock_eat_mouse_input(ctx);
    }
  }

  this->last_used_id = 0;
  UI_pop_layout(this);
}

void UI_free(UI* this) {
  UI_Draw_element_stack_free(&this->draw_element_stack);
}
