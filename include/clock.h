#ifndef _ENGINE_H_
#define _ENGINE_H_

#include <stdlib.h>
#include <commonlib.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <clock_math.h>
#include <vector.h>
#include <clock_vertex.h>

typedef uint32_t u32;
typedef uint8_t   u8;

#define TMPBUFF_SIZE (1024)
static char tmpbuff[TMPBUFF_SIZE] = {0};

#define DEFAULT_WIN_WIDTH 1280
#define DEFAULT_WIN_HEIGHT 720

// Color
typedef Vector4f Color; // 0.0..1.0

// Window
typedef struct {
  u32 width;
  u32 height;
  GLFWwindow* glfw_win;
  const char* title;
  double tp1;
  double tp2;
  double delta;
  int fps;
  Vector4d mpos;
} Window;

int Window_init(Window* win, u32 width, u32 height, const char* title);
void Window_begin_draw(Window* win);
void Window_end_draw(Window* win);
void Window_deinit(Window* win);
void Window_clear(Window* win, Color color);

#define COLOR_BLACK (Color){0.f,0.f,0.f,1.f}
#define COLOR_WHITE (Color){1.f,1.f,1.f,1.f}
#define COLOR_RED   (Color){1.f,0.f,0.f,1.f}
#define COLOR_GREEN (Color){0.f,1.f,0.f,1.f}
#define COLOR_BLUE (Color){0.f,0.f,1.f,1.f}


// Renderer
typedef struct {
#define VERTEX_CAP (16)
  Vertex vertices[VERTEX_CAP];
#define VAO_COUNT 1
  GLuint vao[VAO_COUNT];
#define VBO_COUNT 1
  GLuint vbo[VBO_COUNT];
  GLuint current_shader;
  Window* win;
} Renderer;

int Renderer_init(Renderer* renderer, Window* win);
void Renderer_deinit(Renderer* renderer);
void Render_imm_triangle(Renderer* renderer, Vector3f p0, Vector3f p1, Vector3f p2, Color c0, Color c1, Color c2);
void Render_imm_quad(Renderer* renderer, Vector3f p0, Vector3f p1, Vector3f p2, Vector3f p3, Color c0, Color c1, Color c2, Color c3);
void Render_imm_box(Renderer* renderer, Vector3f p0, Vector3f p1, Vector3f p2, Vector3f p3, Color c0, Color c1, Color c2, Color c3);

// Shader
static const char* default_vert_shader =
  "#version 460\n"
  "layout(location = 0)in vec4 position;\n"
  "layout(location = 1)in vec4 color;\n"
  "layout(location = 2)out vec4 out_color;\n"
  "void main(void){\n"
  "  gl_Position = position;\n"
  "  out_color = color;\n"
  "}\n";

static const char* default_frag_shader =
  "#version 460\n"
  "layout(location = 2)in vec4 in_col;\n"
  "out vec4 frag_col;\n"
  "void main(void){\n"
  "  frag_col = in_col;\n"
  "}\n";

int create_shader(const char* vert_src, const char* frag_src);

// Utility
const char* gl_error_as_cstr(int e);
void gl_check_and_log_error(int line);
#define gl() gl_check_and_log_error(__LINE__)

#endif /* _ENGINE_H_ */
