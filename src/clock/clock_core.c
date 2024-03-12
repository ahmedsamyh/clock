#include <clock/clock_core.h>
#include <stdlib.h>
#define MATRIX_IMPLEMENTATION
#include <clock/clock_matrix.h>
#define COMMONLIB_IMPLEMENTATION
#include <commonlib.h>
#include <assert.h>
#include <string.h>

// Window

bool Window_init(Window* win, unsigned int width, unsigned int height, const char* title){
  win->title = title;

  win->width = width == 0 ? DEFAULT_WIN_WIDTH : width;
  win->height = height == 0 ? DEFAULT_WIN_HEIGHT : height;

  log_f(LOG_INFO, "Running '%s'", win->title);
  if (!glfwInit()){
    log_f(LOG_ERROR, "GLFW Could not initialize!");
    return false;
  }
  log_f(LOG_INFO, "GLFW initialized!");

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  win->glfw_win = glfwCreateWindow(win->width, win->height, win->title, NULL, NULL);

  if (win->glfw_win == NULL){
    char* tmpbuff = (char*)malloc(sizeof(char)*1024);
    glfwGetError((const char**)&tmpbuff);
    log_f(LOG_ERROR, "Could not initialize window: %s", tmpbuff);
    free(tmpbuff);
    return false;
  }
  log_f(LOG_INFO, "Window Created!");
  glfwMakeContextCurrent(win->glfw_win);

  int version = gladLoadGL(glfwGetProcAddress);

  if (version == 0){
    log_f(LOG_ERROR, "Failed to initialize GLAD Context!");
    return false;
  }

  log_f(LOG_INFO, "Loaded OpenGL %d.%d!", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

  return true;
}


void Window_deinit(Window* win){
  glfwDestroyWindow(win->glfw_win);
  log_f(LOG_INFO, "Window Destroyed!");
  glfwTerminate();
  log_f(LOG_INFO, "GLFW Terminated!");
}

// Context / main user api

bool clock_init(Context* ctx, unsigned int window_width, unsigned int window_height, const char* title){
  ctx->win = (Window*)malloc(sizeof(Window));
  ctx->ren = (Renderer*)malloc(sizeof(Renderer));
  if (!Window_init(ctx->win, window_width, window_height, title)){
    return false;
  }
  if (!Renderer_init(ctx->ren, ctx->win)){
    return false;
  }

  glfwSetWindowUserPointer(ctx->win->glfw_win, ctx);
  glfwSetKeyCallback(ctx->win->glfw_win, key_callback);

  ctx->tp1 = glfwGetTime();
  ctx->tp2 = 0.0;
  ctx->delta = 0.0;
  ctx->fps = 0;
  ctx->mpos = (Vector2f){0};

  glGetIntegerv(GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS, &ctx->max_tex_image_count);

  return true;
}

bool clock_should_quit(Context* ctx){
  return glfwWindowShouldClose(ctx->win->glfw_win);
}

void clock_update_keys(Context* ctx){
  Key* keys   = ctx->keys;
  Window* win = ctx->win;
  // update key states
  for (size_t i = 0; i < KEYS_COUNT; ++i){
    keys[i].just_pressed = false;
    keys[i].pressed = false;
    keys[i].released = false;
  }

  for (int i = 0; i < KEYS_COUNT; ++i){
    keys[i].prev_state = keys[i].held;
    int state = glfwGetKey(win->glfw_win, i);
    if (state == GLFW_PRESS){
      keys[i].held = true;
    } else if (state == GLFW_RELEASE){
      keys[i].held = false;
    }
  }

  for (int i = 0; i < KEYS_COUNT; ++i){
    if (!keys[i].prev_state && keys[i].held){
      keys[i].just_pressed = true;
      keys[i].pressed = true;
    }
    if (keys[i].prev_state && !keys[i].held){
      keys[i].released = true;
    }
  }
}

void clock_begin_draw(Context* ctx){
  Window* win = ctx->win;
  Key* keys = ctx->keys;
  double mx, my;
  glfwGetCursorPos(win->glfw_win, &mx, &my);
  ctx->mpos.x = (float)mx;
  ctx->mpos.y = (float)my;

  ctx->tp2 = glfwGetTime();
  ctx->delta = ctx->tp2 - ctx->tp1;
  ctx->tp1 = ctx->tp2;

  ctx->fps = (1.0 / ctx->delta);

  char* tmpbuff = (char*)malloc(sizeof(char)*1024);
  snprintf(tmpbuff, 1024, "%s | %dfps | %fs", win->title, ctx->fps, ctx->delta);

  glfwSetWindowTitle(win->glfw_win, tmpbuff);

  free(tmpbuff);
}

void clock_end_draw(Context* ctx){
  clock_update_keys(ctx);
  glfwSwapBuffers(ctx->win->glfw_win);
  glfwPollEvents();
}

void clock_clear(Context* ctx, Color color){
  gl(glClearColor(color.r, color.g, color.b, color.a));
  gl(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}


void clock_deinit(Context* ctx){
  Renderer_deinit(ctx->ren);
  Window_deinit(ctx->win);
  free(ctx->win);
  free(ctx->ren);
}

// Callbacks

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){

  Context* ctx = (Context*)glfwGetWindowUserPointer(window);
  Key* keys = ctx->keys;

  if (action == GLFW_REPEAT){
    /* log_f(LOG_INFO, "Key: %d repeat", key); */
    keys[key].pressed = true;
  }
}

// Renderer

bool Renderer_init(Renderer* r, Window* win){
  r->win = win;

  gl(glGenVertexArrays(VAO_COUNT, r->vao););
  gl(glBindVertexArray(r->vao[0]););  // 0 is the default one

  // vbo
  gl(glGenBuffers(VBO_COUNT, r->vbo););
  gl(glBindBuffer(GL_ARRAY_BUFFER, r->vbo[0]);); // 0 is the default one
  gl(glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * VERTEX_CAP, NULL, GL_STATIC_DRAW));;

  // position
  gl(glEnableVertexAttribArray(0));
  gl(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL)); //;(const GLvoid*)offsetof(Vertex, position));

  // color
  gl(glEnableVertexAttribArray(1));
  gl(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, color)));

  // texcoord
  gl(glEnableVertexAttribArray(2));
  gl(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, texcoord)));

  // create shaders
  r->texture_shader = create_shader(tex_vert_shader, tex_frag_shader);
  r->color_shader = create_shader(color_vert_shader, color_frag_shader);
  r->custom_shader = 0;

  log_f(LOG_INFO, "Loaded default shader!");

  return true;
}

