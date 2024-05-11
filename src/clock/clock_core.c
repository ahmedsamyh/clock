#include <clock/clock_core.h>
#include <stdlib.h>
#define MATRIX_IMPLEMENTATION
#include <clock/clock_matrix.h>
#define COMMONLIB_IMPLEMENTATION
#include <commonlib.h>
#include <assert.h>
#include <string.h>
#include "default_font.c"

//
// Private functions to this file
//

static void set_view_matrix(Context* ctx) {
  gl(GLuint m = glGetUniformLocation(ctx->ren->current_shader, "view"));
  Matrix4 translate = Mat4_identity();
  if (ctx->use_camera_view) {
    // TODO: Do we have to flip the y-axis of the camera here?
    Vector3f camera_flipped = {
      -ctx->camera.x,
      -ctx->camera.y,
      -ctx->camera.z,
    };
    translate = Mat4_translate(Mat4_identity(), camera_flipped);
  }
  gl(glUniformMatrix4fv(m, 1, GL_TRUE, &translate.m[0][0]));
}

static void set_matrices(Context* ctx) {
  Renderer* r = ctx->ren;
  {
    gl(GLuint m = glGetUniformLocation(r->current_shader, "model"));
    Matrix4 identity = Mat4_identity();
    gl(glUniformMatrix4fv(m, 1, GL_TRUE, &identity.m[0][0]));
  }
  set_view_matrix(ctx);
  {
    gl(GLuint m = glGetUniformLocation(r->current_shader, "proj"));
    gl(glUniformMatrix4fv(m, 1, GL_TRUE, &ctx->ren->proj.m[0][0]));
  }
}

// Window

