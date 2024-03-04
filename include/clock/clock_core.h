#ifndef _CLOCK_CORE_H_
#define _CLOCK_CORE_H_

// Window
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <clock/clock_vertex.h>
#include <clock/clock_vector.h>
#include <stdbool.h>
#include <clock/clock_texture.h>
#include <commonlib.h>

#define DEFAULT_WIN_WIDTH 1280
#define DEFAULT_WIN_HEIGHT 720


// Color
typedef Vector4f Color; // 0.0..1.0

#define COLOR_BLACK (Color){0.f,0.f,0.f,1.f}
#define COLOR_WHITE (Color){1.f,1.f,1.f,1.f}
#define COLOR_RED   (Color){1.f,0.f,0.f,1.f}
#define COLOR_GREEN (Color){0.f,1.f,0.f,1.f}
#define COLOR_BLUE (Color){0.f,0.f,1.f,1.f}

// Window
typedef struct {
  unsigned int width;
  unsigned int height;
  GLFWwindow* glfw_win;
  const char* title;
  double tp1;
  double tp2;
  double delta;
  int fps;
  Vector4d mpos;
} Window;

int Window_init(Window* win, unsigned int width, unsigned int height, const char* title);
void Window_begin_draw(Window* win);
void Window_end_draw(Window* win);
void Window_deinit(Window* win);
void Window_clear(Window* win, Color color);


// Renderer

typedef struct {
#define VERTEX_CAP (16)
  Vertex vertices[VERTEX_CAP];
#define VAO_COUNT 1
  GLuint vao[VAO_COUNT];
#define VBO_COUNT 1
  GLuint vbo[VBO_COUNT];
  GLuint shader;
  Window* win;
} Renderer;

int Renderer_init(Renderer* renderer, Window* win);
void Renderer_deinit(Renderer* renderer);
bool Renderer_set_shader(Renderer* renderer, const char* vs, const char* fs);
void Render_imm_triangle(Renderer* renderer, Vector3f p0, Vector3f p1, Vector3f p2, Color c0, Color c1, Color c2);
void Render_imm_quad(Renderer* renderer, Vector3f p0, Vector3f p1, Vector3f p2, Vector3f p3, Color c0, Color c1, Color c2, Color c3);
void Render_imm_box(Renderer* renderer, Vector3f p0, Vector3f p1, Vector3f p2, Vector3f p3, Color c0, Color c1, Color c2, Color c3);
void Render_texture(Renderer* renderer, Vector3f pos, Texture* texture);

// Shader

static const char* default_vert_shader =
  "#version 460\n"
  "layout(location = 0)in vec4 position;\n"
  "layout(location = 1)in vec4 color;\n"
  "out vec4 v_col;\n"
  "void main(void){\n"
  "  gl_Position = position;\n"
  "  v_col = color;\n"
  "}\n";

static const char* default_frag_shader =
  "#version 460\n"
  "in vec4 v_col;\n"
  "out vec4 frag_col;\n"
  "void main(void){\n"
  "  frag_col = v_col;\n"
  "}\n";

static const char* tex_vert_shader =
  "#version 460\n"
  "layout(location = 0)in vec4 position;\n"
  "layout(location = 1)in vec4 color;\n"
  "layout(location = 2)in vec2 texcoord;\n"
  "out vec2 v_texcoord;\n"
  "out vec4 v_col;\n"
  "void main(void){\n"
  "  gl_Position = position;\n"
  "  v_col = color;\n"
  "  v_texcoord = texcoord;\n"
  "}\n";

static const char* tex_frag_shader =
  "#version 460\n"
  "in vec4 v_col;\n"
  "in vec2 v_texcoord;\n"
  "uniform sampler2D tex;\n"
  "out vec4 frag_col;\n"
  "void main(void){\n"
  "  frag_col = texture(tex, v_texcoord);//vec4(v_texcoord, 0.0, 1.0);\n"
  "}\n";


int create_shader(const char* vert_src, const char* frag_src);

// Utility

const char* gl_error_as_cstr(int e);
void gl_check_and_log_error(int line);
#define gl(...) __VA_ARGS__; gl_check_and_log_error(__LINE__);

#endif /* _CLOCK_CORE_H_ */
