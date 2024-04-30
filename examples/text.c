#include <clock/clock.h>
#include <assert.h>

inline cstr get_text_align_str(const Text_align t) {
  switch (t) {
  case TEXT_ALIGN_TOP_LEFT: {
    return "top left";
  } break;
  case TEXT_ALIGN_TOP_CENTER: {
    return "top center";
  } break;
  case TEXT_ALIGN_TOP_RIGHT: {
    return "top right";
  } break;
  case TEXT_ALIGN_CENTER_LEFT: {
    return "center left";
  } break;
  case TEXT_ALIGN_CENTER_CENTER: {
    return "center center";
  } break;
  case TEXT_ALIGN_CENTER_RIGHT: {
    return "center right";
  } break;
  case TEXT_ALIGN_BOTTOM_LEFT: {
    return "bottom left";
  } break;
  case TEXT_ALIGN_BOTTOM_CENTER: {
    return "bottom center";
  } break;
  case TEXT_ALIGN_BOTTOM_RIGHT: {
    return "bottom right";
  } break;
  default: ASSERT(0 && "Unreachable!");
  }
  return "YOU WILL DIE IF YOU SEE THIS!";
}

static void camera_movement(Context* ctx, float camera_speed) {
  // move camera
  if (clock_key_held(ctx, KEY_A)) {
    ctx->camera.x -= (camera_speed * (clock_key_held(ctx, KEY_LEFT_SHIFT) ? 2.f : 1.f)) * ctx->delta;
  }
  if (clock_key_held(ctx, KEY_D)) {
    ctx->camera.x += (camera_speed * (clock_key_held(ctx, KEY_LEFT_SHIFT) ? 2.f : 1.f)) * ctx->delta;
  }
  if (clock_key_held(ctx, KEY_W)) {
    ctx->camera.y -= (camera_speed * (clock_key_held(ctx, KEY_LEFT_SHIFT) ? 2.f : 1.f)) * ctx->delta;
  }
  if (clock_key_held(ctx, KEY_S)) {
    ctx->camera.y += (camera_speed * (clock_key_held(ctx, KEY_LEFT_SHIFT) ? 2.f : 1.f)) * ctx->delta;
  }
  if (clock_key_held(ctx, KEY_E)) {
    ctx->camera.z -= (camera_speed * (clock_key_held(ctx, KEY_LEFT_SHIFT) ? 2.f : 1.f)) * ctx->delta;
  }
  if (clock_key_held(ctx, KEY_Q)) {
    ctx->camera.z += (camera_speed * (clock_key_held(ctx, KEY_LEFT_SHIFT) ? 2.f : 1.f)) * ctx->delta;
  }

  // reset camera
  if (clock_key_pressed(ctx, KEY_R)) {
    ctx->camera.x = 0.f;
    ctx->camera.y = 0.f;
  }
}

int main(void) {
  Context* ctx = clock_init(1280, 720, 1.f, 1.f, "Text", WINDOW_VSYNC);
  if (ctx == NULL) {
    return 1;
  }
  cstr text = "The quick brown fox jumps over the lazy dog";

  int char_size = 32;

  Text_align align = TEXT_ALIGN_TOP_LEFT;

  /* const float camera_speed = 100.f; */
  float padding = 4.f;

  while (!clock_should_quit(ctx)) {
    clock_begin_draw(ctx);
    clock_clear(ctx, color_from_hex(0xFF141414));

    if (clock_key_pressed(ctx, KEY_E)) {
      char_size += 1;
    }
    if (clock_key_pressed(ctx, KEY_Q)) {
      char_size -= 1;
      if (char_size <= 2) char_size = 2;
    }

    if (clock_key_pressed(ctx, KEY_SPACE)) {
      align = (align + 1) % (TEXT_ALIGN_COUNT);
    }

    draw_text(ctx, &ctx->default_font, get_text_align_str(align), (Vector2f) {padding, padding}, 24, COLOR_WHITE);

    draw_text_aligned(ctx, &ctx->default_font, text, clock_mpos_screen(ctx), char_size, COLOR_WHITE, align);
    /* draw_box(ctx, (Rect) {clock_mpos_screen(ctx), get_text_size(ctx, &ctx->default_font, text, char_size)}, COLOR_WHITE, color_alpha(COLOR_GREEN, 0.2f)); */

    clock_end_draw(ctx);
  }

  clock_deinit(ctx);

  return 0;
}
