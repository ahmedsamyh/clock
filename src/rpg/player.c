#include <rpg/player.h>
#include <assert.h>

void Player_init(Player* player, Context* ctx) {
  player->pos = (Vector2f){0.f, 0.f};
  player->vel = (Vector2f){0.f, 0.f};
  player->acc = (Vector2f){0.f, 0.f};
  player->hitbox = (Rect){player->pos, {DEFAULT_PLAYER_SIZE, DEFAULT_PLAYER_SIZE}};
  player->speed = DEFAULT_PLAYER_SPEED;
  player->max_speed = DEFAULT_PLAYER_MAX_SPEED;
  player->fric  = 0.8f;
  player->is_moving = false;
  player->ctx   = ctx;
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

  // update hitbox pos to p->pos
  p->hitbox.pos = p->pos;

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

void Player_draw(Player* p) {
  assert(p->ctx);

  //draw_imm_quad(Context* ctx, Vector3f p0, Vector3f p1, Vector3f p2, Vector3f p3, Color c0, Color c1, Color c2, Color c3);
  Vector3f p0 = (Vector3f){p->pos.x,                    p->pos.y,                    0.f};
  Vector3f p1 = (Vector3f){p->pos.x + p->hitbox.size.x, p->pos.y,                    0.f};
  Vector3f p2 = (Vector3f){p->pos.x + p->hitbox.size.x, p->pos.y + p->hitbox.size.y, 0.f};
  Vector3f p3 = (Vector3f){p->pos.x,                    p->pos.y + p->hitbox.size.y, 0.f};
  Color col = COLOR_RED;
  draw_imm_quad(p->ctx, p0, p1, p2, p3, col, col, col, col);
}