bool Window_init(Window* win, unsigned int width, unsigned int height, Vector2f scale, const char* title, uint32 flags) {
  win->title = title;

  win->width = width == 0 ? DEFAULT_WIN_WIDTH : width;
  win->height = height == 0 ? DEFAULT_WIN_HEIGHT : height;
  win->scale_x = scale.x <= 0 ? 1.f : scale.x;
  win->scale_y = scale.y <= 0 ? 1.f : scale.y;

  log_f(LOG_INFO, "Running '%s'", win->title);
  if (!glfwInit()) {
    log_f(LOG_ERROR, "GLFW Could not initialize!");
    return false;
  }
  log_f(LOG_INFO, "GLFW initialized!");

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // NOTE: We don't care about the aspect ratio if we can't resize the window, duh
  if (flags & WINDOW_RESIZABLE_ASPECT) flags |= WINDOW_RESIZABLE;
  bool resizable = (bool)(flags & WINDOW_RESIZABLE);
  glfwWindowHint(GLFW_RESIZABLE, (resizable ? GLFW_TRUE : GLFW_FALSE));

  win->glfw_win = glfwCreateWindow(win->width, win->height, win->title, NULL, NULL);

  glfwSetWindowAspectRatio(win->glfw_win, (flags & WINDOW_RESIZABLE_ASPECT ? width : GLFW_DONT_CARE), (flags & WINDOW_RESIZABLE_ASPECT ? height : GLFW_DONT_CARE));

  if (win->glfw_win == NULL) {
    char* tmpbuff = (char*)malloc(sizeof(char)*1024);
    glfwGetError((const char**)&tmpbuff);
    log_f(LOG_ERROR, "Could not initialize window: %s", tmpbuff);
    free(tmpbuff);
    return false;
  }
  log_f(LOG_INFO, "Window Created!");
  glfwMakeContextCurrent(win->glfw_win);

  set_vsync((bool)(flags & WINDOW_VSYNC));

  int version = gladLoadGL(glfwGetProcAddress);

  if (version == 0) {
    log_f(LOG_ERROR, "Failed to initialize GLAD Context!");
    return false;
  }

  log_f(LOG_INFO, "Loaded OpenGL %d.%d!", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

  return true;
}


void Window_deinit(Window* win) {
  glfwDestroyWindow(win->glfw_win);
  log_f(LOG_INFO, "Window Destroyed!");
  glfwTerminate();
  log_f(LOG_INFO, "GLFW Terminated!");
}

// Context / main user api

Context* clock_init(unsigned int window_width, unsigned int window_height, float window_scale_x, float window_scale_y, const char* title, uint32 flags) {
  Context* ctx = (Context*)calloc(1, sizeof(Context));
  ctx->win = (Window*)  malloc(sizeof(Window));
  ctx->ren = (Renderer*)malloc(sizeof(Renderer));

  if (!Window_init(ctx->win, window_width, window_height, (Vector2f) {window_scale_x, window_scale_y}, title, flags)) {
    // TODO: free ctx
    return NULL;
  }

  if (!Renderer_init(ctx->ren, ctx->win, flags)) {
    // TODO: free ctx
    return NULL;
  }

  ctx->resman = (Resource_manager*)calloc(1, sizeof(Resource_manager));

  glfwSetWindowUserPointer(ctx->win->glfw_win, ctx);
  glfwSetKeyCallback(ctx->win->glfw_win, key_callback);
  glfwSetCharCallback(ctx->win->glfw_win, text_callback);
  glfwSetScrollCallback(ctx->win->glfw_win, mouse_scroll_callback);
  glfwSetWindowSizeCallback(ctx->win->glfw_win, window_resize_callback);
  glfwSetFramebufferSizeCallback(ctx->win->glfw_win, framebuffer_resize_callback);
  glfwSetErrorCallback(error_callback);

  ctx->tp1 = glfwGetTime();
  ctx->tp2 = 0.0;
  ctx->delta = 0.0;
  ctx->fps = 0;
  ctx->mpos = (Vector2f){0};

  int max_tex_image_slots;
  gl(glGetIntegerv(GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS, &max_tex_image_slots));
  ctx->ren_tex_image_slot = max_tex_image_slots - 1;

  // enable blending
  gl(glEnable(GL_BLEND));

  set_blend_mode(BLENDMODE_ALPHA);

  // TODO: Why are we using camera view by default???
  clock_use_camera_view(ctx, true);

  // load default font
  if (!Font_init_from_memory(&ctx->default_font, ctx, romulus_bytes, "[DEFAULT_FONT] romulus.ttf")) return NULL;

  clock_init_keys(ctx);

  return ctx;
}

bool clock_should_quit(Context* ctx) {
  return glfwWindowShouldClose(ctx->win->glfw_win);
}

void clock_update_mouse(Context* ctx) {
  Window* win = ctx->win;
  Mouse* m = ctx->m;
  double mx, my;
  glfwGetCursorPos(win->glfw_win, &mx, &my);

  // TODO: fix y position being messed up when scaling (x is fine)
  ctx->prev_mpos = ctx->mpos;
  ctx->mpos.x = (float)mx / win->scale_x;
  ctx->mpos.y = (float)my / win->scale_y;

  for (size_t i = 0; i < MOUSE_BUTTONS_COUNT; ++i) {
    m[i].just_pressed = false;
    m[i].pressed = false;
    m[i].released = false;
  }

  for (int i = 0; i < MOUSE_BUTTONS_COUNT; ++i) {
    m[i].prev_state = m[i].held;
    int state = glfwGetMouseButton(win->glfw_win, i + GLFW_MOUSE_BUTTON_LEFT);
    if (state == GLFW_PRESS) {
      m[i].held = true;
    } else if (state == GLFW_RELEASE) {
      m[i].held = false;
    }
  }

  for (int i = 0; i < MOUSE_BUTTONS_COUNT; ++i) {
    if (!m[i].prev_state && m[i].held) {
      m[i].just_pressed = true;
      m[i].pressed = true;
    }
    if (m[i].prev_state && !m[i].held) {
      m[i].released = true;
    }
  }
}

void clock_begin_draw(Context* ctx) {
  Window* win = ctx->win;

  clock_update_mouse(ctx);
  clock_update_controllers(ctx);

  ctx->tp2 = glfwGetTime();
  ctx->delta = (float32)(ctx->tp2 - ctx->tp1);
  ctx->tp1 = ctx->tp2;

  ctx->fps = (int)(1.0f / ctx->delta);

  snprintf(ctx->tmpbuff, TMP_BUFF_SIZE, "%s | %dfps | %fs", win->title, ctx->fps, ctx->delta);

  glfwSetWindowTitle(win->glfw_win, ctx->tmpbuff);

  gl(glBindFramebuffer(GL_FRAMEBUFFER, ctx->ren->ren_tex->fbo));
}

void clock_flush_draw(Context *ctx) {
  Render_target* rt = ctx->ren->ren_tex;
  Window* win = ctx->win;
  gl(glBindFramebuffer(GL_READ_FRAMEBUFFER, ctx->ren->ren_tex->fbo));
  gl(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
  gl(glBlitFramebuffer(0, 0, rt->width, rt->height,
		       0, 0, win->width, win->height,
		       GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
		       GL_NEAREST));
}

void clock_end_draw(Context* ctx) {
  clock_flush_draw(ctx);
  clock_update_keys(ctx);

  ctx->text_entered = false;
  ctx->key_input_handled = false;
  ctx->mouse_input_handled = false;
  ctx->mscroll.x = 0.f;
  ctx->mscroll.y = 0.f;

  gl(glBindFramebuffer(GL_FRAMEBUFFER, 0));
  glfwSwapBuffers(ctx->win->glfw_win);
  glfwPollEvents();
}

void clock_clear(Context* ctx, Color color) {

  // clear the main fbo
  gl(glBindFramebuffer(GL_FRAMEBUFFER, 0));
  gl(glClearColor(color.r, color.g, color.b, color.a));
  gl(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

  // clear the ren tex's fbo
  gl(glBindFramebuffer(GL_FRAMEBUFFER, ctx->ren->ren_tex->fbo));
  gl(glClearColor(color.r, color.g, color.b, color.a));
  gl(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void clock_deinit(Context* ctx) {
  Font_deinit(&ctx->default_font);
  Renderer_deinit(ctx->ren);
  Window_deinit(ctx->win);
  Resman_unload_every_texture(ctx->resman);
  free(ctx->win);
  free(ctx->ren);
  free(ctx->resman);
}

Vector2f clock_mpos_world(Context* ctx) {
  ASSERT(ctx->use_camera_view);
  return (Vector2f) {
    .x = ctx->mpos.x - ctx->camera.x,
    .y = ctx->mpos.y - ctx->camera.y,
  };
}

Vector2f clock_mpos_screen(Context* ctx) {
  return ctx->mpos;
}

void clock_use_camera_view(Context* ctx, bool use) {
  ctx->use_camera_view = use;
}

// TODO: Maybe use a matrix multiplication in converting from space to space
Vector3f clock_screen_to_world_3d(Context* ctx, Vector3f pos) {
  return (Vector3f) {
    .x = pos.x + ctx->camera.x,
    .y = pos.y + ctx->camera.y,
    .z = pos.z + ctx->camera.z,
  };
}

Vector2f clock_screen_to_world(Context* ctx, Vector2f pos) {
  return (Vector2f) {
    .x = pos.x + ctx->camera.x,
    .y = pos.y + ctx->camera.y,
  };
}

Vector3f clock_world_to_screen_3d(Context* ctx, Vector3f pos) {
  return (Vector3f) {
    .x = pos.x - ctx->camera.x,
    .y = pos.y - ctx->camera.y,
    .z = pos.z - ctx->camera.z,
  };
}

Vector2f clock_world_to_screen(Context* ctx, Vector2f pos) {
  return (Vector2f) {
    .x = pos.x - ctx->camera.x,
    .y = pos.y - ctx->camera.y,
  };
}

void clock_begin_scissor(Context* ctx, Rect rect) {
  gl(glEnable(GL_SCISSOR_TEST));
  gl(glScissor((int)rect.pos.x,
	       (int)ctx->win->height - (int)rect.pos.y - (int)rect.size.y,
	       (int)rect.size.x, (int)rect.size.y));
}

void clock_end_scissor(Context* ctx) {
  gl(glDisable(GL_SCISSOR_TEST));
  clock_flush_draw(ctx);
  gl(glBindFramebuffer(GL_FRAMEBUFFER, ctx->ren->ren_tex->fbo));
}

// Input

// TODO: key pressed are ignored when moving the window

static void add_key_to_key_map(Context* ctx, int code) {
  Key k = {0};
  k.code = code;
  hmput(ctx->key_map, code, k);
  arrput(ctx->key_codes, code);
}

void clock_init_keys(Context* ctx) {
  add_key_to_key_map(ctx, KEY_SPACE);
  add_key_to_key_map(ctx, KEY_APOSTROPHE);
  add_key_to_key_map(ctx, KEY_COMMA);
  add_key_to_key_map(ctx, KEY_MINUS);
  add_key_to_key_map(ctx, KEY_PERIOD);
  add_key_to_key_map(ctx, KEY_SLASH);
  add_key_to_key_map(ctx, KEY_0);
  add_key_to_key_map(ctx, KEY_1);
  add_key_to_key_map(ctx, KEY_2);
  add_key_to_key_map(ctx, KEY_3);
  add_key_to_key_map(ctx, KEY_4);
  add_key_to_key_map(ctx, KEY_5);
  add_key_to_key_map(ctx, KEY_6);
  add_key_to_key_map(ctx, KEY_7);
  add_key_to_key_map(ctx, KEY_8);
  add_key_to_key_map(ctx, KEY_9);
  add_key_to_key_map(ctx, KEY_SEMICOLON);
  add_key_to_key_map(ctx, KEY_EQUAL);
  add_key_to_key_map(ctx, KEY_A);
  add_key_to_key_map(ctx, KEY_B);
  add_key_to_key_map(ctx, KEY_C);
  add_key_to_key_map(ctx, KEY_D);
  add_key_to_key_map(ctx, KEY_E);
  add_key_to_key_map(ctx, KEY_F);
  add_key_to_key_map(ctx, KEY_G);
  add_key_to_key_map(ctx, KEY_H);
  add_key_to_key_map(ctx, KEY_I);
  add_key_to_key_map(ctx, KEY_J);
  add_key_to_key_map(ctx, KEY_K);
  add_key_to_key_map(ctx, KEY_L);
  add_key_to_key_map(ctx, KEY_M);
  add_key_to_key_map(ctx, KEY_N);
  add_key_to_key_map(ctx, KEY_O);
  add_key_to_key_map(ctx, KEY_P);
  add_key_to_key_map(ctx, KEY_Q);
  add_key_to_key_map(ctx, KEY_R);
  add_key_to_key_map(ctx, KEY_S);
  add_key_to_key_map(ctx, KEY_T);
  add_key_to_key_map(ctx, KEY_U);
  add_key_to_key_map(ctx, KEY_V);
  add_key_to_key_map(ctx, KEY_W);
  add_key_to_key_map(ctx, KEY_X);
  add_key_to_key_map(ctx, KEY_Y);
  add_key_to_key_map(ctx, KEY_Z);
  add_key_to_key_map(ctx, KEY_LEFT_BRACKET);
  add_key_to_key_map(ctx, KEY_BACKSLASH);
  add_key_to_key_map(ctx, KEY_RIGHT_BRACKET);
  add_key_to_key_map(ctx, KEY_GRAVE_ACCENT);
  add_key_to_key_map(ctx, KEY_WORLD_1);
  add_key_to_key_map(ctx, KEY_WORLD_2);
  add_key_to_key_map(ctx, KEY_ESCAPE);
  add_key_to_key_map(ctx, KEY_ENTER);
  add_key_to_key_map(ctx, KEY_TAB);
  add_key_to_key_map(ctx, KEY_BACKSPACE);
  add_key_to_key_map(ctx, KEY_INSERT);
  add_key_to_key_map(ctx, KEY_DELETE);
  add_key_to_key_map(ctx, KEY_RIGHT);
  add_key_to_key_map(ctx, KEY_LEFT);
  add_key_to_key_map(ctx, KEY_DOWN);
  add_key_to_key_map(ctx, KEY_UP);
  add_key_to_key_map(ctx, KEY_PAGE_UP);
  add_key_to_key_map(ctx, KEY_PAGE_DOWN);
  add_key_to_key_map(ctx, KEY_HOME);
  add_key_to_key_map(ctx, KEY_END);
  add_key_to_key_map(ctx, KEY_CAPS_LOCK);
  add_key_to_key_map(ctx, KEY_SCROLL_LOCK);
  add_key_to_key_map(ctx, KEY_NUM_LOCK);
  add_key_to_key_map(ctx, KEY_PRINT_SCREEN);
  add_key_to_key_map(ctx, KEY_PAUSE);
  add_key_to_key_map(ctx, KEY_F1);
  add_key_to_key_map(ctx, KEY_F2);
  add_key_to_key_map(ctx, KEY_F3);
  add_key_to_key_map(ctx, KEY_F4);
  add_key_to_key_map(ctx, KEY_F5);
  add_key_to_key_map(ctx, KEY_F6);
  add_key_to_key_map(ctx, KEY_F7);
  add_key_to_key_map(ctx, KEY_F8);
  add_key_to_key_map(ctx, KEY_F9);
  add_key_to_key_map(ctx, KEY_F10);
  add_key_to_key_map(ctx, KEY_F11);
  add_key_to_key_map(ctx, KEY_F12);
  add_key_to_key_map(ctx, KEY_F13);
  add_key_to_key_map(ctx, KEY_F14);
  add_key_to_key_map(ctx, KEY_F15);
  add_key_to_key_map(ctx, KEY_F16);
  add_key_to_key_map(ctx, KEY_F17);
  add_key_to_key_map(ctx, KEY_F18);
  add_key_to_key_map(ctx, KEY_F19);
  add_key_to_key_map(ctx, KEY_F20);
  add_key_to_key_map(ctx, KEY_F21);
  add_key_to_key_map(ctx, KEY_F22);
  add_key_to_key_map(ctx, KEY_F23);
  add_key_to_key_map(ctx, KEY_F24);
  add_key_to_key_map(ctx, KEY_F25);
  add_key_to_key_map(ctx, KEY_KP_0);
  add_key_to_key_map(ctx, KEY_KP_1);
  add_key_to_key_map(ctx, KEY_KP_2);
  add_key_to_key_map(ctx, KEY_KP_3);
  add_key_to_key_map(ctx, KEY_KP_4);
  add_key_to_key_map(ctx, KEY_KP_5);
  add_key_to_key_map(ctx, KEY_KP_6);
  add_key_to_key_map(ctx, KEY_KP_7);
  add_key_to_key_map(ctx, KEY_KP_8);
  add_key_to_key_map(ctx, KEY_KP_9);
  add_key_to_key_map(ctx, KEY_KP_DECIMAL);
  add_key_to_key_map(ctx, KEY_KP_DIVIDE);
  add_key_to_key_map(ctx, KEY_KP_MULTIPLY);
  add_key_to_key_map(ctx, KEY_KP_SUBTRACT);
  add_key_to_key_map(ctx, KEY_KP_ADD);
  add_key_to_key_map(ctx, KEY_KP_ENTER);
  add_key_to_key_map(ctx, KEY_KP_EQUAL);
  add_key_to_key_map(ctx, KEY_LEFT_SHIFT);
  add_key_to_key_map(ctx, KEY_LEFT_CONTROL);
  add_key_to_key_map(ctx, KEY_LEFT_ALT);
  add_key_to_key_map(ctx, KEY_LEFT_SUPER);
  add_key_to_key_map(ctx, KEY_RIGHT_SHIFT);
  add_key_to_key_map(ctx, KEY_RIGHT_CONTROL);
  add_key_to_key_map(ctx, KEY_RIGHT_ALT);
  add_key_to_key_map(ctx, KEY_RIGHT_SUPER);
  add_key_to_key_map(ctx, KEY_MENU);
  add_key_to_key_map(ctx, KEY_LAST);
}

void clock_update_keys(Context* ctx) {
  // Clear Pressed/Released/Just_pressed state
  for (size_t i = 0; i < arrlenu(ctx->key_codes); ++i) {
    int code = ctx->key_codes[i];
    Key_KV* k_kv = hmgetp_null(ctx->key_map, code);
    ASSERT(k_kv != NULL);

    k_kv->value.pressed = false;
    k_kv->value.released = false;
    k_kv->value.just_pressed = false;
  }

  // Get key state for this frame
  // ascii
  for (size_t i = 0; i < arrlenu(ctx->key_codes); ++i) {
    int code = ctx->key_codes[i];
    Key_KV* k_kv = hmgetp_null(ctx->key_map, code);
    ASSERT(k_kv != NULL);

    k_kv->value.prev_state = k_kv->value.held;
    int state = glfwGetKey(ctx->win->glfw_win, code);
    if (state == GLFW_PRESS) {
      k_kv->value.held = true;
    } else if (state == GLFW_RELEASE) {
      k_kv->value.held = false;
    }
  }

  // Set Pressed/Released/Just_pressed state
  // ascii
  for (size_t i = 0; i < arrlenu(ctx->key_codes); ++i) {
    int code = ctx->key_codes[i];
    Key_KV* k_kv = hmgetp_null(ctx->key_map, code);
    ASSERT(k_kv != NULL);

    if (!k_kv->value.prev_state && k_kv->value.held) {
      k_kv->value.just_pressed = true;
      k_kv->value.pressed = true;
    }
    if (k_kv->value.prev_state && !k_kv->value.held) {
      k_kv->value.released = true;
    }
  }
}

void clock_eat_key_input(Context* ctx) {
  ctx->key_input_handled = true;
}

void clock_eat_mouse_input(Context* ctx) {
  ctx->mouse_input_handled = true;
}

void clock_eat_input(Context* ctx) {
  clock_eat_key_input(ctx);
  clock_eat_mouse_input(ctx);
}

void clock_update_controllers(Context* ctx) {
  for (int i = 0; i < XUSER_MAX_COUNT; ++i) {
    XINPUT_STATE state = {0};

    DWORD result = XInputGetState(i, &state);
    if (result == ERROR_SUCCESS) {
      // ith controller is connected
      setup_controller(&ctx->controllers[i], state);
    } else {
      // ith controller is not connected
    }
  }
}

bool get_controller(Controller* ctrler, Context* ctx, int idx) {
  if (idx < 0 || idx >= XUSER_MAX_COUNT) {
    return false;
  }

  *ctrler = ctx->controllers[idx];

  return true;
}

bool clock_key_state(Context* ctx, int key, Key_state state) {
  if (ctx->key_input_handled) return false;
  Key_KV* k_kv = hmgetp(ctx->key_map, key);
  ASSERT(k_kv != NULL);

  switch (state) {
  case KEY_STATE_PRESSED: {
    return k_kv->value.pressed;
  } break;
  case KEY_STATE_JUST_PRESSED: {
    return k_kv->value.just_pressed;
  } break;
  case KEY_STATE_RELEASED: {
    return k_kv->value.released;
  } break;
  case KEY_STATE_HELD: {
    return k_kv->value.held;
  } break;
  default: ASSERT(0 && "Unreachable");
  }
  return false;
}

bool clock_key_pressed(Context* ctx, int key) {
  return clock_key_state(ctx, key, KEY_PRESSED);
}

bool clock_key_just_pressed(Context* ctx, int key) {
  return clock_key_state(ctx, key, KEY_JUST_PRESSED);
}

bool clock_key_released(Context* ctx, int key) {
  return clock_key_state(ctx, key, KEY_RELEASED);
}

bool clock_key_held(Context* ctx, int key) {
  return clock_key_state(ctx, key, KEY_HELD);
}

/* Key clock_get_key_by_code(Context* ctx, int key) { */

/* } */

bool clock_mouse_state(Context* ctx, int button, Mouse_state state) {
  if (ctx->mouse_input_handled) return false;
  switch (state) {
  case MOUSE_STATE_PRESSED: {
    return ctx->m[button].pressed;
  } break;
  case MOUSE_STATE_RELEASED: {
    return ctx->m[button].released;
  } break;
  case MOUSE_STATE_HELD: {
    return ctx->m[button].held;
  } break;
  default: ASSERT(0 && "Unreachable");
  }
  return false;
}

bool clock_mouse_pressed(Context* ctx, int button) {
  return clock_mouse_state(ctx, button, MOUSE_STATE_PRESSED);
}

bool clock_mouse_released(Context* ctx, int button) {
  return clock_mouse_state(ctx, button, MOUSE_STATE_RELEASED);
}

bool clock_mouse_held(Context* ctx, int button) {
  return clock_mouse_state(ctx, button, MOUSE_STATE_HELD);
}

bool clock_controller_state(Controller* ctrler, int button, Controller_state state) {
  switch (state) {
  case CONTROLLER_STATE_PRESSED: {
    return ctrler->buttons[button].pressed;
  } break;
  case CONTROLLER_STATE_HELD: {
    return ctrler->buttons[button].held;
  } break;
  case CONTROLLER_STATE_RELEASED: {
    return ctrler->buttons[button].released;
  } break;
  default: ASSERT(0 && "Unreachable!");
  }

  return false;
}

bool clock_controller_held(Controller* ctrler, int button) {
  return clock_controller_state(ctrler, button, CONTROLLER_STATE_HELD);
}

bool clock_controller_pressed(Controller* ctrler, int button) {
  return clock_controller_state(ctrler, button, CONTROLLER_STATE_PRESSED);
}

bool clock_controller_released(Controller* ctrler, int button) {
  return clock_controller_state(ctrler, button, CONTROLLER_STATE_RELEASED);
}

//
// Misc (Context agnostic)
//

void set_vsync(bool enable) {
  glfwSwapInterval(enable ? 1 : 0);
}

cstr get_clipboard(void) {
  return glfwGetClipboardString(NULL);
}

void set_clipboard(cstr text) {
  glfwSetClipboardString(NULL, text);
}

// Callbacks

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  (void)mods;
  (void)scancode;
  Context* ctx = (Context*)glfwGetWindowUserPointer(window);
  Key_KV* keys = ctx->key_map;

  if (action == GLFW_REPEAT) {
    /* log_f(LOG_INFO, "Key: %d repeat", key); */
    Key_KV* k_kv = hmgetp_null(keys, key);
    ASSERT(k_kv != NULL);
    k_kv->value.pressed = true;
  }
}

void text_callback(GLFWwindow* window, uint32 key_code) {
  Context* ctx = (Context*)glfwGetWindowUserPointer(window);
  ctx->last_entered_character = key_code;
  ctx->text_entered = true;
}

void mouse_scroll_callback(GLFWwindow* window, float64 xoffset, float64 yoffset) {
  Context* ctx = (Context*)glfwGetWindowUserPointer(window);
  ctx->mscroll.x = (float32)xoffset;
  ctx->mscroll.y = (float32)yoffset;
}

void window_resize_callback(GLFWwindow* window, int width, int height) {
  Context* ctx = (Context*)glfwGetWindowUserPointer(window);

  if ((width <= 0) || (height <= 0)) return;

  ctx->win->width = width;
  ctx->win->height = height;

  Rentar_deinit(ctx->ren->ren_tex);
  ASSERT(Rentar_init(ctx->ren->ren_tex, ctx->win, (uint)(ctx->win->width / ctx->win->scale_x), (uint)(ctx->win->height / ctx->win->scale_y)));
  Vector2f screen_size = {(float32)ctx->ren->win->width, (float32)ctx->ren->win->height};

  if (ctx->ren->render_3D) {
    ctx->ren->proj = Mat4_screen_to_clip_projection_perspective(90.f, (float)ctx->ren->win->width/(float)ctx->ren->win->height, 1.f, 1000.f);
  } else {
    ctx->ren->proj = Mat4_screen_to_clip_projection_orthographic(screen_size);
  }

  glViewport(0, 0, width, height);
}

void framebuffer_resize_callback(GLFWwindow* window, int width, int height) {
  (void)window;
  (void)width;
  (void)height;
}

void error_callback(int error_code, cstr description) {
  log_f(LOG_ERROR, "GLFW: [%d] %s", error_code, description);
}

// Renderer

bool Renderer_init(Renderer* r, Window* win, uint32 flags) {
  r->win = win;

  gl(glGenVertexArrays(1, &r->vao););
  gl(glGenBuffers(1, &r->vbo););

  gl(glBindVertexArray(r->vao););  // 0 is the default one

  gl(glBindBuffer(GL_ARRAY_BUFFER, r->vbo);); // 0 is the default one
  gl(glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * VERTEX_CAP, NULL, GL_STATIC_DRAW));;

  // position
  gl(glEnableVertexAttribArray(0));
  gl(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, position)));

  // color
  gl(glEnableVertexAttribArray(1));
  gl(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, color)));

  // texcoord
  gl(glEnableVertexAttribArray(2));
  gl(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, texcoord)));

  // create shaders
  r->texture_shader = create_shader(tex_vert_shader, tex_frag_shader);
  r->color_shader   = create_shader(color_vert_shader, color_frag_shader);
  r->custom_shader  = 0;
  r->current_shader = r->texture_shader;

  r->ren_tex = (Render_target*)calloc(1, sizeof(Render_target));

  if (!Rentar_init(r->ren_tex, win, (uint)(win->width / win->scale_x), (uint)(win->height / win->scale_y))) {
    free(r->ren_tex);
    return false;
  }

  Vector2f screen_size = {(float32)r->win->width, (float32)r->win->height};
  r->render_3D = (bool)flags & RENDER_3D;

  if (r->render_3D) {
    r->proj = Mat4_screen_to_clip_projection_perspective(90.f, (float)r->win->width/(float)r->win->height, 1.f, 1000.f);
  } else {
    r->proj = Mat4_screen_to_clip_projection_orthographic(screen_size);
  }

  return true;
}

