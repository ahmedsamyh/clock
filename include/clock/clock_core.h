#ifndef _CLOCK_CORE_H_
#define _CLOCK_CORE_H_

// Window
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <clock/clock_vertex.h>
#include <clock/clock_vector.h>
#include <clock/clock_matrix.h>
#include <clock/clock_rect.h>
#include <clock/clock_sprite.h>
#include <clock/clock_render_target.h>
#include <clock/clock_key.h>
#include <clock/clock_mouse.h>
#include <clock/clock_color.h>
#include <clock/clock_font.h>
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

// Window
struct Window {
  unsigned int width;
  unsigned int height;
  float scale_x, scale_y;
  GLFWwindow* glfw_win;
  const char* title;
};

// Flags
#define WINDOW_RESIZABLE        (1<<0)
#define WINDOW_RESIZABLE_ASPECT (1<<1)
#define WINDOW_VSYNC            (1<<2)
#define RENDER_3D               (1<<3)

bool Window_init(Window* win, unsigned int width, unsigned int height, Vector2f scale, const char* title, uint32 flags);
void Window_deinit(Window* win);

// Context / main user api

struct Context {
  Window*   win;
  Renderer* ren;
  double    tp1;
  double    tp2;
  float     delta;
  int       fps;
  Vector2f  prev_mpos, mpos;
  Vector2f  mscroll;
  Vector3f  camera;
  bool      use_camera_view;
  Key_KV*   key_map; // hashmap
  int*      key_codes; // dynamic-array; TODO (speed): remove when i know how to iterate over a hashmap;
  Mouse     m[MOUSE_BUTTONS_COUNT];
  Resource_manager* resman;
  int       ren_tex_image_slot;
#define TMP_BUFF_SIZE (1024)
  char tmpbuff[TMP_BUFF_SIZE];
  uint32 last_entered_character; // text input
  bool text_entered;
  bool key_input_handled;
  bool mouse_input_handled;
};

Context* clock_init(unsigned int window_width, unsigned int window_height, float window_scale_x, float window_scale_y, const char* title, uint32 flags);
bool clock_should_quit(Context* ctx);
void clock_update_mouse(Context* ctx);
void clock_begin_draw(Context* ctx);
void clock_end_draw(Context* ctx);
void clock_flush_draw(Context *ctx);
void clock_clear(Context* ctx, Color color);
void clock_deinit(Context* ctx);
Vector2f clock_mpos_world(Context* ctx);
Vector2f clock_mpos_screen(Context* ctx);
void clock_use_camera_view(Context* ctx, bool use);
Vector3f clock_screen_to_world_3d(Context* ctx, Vector3f pos);
Vector2f clock_screen_to_world(Context* ctx, Vector2f pos);
Vector3f clock_world_to_screen_3d(Context* ctx, Vector3f pos);
Vector2f clock_world_to_screen(Context* ctx, Vector2f pos);

void clock_begin_scissor(Context* ctx, Rect rect);
void clock_end_scissor(Context* ctx);

// Input
void clock_init_keys(Context* ctx);
void clock_update_keys(Context* ctx);
void clock_eat_key_input(Context* ctx);
void clock_eat_mouse_input(Context* ctx);
void clock_eat_input(Context* ctx);
// !!!IMPORTANT!!!: User must not access the Context::k[] array manually for key input, rather must use these functions!!!
bool clock_key_state(Context* ctx, int key, Key_state state);
bool clock_key_pressed(Context* ctx, int key);
bool clock_key_just_pressed(Context* ctx, int key);
bool clock_key_released(Context* ctx, int key);
bool clock_key_held(Context* ctx, int key);
Key  clock_get_key_by_code(Context* ctx, int key);

// !!!IMPORTANT!!!: User must not access the Context::m[] array manually for mouse input, rather must use these functions!!!
bool clock_mouse_state(Context* ctx, int button, Mouse_state state);
bool clock_mouse_pressed(Context* ctx, int button);
bool clock_mouse_released(Context* ctx, int button);
bool clock_mouse_held(Context* ctx, int button);

