#include <clock/clock.h>

int main(void){
  Window win = {0};

  if (Window_init(&win, 800, 800, "GLFW Window") < 0) return 1;

  float width =  (float)win.width;
  float height = (float)win.height;

  Renderer ren = {0};

  if (Renderer_init(&ren, &win) < 0) return 1;

  // vsync
  glfwSwapInterval(0);

  Rect rect = {
    .pos = {50.f, 100.f},
    .size = {100.f, 100.f}
  };

  Rect rect2 = {
    .pos = {0.f, 0.f},
    .size = {75.f, 80.f}
  };

  // game loop
  while (!glfwWindowShouldClose(win.glfw_win)){
    Window_begin_draw(&win);

    Window_clear(&win, COLOR_BLACK);

    Vector2f mpos = (Vector2f){win.mpos.x, win.mpos.y};
    rect2.pos = mpos;

    Render_rect(&ren, rect, Rect_contains_rect(rect, rect2) ? COLOR_GREEN : Rect_intersects_rect(rect, rect2) ? COLOR_WHITE : COLOR_RED);
    Render_rect(&ren, rect2, COLOR_WHITE);

    Window_end_draw(&win);
  }

  /* Texture_deinit(&t); */
  Renderer_deinit(&ren);
  Window_deinit(&win);
  return 0;
}