void Renderer_deinit(Renderer* r) {
  gl(glUseProgram(0));
  gl(glDeleteProgram(r->texture_shader));
  gl(glDeleteProgram(r->color_shader));
  if (r->custom_shader != 0)
    gl(glDeleteProgram(r->custom_shader));
  gl(glDisableVertexAttribArray(0));
  gl(glDisableVertexAttribArray(1));
  gl(glDisableVertexAttribArray(2));
  gl(glDeleteBuffers(1, &r->vbo));
  gl(glDeleteVertexArrays(1, &r->vao));
  Rentar_deinit(r->ren_tex);
  free(r->ren_tex);
}

void Renderer_use_texture_shader(Renderer* r) {
  gl(glUseProgram(r->texture_shader));
  r->current_shader = r->texture_shader;
}

void Renderer_use_color_shader(Renderer* r) {
  gl(glUseProgram(r->color_shader));
  r->current_shader = r->color_shader;
}

void Renderer_use_custom_shader(Renderer* r, const char* vs, const char* fs) {
  if (r->custom_shader != 0) {
    gl(glDeleteProgram(r->custom_shader));
  }
  r->custom_shader = create_shader(vs, fs);
  gl(glUseProgram(r->custom_shader));
  r->current_shader = r->custom_shader;
}

