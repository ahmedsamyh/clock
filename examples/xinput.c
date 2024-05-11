#include <clock/clock.h>
#include <xinput.h>

typedef struct Particle {
  Vector2f pos;
  Vector2f size;
  Color color;
  float lifetime;
  float time_since_birth;
} Particle;

Particle Particle_make(Vector2f pos, Vector2f size, Color color) {
  return (Particle) {
    .pos = pos,
    .size = size,
    .color = color,
    .lifetime = 1.f,
    .time_since_birth = 0.f,
  };
}

void Particle_draw(Context* ctx, Particle* p) {
  float t = 1.f - (p->time_since_birth / p->lifetime);
  Color color = color_alpha(p->color, t);

  Vector2f draw_size = {
    .x = t * p->size.x,
    .y = t * p->size.y,
  };

  draw_rect_centered(ctx, (Rect) {p->pos, draw_size}, color);
}

typedef struct R {
  Particle* particles; // dynamic-array
  float accumulator;
  float particle_spawn_rate;

  Vector2f pos;
  float size;

  Color color;

  Context* ctx;
} R;

R R_make(Context* ctx, Color color) {
  return (R) {
    .particles = NULL,
    .accumulator = 0.f,
    .particle_spawn_rate = 0.1f,
    .pos = (Vector2f) {0.f, 0.f},
    .size = 10.f,
    .color = color,
    .ctx = ctx,
  };
}

void R_update(R* r) {
  Context* ctx = r->ctx;
  r->accumulator += ctx->delta;
  if (r->accumulator >= r->particle_spawn_rate) {
    r->accumulator -= r->particle_spawn_rate;
    Particle p = Particle_make(r->pos, (Vector2f) {r->size, r->size}, r->color);
    arrput(r->particles, p);
  }

  for (int i = (int)(arrlenu(r->particles)-1); i >= 0; --i) {
    Particle* p = &r->particles[i];
    p->time_since_birth += ctx->delta;
    if (p->time_since_birth >= p->lifetime) {
      arrdel(r->particles, i);
    }
  }

}

void R_draw(R* r) {
  Context* ctx = r->ctx;
  for (int i = (int)(arrlenu(r->particles)-1); i >= 0; --i) {
    Particle* p = &r->particles[i];
    Particle_draw(ctx, p);
  }

  draw_rect_centered(ctx, (Rect) {r->pos, {r->size, r->size}}, r->color);
}

cstr controller_button_as_str(int button) {
  switch (button) {
  case CONTROLLER_DPAD_UP:		return "DPAD_UP";
  case CONTROLLER_DPAD_DOWN:		return "DPAD_DOWN";
  case CONTROLLER_DPAD_LEFT:		return "DPAD_LEFT";
  case CONTROLLER_DPAD_RIGHT:		return "DPAD_RIGHT";
  case CONTROLLER_BUTTON_START:		return "BUTTON_START";
  case CONTROLLER_BUTTON_SELECT:	return "BUTTON_SELECT";
  case CONTROLLER_BUTTON_L3:		return "BUTTON_L3";
  case CONTROLLER_BUTTON_R3:		return "BUTTON_R3";
  case CONTROLLER_BUTTON_L1:		return "BUTTON_L1";
  case CONTROLLER_BUTTON_R1:		return "BUTTON_R1";
  case CONTROLLER_BUTTON_A:		return "BUTTON_A";
  case CONTROLLER_BUTTON_B:		return "BUTTON_B";
  case CONTROLLER_BUTTON_X:		return "BUTTON_X";
  case CONTROLLER_BUTTON_Y:		return "BUTTON_Y";
  default: {
    ASSERT(0 && "Unreachable!");
    break;
  }
  }
  return "YOU SHOULD NOT SEE THIS!";
}

cstr controller_button_state_str(Controller* ctrler, int button) {
  if (clock_controller_pressed(ctrler, button)) {
    return "Pressed";
  }
  if (clock_controller_held(ctrler, button)) {
    return "Held";
  }
  if (clock_controller_released(ctrler, button)) {
    return "Released";
  }
  return "";
}

void log_controller_state(Controller* ctrler) {
  for (int i = 0; i < CONTROLLER_BUTTON_COUNT; ++i) {
    if (clock_controller_pressed(ctrler, i)) {
      log_info("Controller pressed %s", controller_button_as_str(i));
    }
    if (clock_controller_held(ctrler, i)) {
      log_info("Controller held %s", controller_button_as_str(i));
    }
    if (clock_controller_released(ctrler, i)) {
      log_info("Controller released %s", controller_button_as_str(i));
    }

    if (ctrler->left_trigger > 0.f) {
      log_info("Controller left trigger: %f", ctrler->left_trigger);
    }
    if (ctrler->right_trigger > 0.f) {
      log_info("Controller right trigger: %f", ctrler->right_trigger);
    }
  }
}

