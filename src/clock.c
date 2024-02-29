#include <clock.h>

// Main
void begin_draw(void){
  glfwPollEvents();
}

void end_draw(void){
  glfwSwapBuffers(window);
}

// Color
Colorf Color_normalize(Color color){
  return (Colorf){
    .r = (float)color.r / 255.f,
    .g = (float)color.g / 255.f,
    .b = (float)color.b / 255.f,
    .a = (float)color.a / 255.f,
  };
}
void clear(Color color){
  clearf(Color_normalize(color));
}
void clearf(Colorf colorf){
  glClearColor(colorf.r, colorf.g, colorf.b, colorf.a); gl_check_and_log_error();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); gl_check_and_log_error();
}

int init_window(u32 _width, u32 _height, const char* _title){
  win_title = _title;

  win_width = _width == 0 ? DEFAULT_WIN_WIDTH : _width;
  win_height = _height == 0 ? DEFAULT_WIN_HEIGHT : _height;

  log_f(LOG_INFO, "Running %s", win_title);
  if (!glfwInit()){
    log_f(LOG_ERROR, "GLFW Could not initialize!");
    return -1;
  }
  log_f(LOG_INFO, "GLFW initialized!");

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  window = glfwCreateWindow(win_width, win_height, win_title, NULL, NULL);

  if (window == NULL){
    glfwGetError((const char**)&tmpbuff);
    log_f(LOG_ERROR, "Could not initialize window: %s", tmpbuff);
    return -1;
  }
  log_f(LOG_INFO, "Window Created!");
  glfwMakeContextCurrent(window);

  int version = gladLoadGL(glfwGetProcAddress);

  if (version == 0){
    log_f(LOG_ERROR, "Failed to initialize GLAD Context!");
    return -1;
  }

  log_f(LOG_INFO, "Loaded OpenGL %d.%d!", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));

  return 0;
}

void quit(GLFWwindow* window, int exit_code){
  glfwDestroyWindow(window);
  log_f(LOG_INFO, "Window Destroyed!");
  glfwTerminate();
  log_f(LOG_INFO, "GLFW Terminated!");
  exit(exit_code);
}

// Shader
int create_shader(const char* vert_src, const char* frag_src){
  GLuint vert = glCreateShader(GL_VERTEX_SHADER); gl_check_and_log_error();
  if (!vert){
    log_f(LOG_ERROR, "Failed to create vertex shader!");
    return -1;
  }
  log_f(LOG_INFO, "Successfully created vertex shader!");

  GLuint frag = glCreateShader(GL_FRAGMENT_SHADER); gl_check_and_log_error();
  if (!frag){
    log_f(LOG_ERROR, "Failed to create fragment shader!");
    return -1;
  }
  log_f(LOG_INFO, "Successfully created fragment shader!");

  glShaderSource(vert, 1, (const GLchar**)&vert_src, NULL); gl_check_and_log_error();
  glShaderSource(frag, 1, (const GLchar**)&frag_src, NULL); gl_check_and_log_error();

  glCompileShader(vert); gl_check_and_log_error();
  int compiled;
  glGetShaderiv(vert, GL_COMPILE_STATUS, &compiled); gl_check_and_log_error();
  if (!compiled){
    int infolog_len;
    glGetShaderiv(vert, GL_INFO_LOG_LENGTH, &infolog_len); gl_check_and_log_error();

    char* infolog = (char*)malloc(sizeof(char)*infolog_len);

    glGetShaderInfoLog(vert, infolog_len, &infolog_len, infolog); gl_check_and_log_error();

    log_f(LOG_ERROR, "Failed to compile vertex shader: %s", infolog);

    free(infolog);
    return -1;
  }
  log_f(LOG_INFO, "Successfully compiled vertex shader!");

  glCompileShader(frag); gl_check_and_log_error();
  glGetShaderiv(frag, GL_COMPILE_STATUS, &compiled); gl_check_and_log_error();
  if (!compiled){
    int infolog_len;
    glGetShaderiv(frag, GL_INFO_LOG_LENGTH, &infolog_len); gl_check_and_log_error();

    char* infolog = (char*)malloc(sizeof(char)*infolog_len);

    glGetShaderInfoLog(frag, infolog_len, &infolog_len, infolog); gl_check_and_log_error();

    log_f(LOG_ERROR, "Failed to compile fragment shader: %s", infolog);

    free(infolog);
    return -1;
  }
  log_f(LOG_INFO, "Successfully compiled fragment shader!");

  GLuint program = glCreateProgram(); gl_check_and_log_error();
  if (!program){
    log_f(LOG_ERROR, "Failed to create shader program!");
    return -1;
  }
  log_f(LOG_INFO, "Successfully created shader program!");

  glAttachShader(program, vert); gl_check_and_log_error();
  glAttachShader(program, frag); gl_check_and_log_error();

  glLinkProgram(program); gl_check_and_log_error();

  int linked;
  glGetProgramiv(program, GL_LINK_STATUS, &linked); gl_check_and_log_error();
  if (!linked){
    int infolog_len;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infolog_len); gl_check_and_log_error();

    char* infolog = (char*)malloc(sizeof(char)*infolog_len);

    glGetProgramInfoLog(program, infolog_len, &infolog_len, infolog); gl_check_and_log_error();

    log_f(LOG_ERROR, "Failed to link shader program: %s", infolog);

    free(infolog);
    return -1;
  }
  log_f(LOG_INFO, "Successfully linked shader program!");

  glDetachShader(program, vert); gl_check_and_log_error();
  glDetachShader(program, frag); gl_check_and_log_error();
  glDeleteShader(vert); gl_check_and_log_error();
  glDeleteShader(frag); gl_check_and_log_error();

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

void gl_check_and_log_error(void){
  int e = glGetError();

  if (e != GL_NO_ERROR){
    log_f(LOG_ERROR, "GL error: %s", gl_error_as_cstr(e));
    quit(window, 1);
  }

}