void Renderer_set_render_target(Renderer* r, GLuint target) {
  (void)r;
  gl(glBindFramebuffer(GL_FRAMEBUFFER, target));
}

void draw_imm_triangle_3d(Context* ctx, Vector3f p0, Vector3f p1, Vector3f p2, Color c0, Color c1, Color c2) {
  Renderer* r = ctx->ren;

  Vector3f positions[] = {
    p0, p1, p2
  };

  Vector4f colors[] = {
    c0, c1, c2
  };

  /* Vector2f screen_size = (Vector2f){(float32)ctx->win->width, (float32)ctx->win->height}; */

  for (size_t i = 0; i < 3; ++i) {
    Vector3f p  = positions[i];
    Vector4f pn = (Vector4f) {
      .x = p.x,
      .y = p.y,
      .z = p.z,
      .w = 1.f,
    };
    Vector4f c = colors[i];
    r->vertices[i].position = pn;
    r->vertices[i].color = c;
  }
  Renderer_use_color_shader(r);
  set_matrices(ctx);

  gl(glBindBuffer(GL_ARRAY_BUFFER, r->vbo));
  gl(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * 3, r->vertices));

  gl(glDrawArrays(GL_TRIANGLES, 0, 3));
}

void draw_imm_triangle(Context* ctx, Vector2f p0, Vector2f p1, Vector2f p2, Color c0, Color c1, Color c2) {
  draw_imm_triangle_3d(ctx, (Vector3f){p0.x, p0.y, 0.f}, (Vector3f){p1.x, p1.y, 0.f}, (Vector3f){p2.x, p2.y, 0.f}, c0, c1, c2);
}

