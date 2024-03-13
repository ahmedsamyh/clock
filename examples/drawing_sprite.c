#include <clock/clock.h>
#include <stdlib.h>

int main(void) {
  Context* ctx = (Context*)calloc(1, sizeof(Context));

  if (!clock_init(ctx, 800, 800, "Drawing Sprites")) {
    return 1;
  }

  Sprite spr = {0};

  // TODO: Sprite_load will change
  if (!Sprite_load(&spr, "resources/gfx/khu_sheet.png", 3, 1)) {
    return 1;
  }

  Sprite_center_origin(&spr);

  spr.scale.x = 4.f;
  spr.scale.y = 4.f;


  while (!clock_should_quit(ctx)) {
    float delta = ctx->delta;
    clock_begin_draw(ctx);

    clock_clear(ctx, COLOR_BLACK);

   // rotation
    if (ctx->keys[GLFW_KEY_Z].held) spr.rotation.x += 100.f * delta;
    if (ctx->keys[GLFW_KEY_X].held) spr.rotation.y += 100.f * delta;
    if (ctx->keys[GLFW_KEY_C].held) spr.rotation.z += 100.f * delta;

    // scaling
    const float S = 10.f;
    const float scale_min = 0.35f;
    if (ctx->keys[GLFW_KEY_A].held) {
      spr.scale.x = spr.scale.x <= scale_min ? scale_min : spr.scale.x - S * delta;
      spr.scale.y = spr.scale.y <= scale_min ? scale_min : spr.scale.y - S * delta;
    }
    if (ctx->keys[GLFW_KEY_S].held) {
      spr.scale.x += S * delta;
      spr.scale.y += S * delta;
    }

    spr.pos = ctx->mpos;

    Sprite_animate_hframe(&spr, delta);

    draw_sprite(ctx, &spr);

    clock_end_draw(ctx);
  }

  Sprite_deinit(&spr);
  clock_deinit(ctx);
  return 0;
}
