#include <clock/clock.h>

int main(int argc, char* argv[]) {

  Context* ctx = clock_init(800, 800, 1.f, 1.f, "Clock");
  float width = ctx->win->width;
  float height = ctx->win->height;

  if (ctx == NULL) {
    return 1;
  }

  Font font = {0};

  Texture* atlas_tex = Resman_load_texture_uninitialized(ctx->resman, "resources/fonts/WayfarersToyBoxRegular-gxxER.ttf-atlas");

  if (!Font_load(&font, "resources/fonts/WayfarersToyBoxRegular-gxxER.ttf")) return 1;

  if (!Font_get_atlas_tex(&font, atlas_tex, 24)) return 1;

  Sprite spr = {0};
  if (!Sprite_init(&spr, atlas_tex, 1, 1)) return 1;

  spr.scale.x = 1.f;
  spr.scale.y = 1.f;
  /* Sprite_center_origin(&spr); */
  spr.pos.x = width/2.f;
  spr.pos.y = height/2.f;

  int codepoint = 33;

  while (!clock_should_quit(ctx)) {
    clock_begin_draw(ctx);
    clock_clear(ctx, COLOR_BLACK);

    const float R = 100.f;
    if (ctx->k[KEY_Z].pressed) {
      float scl = spr.scale.x;
      scl -= R * ctx->delta;
      if (scl <= 1.f) scl = 1.f;
      spr.scale.x = scl;
      spr.scale.y = scl;
    }

    if (ctx->k[KEY_X].pressed) {
      float scl = spr.scale.x;
      scl += R * ctx->delta;
      spr.scale.x = scl;
      spr.scale.y = scl;
    }

    /* spr.pos = ctx->mpos; */

    const float speed = 150.f;
    if (ctx->k[KEY_LEFT].held) {
      spr.pos.x -= speed * ctx->delta;
    }

    if (ctx->k[KEY_RIGHT].held) {
      spr.pos.x += speed * ctx->delta;
    }

    if (ctx->k[KEY_SPACE].pressed) {
      codepoint++;
      if (codepoint > 126) codepoint = 33;
      Codepoint_metrics m = Font_get_codepoint_metrics(&font, codepoint);
      spr.tex_rect = m.rect;
      spr.origin = m.offset;
    }

    if (ctx->k[KEY_UP].held) {
      spr.pos.y -= speed * ctx->delta;
    }

    if (ctx->k[KEY_DOWN].held) {
      spr.pos.y += speed * ctx->delta;
    }

    draw_sprite(ctx, &spr);

    draw_rect(ctx, (Rect){.pos = spr.pos, {1.f, 1.f}}, COLOR_RED);

    draw_rect(ctx, (Rect){.pos = spr.pos, spr.tex_rect.size}, color_alpha(COLOR_GREEN, 0.5f));

    clock_end_draw(ctx);
  }

  Font_unload(&font);
  /* Texture_deinit(&t); */

  clock_deinit(ctx);

  return 0;
}
