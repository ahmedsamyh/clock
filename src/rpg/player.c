#include <rpg/player.h>
#include <rpg/config.h>
#include <assert.h>

bool Player_init(Player* p, Context* ctx, Texture* tex) {
  p->pos = (Vector2f){0.f, 0.f};
  p->vel = (Vector2f){0.f, 0.f};
  p->acc = (Vector2f){0.f, 0.f};
  p->hitbox = (Rect){{0.f, 0.f}, {PLAYER_SIZE, PLAYER_SIZE}};
  p->speed = PLAYER_SPEED;
  p->max_speed = PLAYER_MAX_SPEED;
  p->fric  = 0.8f;
  p->is_moving = false;
  p->ctx = ctx;

  if (!Sprite_init(&p->spr, tex, 4, 8)) return false;

  p->spr.scale = (Vector2f){SCALE, SCALE};
  p->spr.time_per_frame = 0.1f;

  return true;
}

void Player_update(Player* p) {
  assert(p->ctx);
  float delta = p->ctx->delta;

  p->vel = v2f_add(p->vel, v2f_muls(p->acc, p->speed));
  p->pos = v2f_add(p->pos, v2f_muls(p->vel, delta));
  p->acc = (Vector2f){0.f, 0.f};

  // friction
  if (!p->is_moving)
    /* p->vel = v2f_sub(p->vel, v2f_muls(p->vel, p->fric)); */
    p->vel = v2f_muls(p->vel, p->fric);

  // limit vel to max_speed
  if (v2f_mag2(p->vel) > p->max_speed*p->max_speed) {
    p->vel = v2f_muls(v2f_normalize(p->vel), p->max_speed);
  }

  // update positions
  p->hitbox.pos = p->pos;
  p->spr.pos    = p->pos;

  // animate sprite
  Sprite_animate_hframe(&p->spr, delta);
  if (p->is_moving) {
    switch (p->last_move_dir) {
    case MOVE_DIR_RIGHT: Sprite_set_vframe(&p->spr, 4 + 0); break;
    case MOVE_DIR_DOWN:  Sprite_set_vframe(&p->spr, 4 + 1); break;
    case MOVE_DIR_LEFT:  Sprite_set_vframe(&p->spr, 4 + 2); break;
    case MOVE_DIR_UP:    Sprite_set_vframe(&p->spr, 4 + 3); break;
    }
  } else {
    switch (p->last_move_dir) {
    case MOVE_DIR_RIGHT: Sprite_set_vframe(&p->spr, 0); break;
    case MOVE_DIR_DOWN:  Sprite_set_vframe(&p->spr, 1); break;
    case MOVE_DIR_LEFT:  Sprite_set_vframe(&p->spr, 2); break;
    case MOVE_DIR_UP:    Sprite_set_vframe(&p->spr, 3); break;
    }
  }
}

void Player_control(Player* p) {
  assert(p->ctx);
  Key* keys = p->ctx->keys;

  Vector2f dir = {0};
  if (keys[PLAYER_MOVE_LEFT_KEY].held) {
    dir.x--;
    p->last_move_dir = MOVE_DIR_LEFT;
  }

  if (keys[PLAYER_MOVE_RIGHT_KEY].held) {
    dir.x++;
    p->last_move_dir = MOVE_DIR_RIGHT;
  }

  if (keys[PLAYER_MOVE_UP_KEY].held) {
    dir.y--;
    p->last_move_dir = MOVE_DIR_UP;
  }

  if (keys[PLAYER_MOVE_DOWN_KEY].held) {
    dir.y++;
    p->last_move_dir = MOVE_DIR_DOWN;
  }
  dir = v2f_normalize(dir);

  p->is_moving = v2f_mag2(dir) > 0.f;

  p->acc = v2f_add(p->acc, dir);

}

void Player_draw(Player* p, bool debug) {
  assert(p->ctx);

  draw_sprite(p->ctx, &p->spr);

  if (debug) {
    draw_rect(p->ctx, p->hitbox, color_alpha(COLOR_RED, 0.45f));
  }
}
