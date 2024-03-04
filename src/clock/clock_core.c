#include <clock/clock_core.h>
#include <stdlib.h>
#define MATRIX_IMPLEMENTATION
#include <clock/clock_matrix.h>
#define COMMONLIB_IMPLEMENTATION
#include <commonlib.h>

// Window

int Window_init(Window* win, unsigned int width, unsigned int height, const char* title){
  win->title = title;

  win->width = width == 0 ? DEFAULT_WIN_WIDTH : width;
  win->height = height == 0 ? DEFAULT_WIN_HEIGHT : height;

  log_f(LOG_INFO, "Running '%s'", win->title);
  if (!glfwInit()){
    log_f(LOG_ERROR, "GLFW Could not initialize!");
    return -1;
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
    return -1;
  }
  log_f(LOG_INFO, "Window Created!");
  glfwMakeContextCurrent(win->glfw_win);

  int version = gladLoadGL(glfwGetProcAddress);

  if (version == 0){
    log_f(LOG_ERROR, "Failed to initialize GLAD Context!");
    return -1;
  }

  log_f(LOG_INFO, "Loaded OpenGL %d.%d!", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

  win->tp1 = glfwGetTime();
  win->tp2 = 0.0;
  win->delta = 0.0;
  win->fps = 0;
  win->mpos = (Vector4d){0};

  return 0;
}

void Window_begin_draw(Window* win){
  glfwGetCursorPos(win->glfw_win, &win->mpos.x, &win->mpos.y);

  win->tp2 = glfwGetTime();
  win->delta = win->tp2 - win->tp1;
  win->tp1 = win->tp2;

  win->fps = (1.0 / win->delta);

  char* tmpbuff = (char*)malloc(sizeof(char)*1024);
  snprintf(tmpbuff, 1024, "%s | fps: %d | delta: %f", win->title, win->fps, win->delta);

  glfwSetWindowTitle(win->glfw_win, tmpbuff);

  free(tmpbuff);
}

void Window_end_draw(Window* win){
  glfwSwapBuffers(win->glfw_win);
  glfwPollEvents();
}

void Window_deinit(Window* win){
  glfwDestroyWindow(win->glfw_win);
  log_f(LOG_INFO, "Window Destroyed!");
  glfwTerminate();
  log_f(LOG_INFO, "GLFW Terminated!");
}

void Window_clear(Window* win, Color color){
  gl(glClearColor(color.r, color.g, color.b, color.a));
  gl(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

// Renderer


int Renderer_init(Renderer* r, Window* win){
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

  // create default shader
  r->shader = create_shader(default_vert_shader, default_frag_shader);
  if (r->shader == 0){
    return -1;
  }
  log_f(LOG_INFO, "Loaded default shader!");

  gl(glUseProgram(r->shader));

  return 0;
}

void Renderer_deinit(Renderer* r){
  gl(glUseProgram(0));
  gl(glDeleteProgram(r->shader));
  gl(glDisableVertexAttribArray(0));
  gl(glDisableVertexAttribArray(1));
  gl(glDeleteBuffers(1, r->vbo));
  gl(glDeleteVertexArrays(1, r->vao));
}

bool Renderer_set_shader(Renderer* r, const char* vs, const char* fs){
  GLuint shader = create_shader(vs, fs);
  if (shader == 0){
    return false;
  }

  r->shader = shader;
  gl(glUseProgram(r->shader));

  return true;
}

void Render_imm_triangle(Renderer* r, Vector3f p0, Vector3f p1, Vector3f p2, Color c0, Color c1, Color c2){
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
  gl(glBindBuffer(GL_ARRAY_BUFFER, r->vbo[0]));				\
  gl(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * 4, r->vertices)); \
  glDrawArrays(draw_type, 0, 4)

// TODO: Should we render quads in term of Render_imm_triangle?
// From what i understand rn about opengl, more draw calls -> bad, so by implementing Render_imm_quad
// in terms of Render_imm_triangle should me more inefficient than doing manual draw calls (which is i know duplicant code but we want speed rn...)
void Render_imm_quad(Renderer* r, Vector3f p0, Vector3f p1, Vector3f p2, Vector3f p3, Color c0, Color c1, Color c2, Color c3){
  IMM_QUAD_BODY(GL_TRIANGLE_FAN);
}

void Render_imm_box(Renderer* r, Vector3f p0, Vector3f p1, Vector3f p2, Vector3f p3, Color c0, Color c1, Color c2, Color c3){
  IMM_QUAD_BODY(GL_LINE_LOOP);
}

void Render_texture(Renderer* r, Vector3f pos, Texture* tex){
  Vector3f positions[4] = {
    v3f_add(pos, (Vector3f){0.f, 0.f, 0.f}),
    v3f_add(pos, (Vector3f){(float)tex->size.x, 0.f, 0.f}),
    v3f_add(pos, (Vector3f){(float)tex->size.x, (float)tex->size.y, 0.f}),
    v3f_add(pos, (Vector3f){0.f, (float)tex->size.y, 0.f}),
  };

  Vector2f texcoords[] = {
    (Vector2f){0.f, 0.f},
    (Vector2f){1.f, 0.f},
    (Vector2f){1.f, 1.f},
    (Vector2f){0.f, 1.f}
  };

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

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex->id);

  gl(glBindBuffer(GL_ARRAY_BUFFER, r->vbo[0]));
  gl(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * 4, r->vertices));
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
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
    gl(  glGetShaderiv(vert, GL_INFO_LOG_LENGTH, &infolog_len));

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
    gl(  glGetShaderiv(frag, GL_INFO_LOG_LENGTH, &infolog_len));

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
