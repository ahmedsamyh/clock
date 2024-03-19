#ifndef _CLOCK_CORE_H_
#define _CLOCK_CORE_H_

// Window
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <clock/clock_vertex.h>
#include <clock/clock_vector.h>
#include <clock/clock_rect.h>
#include <clock/clock_sprite.h>
#include <clock/clock_render_target.h>
#include <clock/clock_key.h>
#include <clock/clock_resource_manager.h>
#include <stdbool.h>
#include <commonlib.h>

#define DEFAULT_WIN_WIDTH 1280
#define DEFAULT_WIN_HEIGHT 720

typedef struct Context Context;
typedef struct Window Window;
typedef struct Renderer Renderer;
typedef struct Render_target Render_target;
typedef enum   Blendmode Blendmode;

// Color
typedef Vector4f Color; // 0.0..1.0

#define COLOR_BLACK (Color){0.f,0.f,0.f,1.f}
#define COLOR_WHITE (Color){1.f,1.f,1.f,1.f}
#define COLOR_RED   (Color){1.f,0.f,0.f,1.f}
#define COLOR_GREEN (Color){0.f,1.f,0.f,1.f}
#define COLOR_BLUE (Color){0.f,0.f,1.f,1.f}

// hex should be in format 0xAAGGBBRR
Color hex_to_color(int color);

// Window
struct Window {
  unsigned int width;
  unsigned int height;
  float scale_x, scale_y;
  GLFWwindow* glfw_win;
  const char* title;
};

bool Window_init(Window* win, unsigned int width, unsigned int height, float scl_x, float scl_y, const char* title);
void Window_deinit(Window* win);

// Context / main user api

#define KEYS_COUNT (GLFW_KEY_LAST+GLFW_KEY_SPACE)

struct Context {
  Window*   win;
  Renderer* ren;
  double    tp1;
  double    tp2;
  double    delta;
  int       fps;
  Vector2f  mpos;
  Key keys[KEYS_COUNT];
  Resource_manager* resman;
  int       ren_tex_image_slot;
#define TMP_BUFF_SIZE (1024)
  char tmpbuff[TMP_BUFF_SIZE];
};

Context* clock_init(unsigned int window_width, unsigned int window_height, float scl_x, float scl_y, const char* title);
bool clock_should_quit(Context* ctx);
void clock_update_keys(Context* ctx);
void clock_begin_draw(Context* ctx);
void clock_end_draw(Context* ctx);
void clock_flush_draw(Context *ctx);
void clock_clear(Context* ctx, Color color);
void clock_deinit(Context* ctx);

void clock_set_vsync(bool enable);

void clock_begin_scissor(Context* ctx, Rect rect);
void clock_endn_scissor(Context* ctx);

// Callbacks

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

// Renderer

typedef enum {
  TEXTURE_SHADER,
  COLOR_SHADER,
  CUSTOM_SHADER,
} SET_SHADER;

struct Renderer {
#define VERTEX_CAP (16)
  Vertex vertices[VERTEX_CAP];
  GLuint vao;
  GLuint vbo;
  GLuint current_shader; // currently using shader do not set manually
  GLuint custom_shader;
  GLuint texture_shader;
  GLuint color_shader;
  Render_target* ren_tex;
  Window* win;
};

bool Renderer_init(Renderer* renderer, Window* win);
void Renderer_deinit(Renderer* renderer);
void Renderer_use_custom_shader(Renderer* renderer, const char* vs, const char* fs);
void Renderer_use_color_shader(Renderer* renderer);
void Renderer_use_texture_shader(Renderer* renderer);
void Renderer_set_render_target(Renderer* renderer, GLuint target);
void draw_imm_triangle(Context* ctx, Vector3f p0, Vector3f p1, Vector3f p2, Color c0, Color c1, Color c2);
void draw_imm_quad(Context* ctx, Vector3f p0, Vector3f p1, Vector3f p2, Vector3f p3, Color c0, Color c1, Color c2, Color c3);
void draw_imm_box(Context* ctx, Vector3f p0, Vector3f p1, Vector3f p2, Vector3f p3, Color c0, Color c1, Color c2, Color c3);
void draw_texture(Context* ctx, Vector3f pos, Rect texcoords, Texture* texture);
void draw_sprite(Context* ctx, Sprite* spr);
void draw_rect(Context* ctx, Rect rect, Color color);
void draw_imm_line(Context* ctx, Vector3f p0, Vector3f p1, Color c0, Color c1);
void draw_rect_centered(Context* ctx, Rect rect, Color col);

// Blendmode

enum Blendmode {
  BLENDMODE_NORMAL = 0, // default alpha blending
  BLENDMODE_ALPHA  = BLENDMODE_NORMAL,
  BLENDMODE_ADD,        // additive
  BLENDMODE_SUB,        // subtractive
  BLENDMODE_MUL,        // multiply
  BLENDMODE_NONE,       // no blending

  BLENDMODE_COUNT
};

void set_blend_mode(const Blendmode mode);

// Shader

static const char* color_vert_shader =
  "#version 460\n"
  "layout(location = 0)in vec4 position;\n"
  "layout(location = 1)in vec4 color;\n"
  "uniform mat4 model;\n"
  "uniform mat4 proj;\n"
  "out vec4 v_col;\n"
  "void main(void){\n"
  "  vec4 new_pos = proj * model * position;\n"
  "  new_pos.y *= -1.f;\n"
  "  gl_Position = new_pos;\n"
  "  v_col = color;\n"
  "}\n";

static const char* color_frag_shader =
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
  "uniform mat4 model;\n"
  "uniform mat4 proj;\n"
  "out vec2 v_texcoord;\n"
  "out vec4 v_col;\n"
  "void main(void){\n"
  "  vec4 new_pos = proj * model * position;\n"
  "  new_pos.y   *= -1.f;\n"
  "  gl_Position  = new_pos;\n"
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
  "  frag_col = texture(tex, v_texcoord);\n"
  "}\n";


int create_shader(const char* vert_src, const char* frag_src);

// Utility

const char* gl_error_as_cstr(int e);
void gl_check_and_log_error(const char* file, int line);
#define gl(...) __VA_ARGS__; gl_check_and_log_error(__FILE__, __LINE__);

#endif /* _CLOCK_CORE_H_ */