void Renderer_deinit(Renderer* r){
  gl(glUseProgram(0));
  if (r->texture_shader != 0)
    gl(glDeleteProgram(r->texture_shader));
  if (r->color_shader != 0)
    gl(glDeleteProgram(r->color_shader));
  if (r->custom_shader != 0)
    gl(glDeleteProgram(r->custom_shader));
  gl(glDisableVertexAttribArray(0));
  gl(glDisableVertexAttribArray(1));
  gl(glDeleteBuffers(1, r->vbo));
  gl(glDeleteVertexArrays(1, r->vao));
}

bool Renderer_create_custom(Renderer* r, const char* vs, const char* fs){
  GLuint shader = create_shader(vs, fs);
  if (shader == 0){
    return false;
  }

  r->custom_shader = shader;

  return true;
}

void Renderer_use_texture_shader(Renderer* r){
  gl(glUseProgram(r->texture_shader));
}

void Renderer_use_color_shader(Renderer* r){
  gl(glUseProgram(r->color_shader));
}

void Renderer_use_custom_shader(Renderer* r){
  gl(glUseProgram(r->custom_shader));
}

void draw_imm_triangle(Context* ctx, Vector3f p0, Vector3f p1, Vector3f p2, Color c0, Color c1, Color c2){
  Renderer* r = ctx->ren;
  // input postions are from {0..width, 0..height}
  // opengl wants them from {-1.f, 1.f, -1.f, 1.f}

  Vector3f positions[] = {
    p0, p1, p2
  };

  Vector4f colors[] = {
    c0, c1, c2,
  };

  // resolution of the z-axis, i dont know what the standard resolution people use in the z-axis.
  // so we just use the height as the z-axis
  float depth = r->win->height;
  for (size_t i = 0; i < 3; ++i){
    Vector3f p = positions[i];
    Vector4f pn = (Vector4f){
      .x = (p.x / (float)r->win->width)*2.f - 1.f,
      .y = (1.f - p.y / (float)r->win->height)*2.f - 1.f,
      .z = (p.z / (float)depth)*2.f - 1.f,
      .w = 1.f,
    };
    Vector4f c = colors[i];
    r->vertices[i].position = pn;
    r->vertices[i].color = c;
  }


  gl(glBindBuffer(GL_ARRAY_BUFFER, r->vbo[0]));// 0 is the default one
  gl(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * 3, r->vertices));

  gl(glDrawArrays(GL_TRIANGLES, 0, 3));
}

