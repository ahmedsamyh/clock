#include <clock/clock.h>
#include <stdlib.h>

int main(void){
  Context* ctx = clock_init(800, 800, 1.f, 1.f, "GLFW Window");

  if (ctx == NULL) return 1;

  float width =  (float)ctx->win->width;
  float height = (float)ctx->win->height;

  float deg = 0.f;
  float scl = 1.f;
  float a = 0.f;

  const float w = 100.f;
  const float h = 100.f;

  // game loop
  while (!clock_should_quit(ctx)){
    clock_begin_draw(ctx);

    clock_clear(ctx, COLOR_BLACK);

    Vector3f p0 = {-w, -h, 0.f};
    Vector3f p1 = {+w, -h, 0.f};
    Vector3f p2 = {+w, +h, 0.f};
    Vector3f p3 = {-w, +h, 0.f};

    Vector4f p0_ = {p0.x, p0.y, p0.z, 1.f};
    Vector4f p1_ = {p1.x, p1.y, p1.z, 1.f};
    Vector4f p2_ = {p2.x, p2.y, p2.z, 1.f};
    Vector4f p3_ = {p3.x, p3.y, p3.z, 1.f};

    // rotation
    p0_ = Mat4_rotate_z_vector(p0_, deg);
    p1_ = Mat4_rotate_z_vector(p1_, deg);
    p2_ = Mat4_rotate_z_vector(p2_, deg);
    p3_ = Mat4_rotate_z_vector(p3_, deg);

    // scale
    scl = 1.f + (sinf(a) + 1.f)*0.5f;
    p0_ = Mat4_scale_vector(p0_, (Vector3f){scl, scl, scl});
    p1_ = Mat4_scale_vector(p1_, (Vector3f){scl, scl, scl});
    p2_ = Mat4_scale_vector(p2_, (Vector3f){scl, scl, scl});
    p3_ = Mat4_scale_vector(p3_, (Vector3f){scl, scl, scl});

    // translation
    p0_ = Mat4_translate_vector(p0_, (Vector3f){ctx->mpos.x, ctx->mpos.y, 0.f});
    p1_ = Mat4_translate_vector(p1_, (Vector3f){ctx->mpos.x, ctx->mpos.y, 0.f});
    p2_ = Mat4_translate_vector(p2_, (Vector3f){ctx->mpos.x, ctx->mpos.y, 0.f});
    p3_ = Mat4_translate_vector(p3_, (Vector3f){ctx->mpos.x, ctx->mpos.y, 0.f});

    p0 = (Vector3f){p0_.x, p0_.y, p0_.z};
    p1 = (Vector3f){p1_.x, p1_.y, p1_.z};
    p2 = (Vector3f){p2_.x, p2_.y, p2_.z};
    p3 = (Vector3f){p3_.x, p3_.y, p3_.z};

    draw_imm_quad(ctx, p0, p1, p2, p3,
		    COLOR_RED,
		    COLOR_GREEN,
		    COLOR_BLUE,
		    COLOR_WHITE);
    deg += 100.f * ctx->delta;
    a += ctx->delta;

    clock_end_draw(ctx);
  }

  clock_deinit(ctx);
  return 0;
}
