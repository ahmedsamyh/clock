#include <clock/clock.h>
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb/stb_truetype.h>

void scale_sprite(Sprite* spr, float scl) {
  spr->scale.x = scl;
  spr->scale.y = scl;
}

int main(void) {
  Context* ctx = (Context*)calloc(1, sizeof(Context));

  if (!clock_init(ctx, 800, 800, "GLFW Window")) return 1;
  const float width  = (float)ctx->win-> width;
  const float height = (float)ctx->win->height;

  float delta = ctx->delta;

  Sprite spr = {0};
  Sprite spr2 = {0};

  Texture* khu_tex = Resman_load_texture(ctx->resman, "resources/gfx/khu_sheet.png");

  if (!Sprite_init(&spr, khu_tex, 3, 1)) {
    return 1;
  }

  if (!Sprite_init(&spr2, khu_tex, 1, 1)) {
    return 1;
  }

  // game loop
  while (!clock_should_quit(ctx)){
    clock_begin_draw(ctx);

    clock_clear(ctx, COLOR_BLACK);

    spr2.pos = ctx->mpos;

    draw_sprite(ctx, &spr);
    draw_sprite(ctx, &spr2);

    clock_end_draw(ctx);
  }

  Sprite_deinit(&spr);
  Sprite_deinit(&spr2);
  clock_deinit(ctx);
  return 0;
}
