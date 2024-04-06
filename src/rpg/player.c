#include <rpg/player.h>
#include <rpg/config.h>
#include <assert.h>

bool Player_init(Player* p, Context* ctx, Texture* tex) {
  p->pos = (Vector2f){0.f, 0.f};
  p->vel = (Vector2f){0.f, 0.f};
  p->acc = (Vector2f){0.f, 0.f};
  p->hitbox = (Rect){{0.f, 0.f}, {PLAYER_WIDTH, PLAYER_HEIGHT}};
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
  Player_control(p);
  Player_look_at_mouse(p);

  Context* ctx = p->ctx;
  assert(ctx);
  float delta = ctx->delta;

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
  p->hitbox.pos = (Vector2f){
    p->pos.x + PLAYER_HITBOX_OFFSET_X,
    p->pos.y + PLAYER_HITBOX_OFFSET_Y,
  };
  p->spr.pos = p->pos;

  /* bool mouse_moving = v2f_sub(ctx->mpos, ctx->prev_mpos).x != 0.f && v2f_sub(ctx->mpos, ctx->prev_mpos).y != 0.f; */

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
  Key* keys = p->ctx->k;

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

void Player_look_at_mouse(Player* p) {
  if (p->is_moving) return;

  Context* ctx = p->ctx;
  Vector2f look_dir = v2f_sub(ctx->mpos, p->pos);

  if (abs(look_dir.x) > abs(look_dir.y)) {
    if (look_dir.x > 0)      p->last_move_dir = MOVE_DIR_RIGHT;
    else if (look_dir.x < 0) p->last_move_dir = MOVE_DIR_LEFT;
  } else {
    if (look_dir.y > 0)      p->last_move_dir = MOVE_DIR_DOWN;
    else if (look_dir.y < 0) p->last_move_dir = MOVE_DIR_UP;
  }

  /* log_f(LOG_INFO, "look_dir: %f, %f", look_dir.x, look_dir.y); */
}

void Player_draw(Player* p, bool debug) {
  assert(p->ctx);

  draw_sprite(p->ctx, &p->spr);

  if (debug) {
    Color color = (p->hitting ? COLOR_RED : COLOR_GREEN);
    draw_rect(p->ctx, p->hitbox, color_alpha(color, 0.45f));
  }
}

void Player_set_pos_to_hitbox_pos(Player* p) {
  p->pos = (Vector2f){
    p->hitbox.pos.x - PLAYER_HITBOX_OFFSET_X,
    p->hitbox.pos.y - PLAYER_HITBOX_OFFSET_Y,
  };
}