#define IMM_QUAD_BODY(draw_type)					\
  Vector3f positions[] = {p0, p1, p2, p3};				\
  Vector4f colors[] = {c0, c1, c2, c3};					\
  for (size_t i = 0; i < 4; ++i){					\
    Vector3f p    = positions[i];					\
    Vector4f pn = (Vector4f) {.x = p.x,					\
			      .y = p.y,					\
			      .z = p.z,					\
			      .w = 1.f,					\
    };									\
    Vector4f c = colors[i];						\
    r->vertices[i].position = pn;					\
    r->vertices[i].color = c;						\
  }									\
  Renderer_use_color_shader(r);						\
  set_matrices(ctx);							\
  gl(gl(glBindBuffer(GL_ARRAY_BUFFER, r->vbo)));			\
  gl(gl(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * 4, r->vertices))); \
  gl(glDrawArrays(draw_type, 0, 4))

// TODO: Should we render quads in term of Render_imm_triangle?
// From what i understand rn about opengl, more draw calls -> bad, so by implementing Render_imm_quad
// in terms of Render_imm_triangle should me more inefficient than doing manual draw calls (which is i know duplicant code but we want speed rn...)
void draw_imm_quad_3d(Context* ctx, Vector3f p0, Vector3f p1, Vector3f p2, Vector3f p3, Color c0, Color c1, Color c2, Color c3) {
  Renderer* r = ctx->ren;
  IMM_QUAD_BODY(GL_TRIANGLE_FAN);
}

void draw_imm_quad(Context* ctx, Vector2f p0, Vector2f p1, Vector2f p2, Vector2f p3, Color c0, Color c1, Color c2, Color c3) {
  draw_imm_quad_3d(ctx, (Vector3f){p0.x, p0.y, 0.f}, (Vector3f){p1.x, p1.y, 0.f}, (Vector3f){p2.x, p2.y, 0.f}, (Vector3f){p3.x, p3.y, 0.f}, c0, c1, c2, c3);
}

