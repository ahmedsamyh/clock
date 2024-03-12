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
  const float width  = (float)ctx->win->width;
  const float height = (float)ctx->win->height;

  /*  stbtt_fontinfo font = {0};

  unsigned char* data = (unsigned char*)slurp_file("resources/fonts/WayfarersToyBoxRegular-gxxER.ttf");

  if (data == NULL) {
    log_f(LOG_ERROR, "Could not load ttf file to memory!");
    return 1;
  }

  if (!stbtt_InitFont(&font, data, 0)) {
    log_f(LOG_ERROR, "Could not init font!");
    return 1;
  }

  int height = 72;
  float scale = stbtt_ScaleForPixelHeight(&font, height);

  int w, h;
  unsigned char* bitmap = stbtt_GetCodepointBitmap(&font, 0.f, scale, 'B', &w, &h, NULL, NULL);
  unsigned char* tex_data = (unsigned char*)malloc(sizeof(unsigned char)*w*h*4);

  for (size_t i = 0; i < w*h; ++i) {
    unsigned char p = bitmap[i];
    size_t o = sizeof(unsigned char);
    size_t idx = i*4;
    tex_data[idx+(o*0)] = p;
    tex_data[idx+(o*1)] = p;
    tex_data[idx+(o*2)] = p;
    tex_data[idx+(o*3)] = p;
  }

  unsigned int tex_id;
  gl(glGenTextures(1, &tex_id));

  gl(glBindTexture(GL_TEXTURE_2D, tex_id));

  gl(glTextureParameteri(tex_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
  gl(glTextureParameteri(tex_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
  gl(glTextureParameteri(tex_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
  gl(glTextureParameteri(tex_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

  gl(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data));

  gl(glBindTexture(GL_TEXTURE_2D, 0));

  stbtt_FreeBitmap(bitmap, NULL);
  free(tex_data);
  */

  // vsync
  /* glfwSwapInterval(0); */

  Sprite spr = {0};
  Sprite spr2 = {0};

  Vector2f vel = {0};
  Vector2f dir = {0};
  const float speed = 100.f;

  if (!Sprite_load(&spr, "resources/gfx/khu_sheet.png", 3, 1)) return 1;
  if (!Sprite_load(&spr2, "resources/gfx/spawn_cmd.png", 1, 1)) return 1;


  spr.scale.x = 4.f;
  spr.scale.y = 4.f;

  spr2.scale.x = 4.f;
  spr2.scale.y = 4.f;

  Sprite_center_origin(&spr);

  // game loop
  while (!clock_should_quit(ctx)){
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
    if (ctx->keys[GLFW_KEY_A].held) scale_sprite(&spr, spr.scale.x <= scale_min ? scale_min : spr.scale.x - S * delta);
    if (ctx->keys[GLFW_KEY_S].held) scale_sprite(&spr, spr.scale.x + S * delta);

    spr.pos = ctx->mpos;

    Sprite_animate_hframe(&spr, delta);

    draw_sprite(ctx, &spr);
    draw_sprite(ctx, &spr2);

    clock_end_draw(ctx);
  }

  Sprite_deinit(&spr);
  Sprite_deinit(&spr2);
  clock_deinit(ctx);
  return 0;
}