#define IMM_QUAD_BODY(draw_type)					\
  Vector3f positions[] = {p0, p1, p2, p3};				\
  Vector4f colors[] = {c0, c1, c2, c3};					\
  float depth = r->win->height;						\
  for (size_t i = 0; i < 4; ++i){					\
    Vector3f p = positions[i];						\
    Vector4f pn = (Vector4f){						\
      .x = (p.x / (float)r->win->width)*2.f - 1.f,			\
      .y = (1.f - p.y / (float)r->win->height)*2.f - 1.f,		\
      .z = (p.z / (float)depth)*2.f - 1.f, .w = 1.f,};			\
    Vector4f c = colors[i];						\
    r->vertices[i].position = pn;					\
    r->vertices[i].color = c;						\
  }									\
  Renderer_use_color_shader(r);						\
  gl(glBindBuffer(GL_ARRAY_BUFFER, r->vbo[0]));				\
  gl(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * 4, r->vertices)); \
  glDrawArrays(draw_type, 0, 4)


// TODO: Should we render quads in term of Render_imm_triangle?
// From what i understand rn about opengl, more draw calls -> bad, so by implementing Render_imm_quad
// in terms of Render_imm_triangle should me more inefficient than doing manual draw calls (which is i know duplicant code but we want speed rn...)
void draw_imm_quad(Context* ctx, Vector3f p0, Vector3f p1, Vector3f p2, Vector3f p3, Color c0, Color c1, Color c2, Color c3){
  Renderer* r = ctx->ren;
  IMM_QUAD_BODY(GL_TRIANGLE_FAN);
}

void draw_imm_box(Context* ctx, Vector3f p0, Vector3f p1, Vector3f p2, Vector3f p3, Color c0, Color c1, Color c2, Color c3){
    Renderer* r = ctx->ren;
  IMM_QUAD_BODY(GL_LINE_LOOP);
}

void draw_texture(Context* ctx, Vector3f pos, Rect texcoord, Texture* tex){
  assert(0 && "Unimplemented");
}