void draw_imm_box_3d(Context* ctx, Vector3f p0, Vector3f p1, Vector3f p2, Vector3f p3, Color c0, Color c1, Color c2, Color c3) {
    Renderer* r = ctx->ren;
  IMM_QUAD_BODY(GL_LINE_LOOP);
}

void draw_imm_box(Context* ctx, Vector2f p0, Vector2f p1, Vector2f p2, Vector2f p3, Color c0, Color c1, Color c2, Color c3) {
  draw_imm_box_3d(ctx, (Vector3f){p0.x, p0.y, 0.f}, (Vector3f){p1.x, p1.y, 0.f}, (Vector3f){p2.x, p2.y, 0.f}, (Vector3f){p3.x, p3.y, 0.f}, c0, c1, c2, c3);
}

void draw_sprite(Context* ctx, Sprite* spr) {
  Renderer* r = ctx->ren;
  Vector3f spr_pos  = (Vector3f){spr->pos.x, spr->pos.y, 1.f};
  Vector3f positions[4] = {
    (Vector3f){0.f,                         0.f,                         0.f},
    (Vector3f){(float)spr->tex_rect.size.x, 0.f,                         0.f},
    (Vector3f){(float)spr->tex_rect.size.x, (float)spr->tex_rect.size.y, 0.f},
    (Vector3f){0.f,                         (float)spr->tex_rect.size.y, 0.f}
  };

  // offset by origin
  for (size_t i = 0; i < 4; ++i) {
    positions[i] = v3f_sub(positions[i], (Vector3f){spr->origin.x, spr->origin.y, 0.f});
  }

  Vector2f texcoords[4] = {
    (Vector2f){spr->tex_rect.pos.x, spr->tex_rect.pos.y},
    (Vector2f){spr->tex_rect.pos.x + spr->tex_rect.size.x, spr->tex_rect.pos.y},
    (Vector2f){spr->tex_rect.pos.x + spr->tex_rect.size.x, spr->tex_rect.pos.y + spr->tex_rect.size.y},
    (Vector2f){spr->tex_rect.pos.x, spr->tex_rect.pos.y + spr->tex_rect.size.y},
  };

  // normalize texcoords
  for (size_t i = 0; i < 4; ++i) {
    texcoords[i].x /= spr->size.x;
    texcoords[i].y /= spr->size.y;
  }

  for (size_t i = 0; i < 4; ++i) {
    Vector3f p    = positions[i];
    Vector4f pn = (Vector4f) {
      .x = p.x,
      .y = p.y,
      .z = p.z,
      .w = 1.f,
    };
    r->vertices[i].position = pn;
    r->vertices[i].color = COLOR_WHITE;
    r->vertices[i].texcoord = texcoords[i];
  }

  // We only want to change to texture shader if the user isnt using a custom shader
  if (r->current_shader != r->custom_shader) {
    Renderer_use_texture_shader(r);
  }

  // Send sprite tint to the shader
  {
    gl(GLuint u = glGetUniformLocation(r->current_shader, "tex_tint"));
    gl(glUniform4f(u, spr->tint.r, spr->tint.g, spr->tint.b, spr->tint.a));
  }

  {
    Matrix4 scale = Mat4_scale(Mat4_identity(), (Vector3f){spr->scale.x, spr->scale.y, 1.f});

    Matrix4 rotatex = Mat4_rotate_x(Mat4_identity(), spr->rotation.x);
    Matrix4 rotatey = Mat4_rotate_y(Mat4_identity(), spr->rotation.y);
    Matrix4 rotatez = Mat4_rotate_z(Mat4_identity(), spr->rotation.z);

    Matrix4 R = Mat4_mul(rotatey, rotatex);
    Matrix4 rotate = Mat4_mul(rotatez, R);

    Matrix4 translate = Mat4_translate(Mat4_identity(), (Vector3f){spr_pos.x, spr_pos.y, 0.f});

    gl(GLuint m = glGetUniformLocation(r->current_shader, "model"));

    Matrix4 t = Mat4_mul(rotate, scale);

    Matrix4 model = Mat4_mul(translate, t);
    gl(glUniformMatrix4fv(m, 1, GL_TRUE, &model.m[0][0]));
  }

  set_view_matrix(ctx);

  {
    gl(GLuint m = glGetUniformLocation(r->current_shader, "proj"));
    gl(glUniformMatrix4fv(m, 1, GL_TRUE, &ctx->ren->proj.m[0][0]));
  }

  glActiveTexture(GL_TEXTURE0 + spr->texture->slot);
  gl(GLuint t = glGetUniformLocation(r->current_shader, "tex"));
  glUniform1i(t, spr->texture->slot);
  glBindTexture(GL_TEXTURE_2D, spr->texture->id);

  gl(glBindBuffer(GL_ARRAY_BUFFER, r->vbo));
  gl(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * 4, r->vertices));
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void draw_sprite_at(Context* ctx, Sprite* spr, Vector2f pos) {
  Vector2f previous_pos = spr->pos;

  spr->pos = pos;

  draw_sprite(ctx, spr);

  spr->pos = previous_pos;
}

void draw_rect(Context* ctx, Rect rect, Color color) {
  Vector2f tl = (Vector2f){rect.pos.x, rect.pos.y};
  Vector2f tr = (Vector2f){rect.pos.x + rect.size.x, rect.pos.y};
  Vector2f br = (Vector2f){rect.pos.x + rect.size.x, rect.pos.y + rect.size.y};
  Vector2f bl = (Vector2f){rect.pos.x, rect.pos.y + rect.size.y};
  draw_imm_quad(ctx, tl, tr, br, bl, color, color, color, color);
}

void draw_box(Context* ctx, Rect rect, Color out_color, Color fill_color) {
  Vector2f tl, tr, br, bl;
  Rect_get_points(rect, &tl, &tr, &br, &bl);
  draw_imm_quad(ctx, tl, tr, br, bl, fill_color, fill_color, fill_color, fill_color);
  draw_imm_box(ctx, tl, tr, br, bl, out_color, out_color, out_color, out_color);
}

void draw_rect_centered(Context* ctx, Rect rect, Color col) {
  Vector2f p0 = {rect.pos.x - (rect.size.x * 0.5f), rect.pos.y - (rect.size.y * 0.5f)};
  Vector2f p1 = {rect.pos.x + (rect.size.x * 0.5f), rect.pos.y - (rect.size.y * 0.5f)};
  Vector2f p2 = {rect.pos.x + (rect.size.x * 0.5f), rect.pos.y + (rect.size.y * 0.5f)};
  Vector2f p3 = {rect.pos.x - (rect.size.x * 0.5f), rect.pos.y + (rect.size.y * 0.5f)};
  draw_imm_quad(ctx, p0, p1, p2, p3, col, col, col, col);
}

