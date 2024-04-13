#include <clock/clock.h>

void draw_bezier_curve(Context* ctx, Vector2f p0, Vector2f p1, Vector2f p2, int res, Color color) {
  Vector2f prev_pos = p0;
  for (int i = 0; i < res; ++i) {
    float t = (i + 1.f) / res;
    Vector2f next_pos = v2f_bezier_lerp(p0, p1, p2, t);
    draw_imm_line(ctx, prev_pos, next_pos, color, color);
    prev_pos = next_pos;
  }
}

int main(void) {
  Context* ctx = clock_init(1280, 720, 1.f, 1.f, "Bezier Curve", WINDOW_RESIZABLE);
  if (!ctx) return 1;

  float w = (float)ctx->win->width;

  Vector2f
    p0 = {w*0.5f + 10.f, 100.f},
    p1 = {100.f, 500.f};
  Vector2f control = {0.f, 0.f};

  const float S = 4.f;
  const Vector2f size = {S, S};

  float t = 0.f;
  while (!clock_should_quit(ctx)) {
    clock_begin_draw(ctx);
    clock_clear(ctx, COLOR_BLACK);

#if 0
    const float R = 1.f;
    if (clock_key_held(ctx, KEY_Z)) {
      t -= R * ctx->delta;
      if (t < 0.f) t = 0.f;
    }
    if (clock_key_held(ctx, KEY_X)) {
      t += R * ctx->delta;
      if (t > 1.f) t = 1.f;
    }
#else
    t = (sinf((float)glfwGetTime()*2.f) + 1.f) / 2.f;
#endif
    control = ctx->mpos;

    if (clock_mouse_pressed(ctx, MOUSE_BUTTON_LEFT)) {
      p0 = ctx->mpos;
    }

    if (clock_mouse_pressed(ctx, MOUSE_BUTTON_RIGHT)) {
      p1 = ctx->mpos;
    }

    draw_rect_centered(ctx, (Rect){.pos = p0, .size = size}, COLOR_RED);
    draw_rect_centered(ctx, (Rect){.pos = p1, .size = size}, COLOR_RED);
    draw_rect_centered(ctx, (Rect){.pos = control, .size = size}, COLOR_GREEN);

    const Color C =  color_alpha(COLOR_WHITE, 0.2f);
    draw_imm_line(ctx, p0, p1, C, C);

    Vector2f middle = v2f_lerp(p0, p1, t);
    draw_rect_centered(ctx, (Rect){.pos = middle, .size = v2f_muls(size, 1.5f)}, COLOR_GREEN);

    Vector2f b = v2f_bezier_lerp(p0, control, p1, t);
    draw_rect_centered(ctx, (Rect){.pos = b, .size = v2f_muls(size, 2.f)}, COLOR_GOLD);

    draw_bezier_curve(ctx, p0, control, p1, 100, COLOR_GOLD);

    clock_end_draw(ctx);
  }

  clock_deinit(ctx);

  return 0;
}
