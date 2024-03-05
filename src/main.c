#include <clock/clock.h>

int main(void){
  Context ctx = {0};

  if (!clock_init(&ctx, 800, 800, "GLFW Window")) return 1;

  // vsync
  glfwSwapInterval(0);

  Sprite spr = {0};

  if (!Sprite_load(&spr, "resources/gfx/khu_sheet.png", 3, 1)) return 1;

  // game loop
  while (!clock_should_quit(&ctx)){
    clock_begin_draw(&ctx);

    clock_clear(&ctx, COLOR_BLACK);

    spr.pos = ctx.mpos;

    if (ctx.keys[GLFW_KEY_SPACE].just_pressed){
      log_f(LOG_INFO, "space just pressed");
    }

    Sprite_animate_hframe(&spr, ctx.delta);

    Render_sprite(ctx.ren, &spr);

    clock_end_draw(&ctx);
  }

  Sprite_deinit(&spr);
  clock_deinit(&ctx);
  return 0;
}