void draw_sprite(Context* ctx, Sprite* spr){
  Renderer* r = ctx->ren;
  Vector3f pos = (Vector3f){spr->pos.x, spr->pos.y, 0.f};
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

  // rotate
  for (size_t i = 0; i < 4; ++i) {
    Vector3f p = positions[i];
    Vector4f v4 = Mat4_rotate_x_vector((Vector4f){p.x, p.y, p.z, 1.f}, spr->rotation.x);
    v4 =          Mat4_rotate_y_vector(v4,                             spr->rotation.y);
    v4 =          Mat4_rotate_z_vector(v4,                             spr->rotation.z);
    positions[i] = (Vector3f){v4.x, v4.y, v4.z};
  }

  // scale
  for (size_t i = 0; i < 4; ++i) {
    positions[i] = v3f_mul(positions[i], (Vector3f){spr->scale.x, spr->scale.y, 0.f});
  }

  // translate
  for (size_t i = 0; i < 4; ++i) {
    Vector3f p = positions[i];
    Vector4f v4 = Mat4_translate_vector((Vector4f){p.x, p.y, p.z, 1.f}, pos);
    positions[i] = (Vector3f){v4.x, v4.y, v4.z};
  };


  Vector2f texcoords[4] = {
    (Vector2f){spr->tex_rect.pos.x, spr->tex_rect.pos.y},
    (Vector2f){spr->tex_rect.pos.x + spr->tex_rect.size.x, spr->tex_rect.pos.y},
    (Vector2f){spr->tex_rect.pos.x + spr->tex_rect.size.x, spr->tex_rect.pos.y + spr->tex_rect.size.y},
    (Vector2f){spr->tex_rect.pos.x, spr->tex_rect.pos.y + spr->tex_rect.size.y},
  };

  // normalize texcoords
  for (size_t i = 0; i < 4; ++i){
    texcoords[i].x /= spr->size.x;
    texcoords[i].y /= spr->size.y;
  }

  float depth = r->win->height;
  for (size_t i = 0; i < 4; ++i){
    Vector3f p = positions[i];
    Vector4f pn = (Vector4f){
      .x = (p.x / (float)r->win->width)*2.f - 1.f,
      .y = (1.f - p.y / (float)r->win->height)*2.f - 1.f,
      .z = (p.z / (float)depth)*2.f - 1.f, .w = 1.f
    };
    r->vertices[i].position = pn;
    r->vertices[i].color = COLOR_WHITE;
    r->vertices[i].texcoord = texcoords[i];
  }

  Renderer_use_texture_shader(r);

  /* glActiveTexture(GL_TEXTURE0 + (spr->texture.slot % )); */
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, spr->texture.id);

  gl(glBindBuffer(GL_ARRAY_BUFFER, r->vbo[0]));
  gl(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * 4, r->vertices));
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void draw_rect(Context* ctx, Rect rect, Color color){
  Renderer* r = ctx->ren;
  Vector3f tl = (Vector3f){rect.pos.x, rect.pos.y, 0.f};
  Vector3f tr = (Vector3f){rect.pos.x + rect.size.x, rect.pos.y, 0.f};
  Vector3f br = (Vector3f){rect.pos.x + rect.size.x, rect.pos.y + rect.size.y, 0.f};
  Vector3f bl = (Vector3f){rect.pos.x, rect.pos.y + rect.size.y, 0.f};
  draw_imm_quad(ctx, tl, tr, br, bl, color, color, color, color);
}

// Shader

int create_shader(const char* vert_src, const char* frag_src){
  gl(unsigned int vert = glCreateShader(GL_VERTEX_SHADER));
  if (vert == 0){
    log_f(LOG_ERROR, "Failed to create vertex shader!");
    return 0;
  }
  /* log_f(LOG_INFO, "Successfully created vertex shader!"); */

  gl(unsigned int frag = glCreateShader(GL_FRAGMENT_SHADER));
  if (frag == 0){
    log_f(LOG_ERROR, "Failed to create fragment shader!");
    return 0;
  }
  /* log_f(LOG_INFO, "Successfully created fragment shader!"); */

  gl(glShaderSource(vert, 1, (const GLchar**)&vert_src, NULL));
  gl(glShaderSource(frag, 1, (const GLchar**)&frag_src, NULL));

  gl(glCompileShader(vert));
  int compiled;
  gl(glGetShaderiv(vert, GL_COMPILE_STATUS, &compiled));
  if (compiled == GL_FALSE){
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
  if (compiled == GL_FALSE){
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
  if (program == GL_FALSE){
    log_f(LOG_ERROR, "Failed to create shader program!");
    return 0;
  }
  /* log_f(LOG_INFO, "Successfully created shader program!"); */

  gl(glAttachShader(program, vert));
  gl(glAttachShader(program, frag));

  gl(glLinkProgram(program));

  int linked;
  gl(glGetProgramiv(program, GL_LINK_STATUS, &linked));
  if (linked == GL_FALSE){
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
  if (validated == GL_FALSE){
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

const char* gl_error_as_cstr(int e){
  switch (e){
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

void gl_check_and_log_error(int line){
  int e = glGetError();

  if (e != GL_NO_ERROR){
    log_f(LOG_ERROR, "%s:%u:0: GL error: %s", __FILE__, line, gl_error_as_cstr(e));
    exit(1); // is it wise to just exit without deinitializing glfw and gl resources?
  }

}
