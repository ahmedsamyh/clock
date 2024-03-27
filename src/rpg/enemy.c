#include <rpg/enemy.h>
#include <rpg/config.h>
#include <assert.h>

bool Enemy_init(Enemy* e, Context* ctx, Texture* tex) {
  e->pos = (Vector2f){0.f, 0.f};
  e->vel = (Vector2f){0.f, 0.f};
  e->acc = (Vector2f){0.f, 0.f};
  e->hitbox = (Rect){{0.f, 0.f}, {ENEMY_WIDTH, ENEMY_HEIGHT}};
  e->speed = ENEMY_SPEED;
  e->max_speed = ENEMY_MAX_SPEED;
  e->fric  = 0.8f;
  e->is_moving = false;
  e->ctx = ctx;

  if (!Sprite_init(&e->spr, tex, 4, 8)) return false;

  e->spr.scale = (Vector2f){SCALE, SCALE};
  e->spr.time_per_frame = 0.1f;

  return true;
}

void Enemy_update(Enemy* e) {
  assert(e->ctx);
  float delta = e->ctx->delta;

  e->vel = v2f_add(e->vel, v2f_muls(e->acc, e->speed));
  e->pos = v2f_add(e->pos, v2f_muls(e->vel, delta));
  e->acc = (Vector2f){0.f, 0.f};

  // friction
  if (!e->is_moving)
    /* e->vel = v2f_sub(e->vel, v2f_muls(e->vel, e->fric)); */
    e->vel = v2f_muls(e->vel, e->fric);

  // limit vel to max_speed
  if (v2f_mag2(e->vel) > e->max_speed*e->max_speed) {
    e->vel = v2f_muls(v2f_normalize(e->vel), e->max_speed);
  }

  // update positions
  e->hitbox.pos = e->pos;
  e->spr.pos    = e->pos;

  // animate sprite
  Sprite_animate_hframe(&e->spr, delta);
  if (e->is_moving) {
    switch (e->last_move_dir) {
    case MOVE_DIR_RIGHT: Sprite_set_vframe(&e->spr, 4 + 0); break;
    case MOVE_DIR_DOWN:  Sprite_set_vframe(&e->spr, 4 + 1); break;
    case MOVE_DIR_LEFT:  Sprite_set_vframe(&e->spr, 4 + 2); break;
    case MOVE_DIR_UP:    Sprite_set_vframe(&e->spr, 4 + 3); break;
    }
  } else {
    switch (e->last_move_dir) {
    case MOVE_DIR_RIGHT: Sprite_set_vframe(&e->spr, 0); break;
    case MOVE_DIR_DOWN:  Sprite_set_vframe(&e->spr, 1); break;
    case MOVE_DIR_LEFT:  Sprite_set_vframe(&e->spr, 2); break;
    case MOVE_DIR_UP:    Sprite_set_vframe(&e->spr, 3); break;
    }
  }
}

void Enemy_draw(Enemy* e, bool debug) {
  assert(e->ctx);

  draw_sprite(e->ctx, &e->spr);

  if (debug) {
    draw_rect(e->ctx, e->hitbox, color_alpha(COLOR_RED, 0.45f));
  }
}
