#include <clock/clock.h>

int main(void){
  Window win = {0};

  if (Window_init(&win, 800, 800, "GLFW Window") < 0) return 1;

  float width =  (float)win.width;
  float height = (float)win.height;

  Renderer ren = {0};

  if (Renderer_init(&ren, &win) < 0) return 1;

  if (!Renderer_set_shader(&ren, tex_vert_shader, tex_frag_shader)) return 1;

  float deg = 0.f;
  float scl = 1.f;
  float a = 0.f;

  // vsync
  glfwSwapInterval(0);

  const size_t num_quads = 150;
  const float w = 100.f;
  const float h = 100.f;

  Texture t = {0};

  if (!Texture_load_from_file(&t, "resources/gfx/khu_sheet.png")) return 1;

  // game loop
  while (!glfwWindowShouldClose(win.glfw_win)){
    Window_begin_draw(&win);

    Window_clear(&win, COLOR_BLACK);

    Vector3f mpos = (Vector3f){win.mpos.x, win.mpos.y, win.mpos.z};

    Render_texture(&ren, mpos, &t);

    Window_end_draw(&win);
  }

  Texture_deinit(&t);
  Renderer_deinit(&ren);
  Window_deinit(&win);
  return 0;
}
