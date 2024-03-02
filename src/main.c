#include <math.h>
#include <clock.h>
#define VECTOR_IMPLEMENTATION
#include <vector.h>
#define COMMONLIB_IMPLEMENTATION
#include <commonlib.h>
#include <stddef.h> // offsetof

int main(void){
  Window win = {0};

  if (Window_init(&win, 800, 800, "GLFW Window") < 0) return 1;

  float width =  (float)win.width;
  float height = (float)win.height;

  Renderer ren = {0};

  if (Renderer_init(&ren, &win) < 0) return 1;

  // game loop
  while (!glfwWindowShouldClose(win.glfw_win)){
    Window_clear(&win, COLOR_BLACK);

    Vector3f p0 = {0.f, height, 0.f};
    Vector3f p1 = {width*0.5f, 0.f, 0.f};
    Vector3f p2 = {width, height, 0.f};

    /* Vector3f p0 = {-0.5f, -0.5f, 0.f}; */
    /* Vector3f p1 = { 0.0f,  0.5f, 0.f}; */
    /* Vector3f p2 = { 0.5f, -0.5f, 0.f}; */

    Render_draw_triangle3(&ren, p0, p1, p2,
			   COLOR_RED,
			   COLOR_GREEN,
			   COLOR_BLUE);
    /* quit(window, 0); */

    Window_display(&win);
  }

  Renderer_deinit(&ren);
  Window_deinit(&win);

  return 0;
}