void draw_imm_line3d(Context* ctx, Vector3f p0, Vector3f p1, Color c0, Color c1) {
  Renderer* r = ctx->ren;

  Vector3f positions[] = {
    p0, p1
  };

  Vector4f colors[] = {
    c0, c1
  };

  for (size_t i = 0; i < 2; ++i) {
    Vector3f p    = positions[i];
    Vector4f pn = (Vector4f) {
      .x = p.x,
      .y = p.y,
      .z = p.z,
      .w = 1.f,
    };
    Vector4f c = colors[i];
    r->vertices[i].position = pn;
    r->vertices[i].color = c;
  }

  Renderer_use_color_shader(r);
  set_matrices(ctx);
  gl(glBindBuffer(GL_ARRAY_BUFFER, r->vbo));
  gl(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * 2, r->vertices));
  gl(glDrawArrays(GL_LINE_STRIP, 0, 2));
}

void draw_imm_line(Context* ctx, Vector2f p0, Vector2f p1, Color c0, Color c1) {
  draw_imm_line3d(ctx, (Vector3f) {p0.x, p0.y, 0.f}, (Vector3f) {p1.x, p1.y, 0.f}, c0, c1);
}

void draw_point_3d(Context* ctx, Vector3f pos, Color col) {
 Renderer* r = ctx->ren;

  Vector3f positions[] = {
    pos
  };

  Vector4f colors[] = {
    col
  };


  for (size_t i = 0; i < 1; ++i) {
    Vector3f p    = positions[i];
    Vector4f pn = (Vector4f) {
      .x = p.x,
      .y = p.y,
      .z = p.z,
      .w = 1.f,
    };
    Vector4f c = colors[i];
    r->vertices[i].position = pn;
    r->vertices[i].color = c;
  }

  set_matrices(ctx);
  gl(glBindBuffer(GL_ARRAY_BUFFER, r->vbo));
  gl(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * 1, r->vertices));
  gl(glDrawArrays(GL_POINTS, 0, 1));
}

void draw_point(Context* ctx, Vector2f p, Color col) {
  draw_point_3d(ctx, (Vector3f){p.x, p.y, 0.f}, col);
}

void draw_text(Context* ctx, Font* font, cstr text, Vector2f pos, int char_size, Color color) {
  draw_text_aligned(ctx, font, text, pos, char_size, color, TEXT_ALIGN_TOP_LEFT);
}

void draw_text_aligned(Context* ctx, Font* font, cstr text, Vector2f pos, int char_size, Color color, Text_align align) {
  if (!text) return;
  Sprite spr = {0};
  if (font == NULL) font = &ctx->default_font;
  if (font->current_character_size != char_size) {
    Font_generate_atlas_tex(font, char_size);
  }
  if (!Sprite_init(&spr, font->texture, 1, 1)) {
    log_f(LOG_ERROR, "Could not initialize text sprite");
    // TODO: close program
  }
  spr.tint = color;

  Vector2f full_text_size = get_text_size(ctx, font, text, char_size);

  Vector2f codepoint_pos = pos;
  while (*text != '\0') {

    int codepoint = *text;

    float sf = stbtt_ScaleForPixelHeight(&font->font, (float32)font->current_character_size);

    // TODO: Handle newline and other codepoints
    Codepoint_rect_KV* kv = hmgetp_null(font->codepoint_rect_map, codepoint);
    ASSERT(kv != NULL);
    Rect rect = kv->value.rect;

    spr.tex_rect = rect;
    spr.pos.y = pos.y + kv->value.offset.y;
    // NOTE: Make the text draw from topleft
    /* spr.pos.y += font->current_character_size; */
    spr.pos.x = codepoint_pos.x + kv->value.offset.x;

    switch (align) {
    case TEXT_ALIGN_TOP_LEFT: {
    } break;
    case TEXT_ALIGN_TOP_CENTER: {
      spr.pos.x -= full_text_size.x / 2.f;
    } break;
    case TEXT_ALIGN_TOP_RIGHT: {
      spr.pos.x -= full_text_size.x;
    } break;
    case TEXT_ALIGN_CENTER_LEFT: {
      spr.pos.y -= full_text_size.y / 2.f;
    } break;
    case TEXT_ALIGN_CENTER_CENTER: {
      spr.pos.y -= full_text_size.y / 2.f;
      spr.pos.x -= full_text_size.x / 2.f;
    } break;
    case TEXT_ALIGN_CENTER_RIGHT: {
      spr.pos.y -= full_text_size.y / 2.f;
      spr.pos.x -= full_text_size.x;
    } break;
    case TEXT_ALIGN_BOTTOM_LEFT: {
      spr.pos.y -= full_text_size.y;
    } break;
    case TEXT_ALIGN_BOTTOM_CENTER: {
      spr.pos.y -= full_text_size.y;
      spr.pos.x -= full_text_size.x / 2.f;
    } break;
    case TEXT_ALIGN_BOTTOM_RIGHT: {
      spr.pos.y -= full_text_size.y;
      spr.pos.x -= full_text_size.x;
    } break;
    default: ASSERT(0 && "Unreachable!");
    }

    draw_sprite(ctx, &spr);

    int adv, lsb;
    stbtt_GetCodepointHMetrics(&font->font, codepoint, &adv, &lsb);
    codepoint_pos.x += (adv * sf) + (lsb * sf);
    if (*(text+1) != '\0') {
      int next_codepoint = *(text+1);
      int kern = stbtt_GetCodepointKernAdvance(&font->font, codepoint, next_codepoint);
      codepoint_pos.x += kern*sf;
    }
    *text++;
  }
}

void draw_bezier_curve(Context* ctx, Vector2f p0, Vector2f p1, Vector2f p2, int res, Color color) {
  Vector2f prev_pos = p0;
  for (int i = 0; i < res; ++i) {
    float t = (i + 1.f) / res;
    Vector2f next_pos = v2f_bezier_lerp(p0, p1, p2, t);
    draw_imm_line(ctx, prev_pos, next_pos, color, color);
    prev_pos = next_pos;
  }
}

Vector2f get_text_size(Context* ctx, Font* font, cstr text, int char_size) {
  (void)ctx;
  Vector2f size = {0};
  if (text == NULL) return size;
  size.y = 0.f;
  while (*text != '\0') {
    int codepoint = *text;

    float sf = stbtt_ScaleForPixelHeight(&font->font, (float32)char_size);

    /* Codepoint_rect_KV* kv = hmgetp_null(font->codepoint_rect_map, codepoint); */
    /* if (kv == NULL) { */
    /*   // could not find codepoint in font, so continue */
    /*   continue; */
    /* } */
    /* Rect rect = kv->value.rect; */

    int x0, y0, x1, y1;
    stbtt_GetCodepointBox(&font->font, codepoint, &x0, &y0, &x1, &y1);

    float height = (y1 - y0)*sf;
    if (height > size.y) {
      size.y = height;
    }

    int adv, lsb;
    stbtt_GetCodepointHMetrics(&font->font, codepoint, &adv, &lsb);
    if (*(text+1) != '\0') {
      int next_codepoint = *(text+1);
      int kern = stbtt_GetCodepointKernAdvance(&font->font, codepoint, next_codepoint);
      size.x += kern*sf;
    }
    *text++;
    size.x += (adv * sf) + (lsb * sf);
  }
  return size;
}

