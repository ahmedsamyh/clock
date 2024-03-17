#include <rpg/player.h>
#include <assert.h>

bool Player_init(Player* p, Context* ctx, Texture* tex) {
  p->pos = (Vector2f){0.f, 0.f};
  p->vel = (Vector2f){0.f, 0.f};
  p->acc = (Vector2f){0.f, 0.f};
  p->hitbox = (Rect){{0.f, 0.f}, {DEFAULT_PLAYER_SIZE, DEFAULT_PLAYER_SIZE}};
  p->speed = DEFAULT_PLAYER_SPEED;
  p->max_speed = DEFAULT_PLAYER_MAX_SPEED;
  p->fric  = 0.8f;
  p->is_moving = false;
  p->ctx = ctx;

  if (!Sprite_init(&p->spr, tex, 1, 1)) return false;

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
}

void Player_control(Player* p) {
  assert(p->ctx);
  Key* keys = p->ctx->keys;

  Vector2f dir = {0};
  if (keys[PLAYER_MOVE_LEFT_KEY].held) {
    dir.x--;
  }

  if (keys[PLAYER_MOVE_RIGHT_KEY].held) {
    dir.x++;
  }

  if (keys[PLAYER_MOVE_UP_KEY].held) {
    dir.y--;
  }

  if (keys[PLAYER_MOVE_DOWN_KEY].held) {
    dir.y++;
  }
  dir = v2f_normalize(dir);

  p->is_moving = v2f_mag2(dir) > 0.f;

  p->acc = v2f_add(p->acc, dir);

}

void Player_draw(Player* p, bool debug) {
  assert(p->ctx);

  if (debug && false) {
    Rect base_hitbox = {(Vector2f){p->pos.x, p->pos.y}, p->hitbox.size};

    // draw the base (pos at height 0)
    draw_rect_centered(p->ctx, base_hitbox, COLOR_GREEN);

    draw_imm_line(p->ctx, (Vector3f){base_hitbox.pos.x, base_hitbox.pos.y}, (Vector3f){p->hitbox.pos.x, p->hitbox.pos.y}, COLOR_GREEN, COLOR_RED);

    // draw actual pos with height considered
    draw_rect_centered(p->ctx, p->hitbox, COLOR_RED);
  }

  draw_sprite(p->ctx, &p->spr);
}
