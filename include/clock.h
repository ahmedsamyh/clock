#ifndef _ENGINE_H_
#define _ENGINE_H_

#include <stdlib.h>
#include <commonlib.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <clock_math.h>

typedef uint32_t u32;
typedef uint8_t   u8;

#define TMP_BUFF_SIZE (1024)
static char tmpbuff[TMP_BUFF_SIZE] = {0};

#define DEFAULT_WIN_WIDTH 1280
#define DEFAULT_WIN_HEIGHT 720

static u32 win_width;
static u32 win_height;
GLFWwindow* window;
static const char* win_title;

// Main
int  clock_init_window(u32 width, u32 height, const char* title);
void clock_begin_draw(void);
void clock_end_draw(void);

void clock_quit(GLFWwindow* window, int exit_code);

// Color
typedef struct {
  u8 r, g, b, a; // 0..255
} Color;
typedef struct {
  float r, g, b, a; // 0.0..1.0
} Colorf;

Colorf Color_normalize(Color color);
void clear(Color color);
void clearf(Colorf colorf);

#define COLOR_BLACK (Color){0,0,0,255}
#define COLOR_RED   (Color){255,0,0,255}
#define COLOR_GREEN (Color){0,255,0,255}
#define COLOR_BLUE (Color){0,0,255,255}

// Shader
static const char* default_vert_shader =
  "#version 460\n"
  "layout(location = 0)in vec4 position;\n"
  "void main(void){\n"
  "  gl_Position = position;\n"
  "}\n";

static const char* default_frag_shader =
  "#version 460\n"
  "out vec4 frag_col;"
  "void main(void){\n"
  "  frag_col = vec4(1.0);\n"
  "}\n";

int create_shader(const char* vert_src, const char* frag_src);

// Utility
const char* gl_error_as_cstr(int e);
void gl_check_and_log_error(void);

#endif /* _ENGINE_H_ */