#define UI_CONTROLLER_BUTTON(btn)					\
  cstr controller_str_##btn = Arena_alloc_str(str_arena, "%s button: P: %d H: %d R: %d", \
					      #btn,			\
					      clock_controller_pressed(&controller, CONTROLLER_##btn), \
					      clock_controller_held(&controller, CONTROLLER_##btn), \
					      clock_controller_released(&controller, CONTROLLER_##btn)); \
  UI_text(&ui, controller_str_##btn, 24, COLOR_WHITE)


int main(void) {
  Context* ctx = clock_init(1280, 720, 1.f, 1.f, "Xinput", WINDOW_VSYNC);

  if (!ctx) return 1;

  R left =  R_make(ctx, COLOR_GOLD);
  R right = R_make(ctx, COLOR_MAGENTA);
  const float speed = 120.f;

  log_info("Xinput supports %u controllers", XUSER_MAX_COUNT);

  Arena str_arena = Arena_make(0);
  UI ui = UI_make(ctx, &ctx->default_font, (Vector2f) {200.f, 200.f}, "Debug");

  while (!clock_should_quit(ctx)) {
    clock_begin_draw(ctx);
    clock_clear(ctx, COLOR_BLACK);

    Controller controller = {0};
    if (get_controller(&controller, ctx, 0)) {
      UI_begin(&ui, UI_LAYOUT_KIND_VERT);

      Arena_reset(&str_arena);
      cstr controller_thumbL_str = Arena_alloc_str(str_arena, "ThumbL: %f, %f", controller.left_thumb.x, controller.left_thumb.y);
      UI_text(&ui, controller_thumbL_str, 24, COLOR_WHITE);

      cstr controller_thumbR_str = Arena_alloc_str(str_arena, "ThumbR: %f, %f", controller.right_thumb.x, controller.right_thumb.y);
      UI_text(&ui, controller_thumbR_str, 24, COLOR_WHITE);

      UI_CONTROLLER_BUTTON(DPAD_UP);
      UI_CONTROLLER_BUTTON(DPAD_DOWN);
      UI_CONTROLLER_BUTTON(DPAD_LEFT);
      UI_CONTROLLER_BUTTON(DPAD_RIGHT);
      UI_CONTROLLER_BUTTON(BUTTON_START);
      UI_CONTROLLER_BUTTON(BUTTON_SELECT);
      UI_CONTROLLER_BUTTON(BUTTON_L3);
      UI_CONTROLLER_BUTTON(BUTTON_R3);
      UI_CONTROLLER_BUTTON(BUTTON_L1);
      UI_CONTROLLER_BUTTON(BUTTON_R1);
      UI_CONTROLLER_BUTTON(BUTTON_A);
      UI_CONTROLLER_BUTTON(BUTTON_B);
      UI_CONTROLLER_BUTTON(BUTTON_X);
      UI_CONTROLLER_BUTTON(BUTTON_Y);

      cstr controller_left_trig_str = Arena_alloc_str(str_arena, "TriggerL: %f", controller.left_trigger);
      UI_text(&ui, controller_left_trig_str, 24, COLOR_WHITE);
      cstr controller_right_trig_str = Arena_alloc_str(str_arena, "TriggerR: %f", controller.right_trigger);
      UI_text(&ui, controller_right_trig_str, 24, COLOR_WHITE);

      UI_end(&ui);

      left.pos = v2f_add(left.pos, v2f_muls(v2f_muls(controller.left_thumb, speed), ctx->delta));
      right.pos = v2f_add(right.pos, v2f_muls(v2f_muls(controller.right_thumb, speed), ctx->delta));

      log_controller_state(&controller);

      if (clock_controller_held(&controller, CONTROLLER_BUTTON_A)) {
	left.size += 10.f * ctx->delta;
      }

      if (clock_controller_held(&controller, CONTROLLER_BUTTON_B)) {
	left.size -= 10.f * ctx->delta;
      }

      if (clock_controller_held(&controller, CONTROLLER_BUTTON_X)) {
	right.size += 10.f * ctx->delta;
      }

      if (clock_controller_held(&controller, CONTROLLER_BUTTON_Y)) {
	right.size -= 10.f * ctx->delta;
      }
    }

    R_update(&left);
    R_update(&right);

    R_draw(&left);
    R_draw(&right);

    clock_end_draw(ctx);
  }

  Arena_free(&str_arena);
  UI_free(&ui);
  clock_deinit(ctx);

  return 0;
}