// Misc (Context agnostic)
void set_vsync(bool enable);
cstr get_clipboard(void);
void set_clipboard(cstr text);

// Callbacks

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void text_callback(GLFWwindow* window, uint32 key_code);
void mouse_scroll_callback(GLFWwindow* window, real64 xoffset, real64 yoffset);
void window_resize_callback(GLFWwindow* window, int width, int height);
void framebuffer_resize_callback(GLFWwindow* window, int width, int height);
void error_callback(int error_code, cstr description);

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
  Matrix4 proj;
  bool render_3D;
};

bool Renderer_init(Renderer* renderer, Window* win, uint32 flags);
void Renderer_deinit(Renderer* renderer);
void Renderer_use_custom_shader(Renderer* renderer, const char* vs, const char* fs);
void Renderer_use_color_shader(Renderer* renderer);
void Renderer_use_texture_shader(Renderer* renderer);
void Renderer_set_render_target(Renderer* renderer, GLuint target);
void draw_imm_triangle_3d(Context* ctx, Vector3f p0, Vector3f p1, Vector3f p2, Color c0, Color c1, Color c2);
void draw_imm_triangle(Context* ctx, Vector2f p0, Vector2f p1, Vector2f p2, Color c0, Color c1, Color c2);
void draw_imm_quad_3d(Context* ctx, Vector3f p0, Vector3f p1, Vector3f p2, Vector3f p3, Color c0, Color c1, Color c2, Color c3);
void draw_imm_quad(Context* ctx, Vector2f p0, Vector2f p1, Vector2f p2, Vector2f p3, Color c0, Color c1, Color c2, Color c3);
void draw_imm_box_2d(Context* ctx, Vector3f p0, Vector3f p1, Vector3f p2, Vector3f p3, Color c0, Color c1, Color c2, Color c3);
void draw_imm_box(Context* ctx, Vector2f p0, Vector2f p1, Vector2f p2, Vector2f p3, Color c0, Color c1, Color c2, Color c3);
void draw_sprite(Context* ctx, Sprite* spr);
void draw_sprite_at(Context* ctx, Sprite* spr, Vector2f pos);
void draw_rect(Context* ctx, Rect rect, Color color);
void draw_rect_centered(Context* ctx, Rect rect, Color col);
void draw_box(Context* ctx, Rect rect, Color out_color, Color fill_color);
void draw_imm_line3d(Context* ctx, Vector3f p0, Vector3f p1, Color c0, Color c1);
void draw_imm_line(Context* ctx, Vector2f p0, Vector2f p1, Color c0, Color c1);
void draw_point_3d(Context* ctx, Vector3f p, Color col);
void draw_point(Context* ctx, Vector2f p, Color col);
// TODO: do we want a 3d version for draw_text()?
void draw_text(Context* ctx, Font* font, cstr text, Vector2f pos, int char_size, Color color);

Vector2f get_text_size(Context* ctx, Font* font, cstr text, int char_size);
Vector2f get_text_sizen(Context* ctx, Font* font, char* text, uint32 text_size, int char_size);

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
  "uniform mat4 view;\n"
  "uniform mat4 proj;\n"
  "out vec4 v_col;\n"
  "void main(void){\n"
  "  vec4 new_pos = proj * view * model * position;\n"
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
  "uniform mat4 view;\n"
  "uniform mat4 proj;\n"
  "out vec2 v_texcoord;\n"
  "out vec4 v_col;\n"
  "void main(void){\n"
  "  vec4 new_pos = proj * view * model * position;\n"
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
  "uniform vec4 tex_tint;\n"
  "out vec4 frag_col;\n"
  "void main(void){\n"
  "  frag_col = texture(tex, v_texcoord) * tex_tint;\n"
  "}\n";


int create_shader(const char* vert_src, const char* frag_src);

// Utility

const char* gl_error_as_cstr(int e);
void gl_check_and_log_error(const char* file, int line);
#define gl(...) __VA_ARGS__; gl_check_and_log_error(__FILE__, __LINE__);

#endif /* _CLOCK_CORE_H_ */
