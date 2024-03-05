#include <clock/clock.h>

bool left, right, up, down;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
  /* log_f(LOG_INFO, "Key: %d | action: %d", key, action); */

  if (action == GLFW_PRESS){
    switch(key){
    case GLFW_KEY_D: right = true; break;
    case GLFW_KEY_A: left  = true; break;
    case GLFW_KEY_W: up    = true; break;
    case GLFW_KEY_S: down  = true; break;
    }
  }

  if (action == GLFW_RELEASE){
    switch(key){
    case GLFW_KEY_D: right = false; break;
    case GLFW_KEY_A: left  = false; break;
    case GLFW_KEY_W: up    = false; break;
    case GLFW_KEY_S: down  = false; break;
    }
  }
}


int main(void){
  Window win = {0};

  if (Window_init(&win, 800, 800, "GLFW Window") < 0) return 1;
  glfwSetKeyCallback(win.glfw_win, key_callback);

  float width =  (float)win.width;
  float height = (float)win.height;

  Renderer ren = {0};

  if (Renderer_init(&ren, &win) < 0) return 1;

  // vsync
  glfwSwapInterval(0);

  Sprite spr = {0};
  if (!Sprite_load(&spr, "resources/gfx/khu_sheet.png", 3, 1)) return 1;

  Vector2f vel = {0};
  const float speed = 130.f;

  // game loop
  while (!glfwWindowShouldClose(win.glfw_win)){
    Window_begin_draw(&win);

    Window_clear(&win, COLOR_BLACK);

    Vector2f dir = {0};
    if (left){
      dir.x--;
    }
    if (right){
      dir.x++;
    }
    if (up){
      dir.y--;
    }
    if (down){
      dir.y++;
    }

    dir = v2f_normalize(dir);

    vel = v2f_muls(v2f_add(v2f_muls(dir, speed), vel), win.delta);

    spr.pos = v2f_add(spr.pos, vel);

    Sprite_animate_hframe(&spr, win.delta);
    Render_sprite(&ren, &spr);

    Window_end_draw(&win);
  }

  Sprite_deinit(&spr);
  Renderer_deinit(&ren);
  Window_deinit(&win);
  return 0;
}
