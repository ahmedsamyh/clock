#include <clock/clock.h>


int main(void) {

  Context* ctx = clock_init(1280, 720, 1.f, 1.f, "Camera", 0);

  if (!ctx) return 1;

  Font font = {0};
  if (!Font_init_from_file(&font, ctx, "resources/fonts/WayfarersToyBoxRegular-gxxER.ttf")) return 1;

  Sprite spr = {0};
  if (!Sprite_init(&spr, Resman_load_texture_from_file(ctx->resman, "resources/gfx/flower.png"), 1, 1)) return 1;

  UI ui = UI_make(ctx, &font, (Vector2f) {32.f, 32.f});

  Arena str_arena = Arena_make(0);

  float camera_speed = 100.f;

  while (!clock_should_quit(ctx)) {
    clock_begin_draw(ctx);
    clock_clear(ctx, COLOR_BLACK);

    // move camera
    if (clock_key_held(ctx, KEY_A)) {
      ctx->camera.x -= camera_speed * ctx->delta;
    }
    if (clock_key_held(ctx, KEY_D)) {
      ctx->camera.x += camera_speed * ctx->delta;
    }
    if (clock_key_held(ctx, KEY_W)) {
      ctx->camera.y -= camera_speed * ctx->delta;
    }
    if (clock_key_held(ctx, KEY_S)) {
      ctx->camera.y += camera_speed * ctx->delta;
    }
    if (clock_key_held(ctx, KEY_E)) {
      ctx->camera.z -= camera_speed * ctx->delta;
    }
    if (clock_key_held(ctx, KEY_Q)) {
      ctx->camera.z += camera_speed * ctx->delta;
    }

    // reset camera
    if (clock_key_pressed(ctx, KEY_R)) {
      ctx->camera.x = 0.f;
      ctx->camera.y = 0.f;
    }

    clock_use_camera_view(ctx, true);

    draw_sprite_at(ctx, &spr, clock_screen_to_world(ctx, clock_mpos_screen(ctx)));

    Vector2f mpos_world = clock_mpos_world(ctx);

    clock_use_camera_view(ctx, false);

    UI_begin(&ui, UI_LAYOUT_KIND_VERT);

    Arena_reset(&str_arena);

    cstr camera_pos_str = Arena_alloc_str(str_arena, "Camera: %.2f, %.2f", ctx->camera.x, ctx->camera.y);
    UI_text(&ui, camera_pos_str, 24, COLOR_WHITE);

    cstr mpos_world_str = Arena_alloc_str(str_arena, "Mpos World: %.2f, %.2f", mpos_world.x, mpos_world.y);
    UI_text(&ui, mpos_world_str, 24, COLOR_WHITE);

    cstr mpos_screen_str = Arena_alloc_str(str_arena, "Mpos Screen: %.2f, %.2f", clock_mpos_screen(ctx).x, clock_mpos_screen(ctx).y);
    UI_text(&ui, mpos_screen_str, 24, COLOR_WHITE);

    UI_end(&ui);

    // draw camera origin
    Rect camera_rect = {
      .pos = (Vector2f) {ctx->camera.x, ctx->camera.y},
      .size = (Vector2f) {10.f, 10.f},
    };
    draw_rect_centered(ctx, camera_rect, COLOR_RED);
    {
      const float R = 32.f;
      draw_imm_line(ctx,
		    v2f_sub(camera_rect.pos, (Vector2f) {R, 0.f}),
		    v2f_add(camera_rect.pos, (Vector2f) {R, 0.f}),
		    COLOR_RED,
		    COLOR_RED);
      draw_imm_line(ctx,
		    v2f_sub(camera_rect.pos, (Vector2f) {0.f, R}),
		    v2f_add(camera_rect.pos, (Vector2f) {0.f, R}),
		    COLOR_RED,
		    COLOR_RED);
    }
    clock_end_draw(ctx);
  }

  Arena_free(&str_arena);
  UI_free(&ui);
  Font_deinit(&font);
  Sprite_deinit(&spr);
  clock_deinit(ctx);

  return 0;
}
