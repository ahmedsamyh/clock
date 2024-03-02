#include <math.h>
#include <clock.h>
#define MATRIX_IMPLEMENTATION
#include <matrix.h>
#define VECTOR_IMPLEMENTATION
#include <vector.h>
#define COMMONLIB_IMPLEMENTATION
#include <commonlib.h>

int main(void){
#if 0

  Vector4f v = {10, 10, 10, 1};
  Vector4f r = Mat4_translate_vector(v, (Vector3f){10.f, 0.f, 0.f});

  printf("r: %.2f, %.2f, %.2f, %.2f\n", r.x, r.y, r.z, r.w);

  r = Mat4_scale_vector(r, (Vector3f){2.f, 2.f, 2.f});

  printf("r: %.2f, %.2f, %.2f, %.2f\n", r.x, r.y, r.z, r.w);
#else
  Window win = {0};

  if (Window_init(&win, 800, 800, "GLFW Window") < 0) return 1;

  float width =  (float)win.width;
  float height = (float)win.height;

  Renderer ren = {0};

  if (Renderer_init(&ren, &win) < 0) return 1;

  float deg = 0.f;
  float scl = 1.f;
  float a = 0.f;

  // vsync
  glfwSwapInterval(0);

  const size_t num_quads = 150;
  const float w = 10.f;
  const float h = 10.f;

  // game loop
  while (!glfwWindowShouldClose(win.glfw_win)){
    Window_begin_draw(&win);

    Window_clear(&win, COLOR_BLACK);

    for (size_t i = 0; i < num_quads; ++i){
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
      float xd = 0.01f * (i*sinf(a));
      float yd = 0.01f * (i*cosf(a)) + (xd*sinf(a+0.4f));
      p0_ = Mat4_translate_vector(p0_, (Vector3f){win.mpos.x + (float)i*xd, win.mpos.y + (float)i*yd, 0.f});
      p1_ = Mat4_translate_vector(p1_, (Vector3f){win.mpos.x + (float)i*xd, win.mpos.y + (float)i*yd, 0.f});
      p2_ = Mat4_translate_vector(p2_, (Vector3f){win.mpos.x + (float)i*xd, win.mpos.y + (float)i*yd, 0.f});
      p3_ = Mat4_translate_vector(p3_, (Vector3f){win.mpos.x + (float)i*xd, win.mpos.y + (float)i*yd, 0.f});

      p0 = (Vector3f){p0_.x, p0_.y, p0_.z};
      p1 = (Vector3f){p1_.x, p1_.y, p1_.z};
      p2 = (Vector3f){p2_.x, p2_.y, p2_.z};
      p3 = (Vector3f){p3_.x, p3_.y, p3_.z};


      Render_imm_quad(&ren, p0, p1, p2, p3,
		      COLOR_RED,
		      COLOR_GREEN,
		      COLOR_BLUE,
		      COLOR_WHITE);
    }

    deg += 100.f * win.delta;
    a += win.delta;

    Window_end_draw(&win);
  }

  Renderer_deinit(&ren);
  Window_deinit(&win);

#endif
  return 0;
}
