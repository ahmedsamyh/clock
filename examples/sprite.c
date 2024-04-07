#include <clock/clock.h>
#include <stdlib.h>

int main(void) {
  Context* ctx = clock_init(800, 800, 1.f, 1.f, "Drawing Sprites", RENDER_MODE_2D);

  if (ctx == NULL) {
    return 1;
  }

  Sprite spr = {0};

  Texture* khu_tex = Resman_load_texture_from_file(ctx->resman, "resources/gfx/khu_sheet.png");

  if (!Sprite_init(&spr, Resman_load_texture_from_file(ctx->resman, "resources/gfx/flower.png"), 1, 1)) {
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
    if (clock_key_held(KEY_Z)) spr.rotation.x += 100.f * delta;
    if (clock_key_held(KEY_X)) spr.rotation.y += 100.f * delta;
    if (clock_key_held(KEY_C)) spr.rotation.z += 100.f * delta;

    // scaling
    const float S = 10.f;
    const float scale_min = 0.35f;
    if (clock_key_held(KEY_A)) {
      spr.scale.x = spr.scale.x <= scale_min ? scale_min : spr.scale.x - S * delta;
      spr.scale.y = spr.scale.y <= scale_min ? scale_min : spr.scale.y - S * delta;
    }
    if (clock_key_held(KEY_S)) {
      spr.scale.x += S * delta;
      spr.scale.y += S * delta;
    }

    // move camera
    const float speed = 400.f;
    if (clock_key_held(KEY_UP)) {
      ctx->camera.y += speed * ctx->delta;
    }
    if (clock_key_held(KEY_DOWN)) {
      ctx->camera.y -= speed * ctx->delta;
    }
    if (clock_key_held(KEY_RIGHT)) {
      ctx->camera.x += speed * ctx->delta;
    }
    if (clock_key_held(KEY_LEFT)) {
      ctx->camera.x -= speed * ctx->delta;
    }

    Sprite_animate_hframe(&spr, delta);

    spr.pos = ctx->mpos;
    draw_sprite(ctx, &spr);

    clock_end_draw(ctx);
  }

  Sprite_deinit(&spr);
  clock_deinit(ctx);
  return 0;
}
