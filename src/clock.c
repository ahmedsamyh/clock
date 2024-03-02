#include <clock.h>
#include <stddef.h> // offsetof

// Window
int Window_init(Window* win, u32 width, u32 height, const char* title){
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
    glfwGetError((const char**)&tmpbuff);
    log_f(LOG_ERROR, "Could not initialize window: %s", tmpbuff);
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

  snprintf(tmpbuff, TMPBUFF_SIZE, "%s | fps: %d | delta: %f", win->title, win->fps, win->delta);

  glfwSetWindowTitle(win->glfw_win, tmpbuff);
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
  glClearColor(color.r, color.g, color.b, color.a); gl();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); gl();
}


// Renderer
int Renderer_init(Renderer* r, Window* win){
  r->win = win;

  glGenVertexArrays(VAO_COUNT, r->vao);  gl();
  glBindVertexArray(r->vao[0]);  gl(); // 0 is the default one

  // vbo
  glGenBuffers(VBO_COUNT, r->vbo);  gl();
  glBindBuffer(GL_ARRAY_BUFFER, r->vbo[0]);  gl();// 0 is the default one
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * VERTEX_CAP, NULL, GL_STATIC_DRAW); gl();

  // position
  glEnableVertexAttribArray(0); gl();
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL); gl(); //;(const GLvoid*)offsetof(Vertex, position));

  // color
  glEnableVertexAttribArray(1); gl();
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, color)); gl();

  // create default shader
  r->current_shader = create_shader(default_vert_shader, default_frag_shader);
  if (r->current_shader < 0){
    return -1;
  }

  glUseProgram(r->current_shader); gl();

  return 0;
}

void Renderer_deinit(Renderer* r){
  glUseProgram(0); gl();
  glDeleteProgram(r->current_shader); gl();
  glDisableVertexAttribArray(0); gl();
  glDisableVertexAttribArray(1); gl();
  glDeleteBuffers(1, r->vbo); gl();
  glDeleteVertexArrays(1, r->vao); gl();
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

  glBindBuffer(GL_ARRAY_BUFFER, r->vbo[0]); gl();// 0 is the default one
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * 3, r->vertices); gl();

  glDrawArrays(GL_TRIANGLES, 0, 3); gl();
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
  glBindBuffer(GL_ARRAY_BUFFER, r->vbo[0]); gl();			\
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * 4, r->vertices); gl(); \
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

// Shader
int create_shader(const char* vert_src, const char* frag_src){
  GLuint vert = glCreateShader(GL_VERTEX_SHADER); gl();
  if (vert == 0){
    log_f(LOG_ERROR, "Failed to create vertex shader!");
    return -1;
  }
  log_f(LOG_INFO, "Successfully created vertex shader!");

  GLuint frag = glCreateShader(GL_FRAGMENT_SHADER); gl();
  if (frag == 0){
    log_f(LOG_ERROR, "Failed to create fragment shader!");
    return -1;
  }
  log_f(LOG_INFO, "Successfully created fragment shader!");

  glShaderSource(vert, 1, (const GLchar**)&vert_src, NULL); gl();
  glShaderSource(frag, 1, (const GLchar**)&frag_src, NULL); gl();

  glCompileShader(vert); gl();
  int compiled;
  glGetShaderiv(vert, GL_COMPILE_STATUS, &compiled); gl();
  if (compiled == GL_FALSE){
    int infolog_len;
    glGetShaderiv(vert, GL_INFO_LOG_LENGTH, &infolog_len); gl();

    char* infolog = (char*)malloc(sizeof(char)*infolog_len);

    glGetShaderInfoLog(vert, infolog_len, &infolog_len, infolog); gl();

    log_f(LOG_ERROR, "Failed to compile vertex shader: \n    %s", infolog);

    free(infolog);
    return -1;
  }
  log_f(LOG_INFO, "Successfully compiled vertex shader!");

  glCompileShader(frag); gl();
  glGetShaderiv(frag, GL_COMPILE_STATUS, &compiled); gl();
  if (compiled == GL_FALSE){
    int infolog_len;
    glGetShaderiv(frag, GL_INFO_LOG_LENGTH, &infolog_len); gl();

    char* infolog = (char*)malloc(sizeof(char)*infolog_len);

    glGetShaderInfoLog(frag, infolog_len, &infolog_len, infolog); gl();

    log_f(LOG_ERROR, "Failed to compile fragment shader: \n    %s", infolog);

    free(infolog);
    return -1;
  }
  log_f(LOG_INFO, "Successfully compiled fragment shader!");

  GLuint program = glCreateProgram(); gl();
  if (program == GL_FALSE){
    log_f(LOG_ERROR, "Failed to create shader program!");
    return -1;
  }
  log_f(LOG_INFO, "Successfully created shader program!");

  glAttachShader(program, vert); gl();
  glAttachShader(program, frag); gl();

  glLinkProgram(program); gl();

  int linked;
  glGetProgramiv(program, GL_LINK_STATUS, &linked); gl();
  if (linked == GL_FALSE){
    int infolog_len;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infolog_len); gl();

    char* infolog = (char*)malloc(sizeof(char)*infolog_len);

    glGetProgramInfoLog(program, infolog_len, &infolog_len, infolog); gl();

    log_f(LOG_ERROR, "Failed to link shader program: %s", infolog);

    free(infolog);
    return -1;
  }

  log_f(LOG_INFO, "Successfully linked shader program!");

  glDetachShader(program, vert); gl();
  glDetachShader(program, frag); gl();
  glDeleteShader(vert); gl();
  glDeleteShader(frag); gl();

  glValidateProgram(program);

  int validated;
  glGetProgramiv(program, GL_VALIDATE_STATUS, &validated);
  if (validated == GL_FALSE){
    int infolog_len;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infolog_len); gl();

    char* infolog = (char*)malloc(sizeof(char)*infolog_len);

    glGetProgramInfoLog(program, infolog_len, &infolog_len, infolog); gl();

    log_f(LOG_ERROR, "Failed to validate shader program: %s", infolog);

    free(infolog);
    return -1;
  }
  log_f(LOG_INFO, "Successfully validated shader program!");

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