Vector2f get_text_sizen(Context* ctx, Font* font, char* text, uint32 text_size, int char_size) {
  (void)ctx;
  Vector2f size = {0};
  if (text == NULL) return size;
  size.y = (float32)char_size;
  uint32 n = 0;
  while (n < text_size && *text != '\0') {
    int codepoint = *text;

    float sf = stbtt_ScaleForPixelHeight(&font->font, (float32)char_size);

    Codepoint_rect_KV* kv = hmgetp_null(font->codepoint_rect_map, codepoint);
    if (kv == NULL) {
      // could not find codepoint in font, so continue
      continue;
    }

    int adv, lsb;
    stbtt_GetCodepointHMetrics(&font->font, codepoint, &adv, &lsb);
    *text++;
    n++;
    size.x += (adv * sf) + (lsb * sf);
  }
  return size;
}

// Blendmode

void set_blend_mode(const Blendmode mode) {
  switch(mode) {
  case BLENDMODE_NORMAL: {
    gl(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    gl(glBlendEquation(GL_FUNC_ADD));
  } break;
  case BLENDMODE_ADD: {
    gl(glBlendFunc(GL_ONE, GL_ONE));
    gl(glBlendEquation(GL_FUNC_ADD));
  } break;
  case BLENDMODE_SUB: {
    gl(glBlendFunc(GL_ONE, GL_ONE));
    gl(glBlendEquation(GL_FUNC_SUBTRACT));
  } break;
  case BLENDMODE_MUL: {
    gl(glBlendFunc(GL_DST_COLOR, GL_ZERO));
    gl(glBlendEquation(GL_FUNC_ADD));
  } break;
  case BLENDMODE_NONE: {
    gl(glBlendFunc(GL_ONE, GL_ZERO));
    gl(glBlendEquation(GL_FUNC_ADD));
  } break;
  default: {
    ASSERT(0 && "Unreachable");
  }
  }
}

// Shader

int create_shader(const char* vert_src, const char* frag_src) {
  if (!vert_src && !frag_src) return 0;

  gl(unsigned int vert = glCreateShader(GL_VERTEX_SHADER));
  if (vert == 0) {
    log_f(LOG_ERROR, "Failed to create vertex shader!");
    return 0;
  }
  /* log_f(LOG_INFO, "Successfully created vertex shader!"); */

  gl(unsigned int frag = glCreateShader(GL_FRAGMENT_SHADER));
  if (frag == 0) {
    log_f(LOG_ERROR, "Failed to create fragment shader!");
    return 0;
  }
  /* log_f(LOG_INFO, "Successfully created fragment shader!"); */

  gl(glShaderSource(vert, 1, (const GLchar**)&vert_src, NULL));
  gl(glShaderSource(frag, 1, (const GLchar**)&frag_src, NULL));

  gl(glCompileShader(vert));
  int compiled;
  gl(glGetShaderiv(vert, GL_COMPILE_STATUS, &compiled));
  if (compiled == GL_FALSE) {
    int infolog_len;
    gl(glGetShaderiv(vert, GL_INFO_LOG_LENGTH, &infolog_len));

    char* infolog = (char*)malloc(sizeof(char)*infolog_len);

    gl(glGetShaderInfoLog(vert, infolog_len, &infolog_len, infolog));

    log_f(LOG_ERROR, "Failed to compile vertex shader: \n    %s", infolog);

    free(infolog);
    return 0;
  }
  /* log_f(LOG_INFO, "Successfully compiled vertex shader!"); */

  gl(glCompileShader(frag));
  gl(glGetShaderiv(frag, GL_COMPILE_STATUS, &compiled));
  if (compiled == GL_FALSE) {
    int infolog_len;
    gl(glGetShaderiv(frag, GL_INFO_LOG_LENGTH, &infolog_len));

    char* infolog = (char*)malloc(sizeof(char)*infolog_len);

    gl(glGetShaderInfoLog(frag, infolog_len, &infolog_len, infolog));

    log_f(LOG_ERROR, "Failed to compile fragment shader: \n    %s", infolog);

    free(infolog);
    return 0;
  }
  /* log_f(LOG_INFO, "Successfully compiled fragment shader!"); */

  gl(unsigned int program = glCreateProgram());
  if (program == GL_FALSE) {
    log_f(LOG_ERROR, "Failed to create shader program!");
    return 0;
  }
  /* log_f(LOG_INFO, "Successfully created shader program!"); */

  gl(glAttachShader(program, vert));
  gl(glAttachShader(program, frag));

  gl(glLinkProgram(program));

  int linked;
  gl(glGetProgramiv(program, GL_LINK_STATUS, &linked));
  if (linked == GL_FALSE) {
    int infolog_len;
    gl(  glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infolog_len));

    char* infolog = (char*)malloc(sizeof(char)*infolog_len);

    gl(glGetProgramInfoLog(program, infolog_len, &infolog_len, infolog));

    log_f(LOG_ERROR, "Failed to link shader program: %s", infolog);

    free(infolog);
    return 0;
  }

  /* log_f(LOG_INFO, "Successfully linked shader program!"); */

  gl(glDetachShader(program, vert));
  gl(glDetachShader(program, frag));
  gl(glDeleteShader(vert));
  gl(glDeleteShader(frag));

  glValidateProgram(program);

  int validated;
  glGetProgramiv(program, GL_VALIDATE_STATUS, &validated);
  if (validated == GL_FALSE) {
    int infolog_len;
    gl(  glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infolog_len));

    char* infolog = (char*)malloc(sizeof(char)*infolog_len);

    gl(glGetProgramInfoLog(program, infolog_len, &infolog_len, infolog));

    log_f(LOG_ERROR, "Failed to validate shader program: %s", infolog);

    free(infolog);
    return 0;
  }
  /* log_f(LOG_INFO, "Successfully validated shader program!"); */

  return program;
}


// Utility

const char* gl_error_as_cstr(int e) {
  switch (e) {
  case GL_NO_ERROR: return "GL_NO_ERROR"; break;
  case GL_INVALID_ENUM: return "GL_INVALID_ENUM"; break;
  case GL_INVALID_VALUE: return "GL_INVALID_VALUE"; break;
  case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION"; break;
  case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
  case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY"; break;
  case GL_STACK_UNDERFLOW: return "GL_STACK_UNDERFLOW"; break;
  case GL_STACK_OVERFLOW: return "GL_STACK_OVERFLOW"; break;
  default: ASSERT(0 && "Unreachable!");
  }
  return "INVALID!";
}

void gl_check_and_log_error(const char* file, int line) {
  int e = glGetError();

  if (e != GL_NO_ERROR) {
    log_f(LOG_ERROR, "%s:%u:0: GL error: %s", file, line, gl_error_as_cstr(e));
    exit(1); // is it wise to just exit without deinitializing glfw and gl resources?
  }
}
