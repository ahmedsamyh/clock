#include <rpg/player.h>
#include <assert.h>

bool Player_init(Player* player, Context* ctx, Texture* head_tex, Texture* torso_tex, Texture* arm_tex, Texture* leg_tex) {
  player->pos = (Vector3f){0.f, 0.f, 0.f};
  player->vel = (Vector2f){0.f, 0.f};
  player->acc = (Vector2f){0.f, 0.f};
  player->hitbox = (Rect){{0.f, 0.f}, {DEFAULT_PLAYER_SIZE, DEFAULT_PLAYER_SIZE}};
  player->speed = DEFAULT_PLAYER_SPEED;
  player->max_speed = DEFAULT_PLAYER_MAX_SPEED;
  player->fric  = 0.8f;
  player->is_moving = false;
  player->ctx = ctx;

  if (!Sprite_init(&player->head_spr, head_tex, 1, 1)) return false;
  if (!Sprite_init(&player->torso_spr, torso_tex, 1, 1)) return false;
  for (size_t i = 0; i < 2; ++i) {
    if (!Sprite_init(&player->arm_spr[i], arm_tex, 1, 1)) return false;
    if (!Sprite_init(&player->leg_spr[i], leg_tex, 1, 1)) return false;
  }

  return true;
}

void Player_update(Player* p) {
  assert(p->ctx);
  float delta = p->ctx->delta;

  p->vel = v2f_add(p->vel, v2f_muls(p->acc, p->speed));
  const Vector2f vel_dt = v2f_muls(p->vel, delta);
  p->pos = v3f_add(p->pos, (Vector3f){vel_dt.x, vel_dt.y, 0.f});
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
  p->hitbox.pos = two_5d_to_2d(p->pos);
  /* p->spr.pos   = p->pos; */
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


  // TEMP: height
  const float D = 50.f;
  if (keys[GLFW_KEY_Z].held) {
    change_height(&p->pos, p->pos.z + D * p->ctx->delta);
  }

  if (keys[GLFW_KEY_X].held) {
    change_height(&p->pos, p->pos.z - D * p->ctx->delta);
  }
}

void Player_draw(Player* p) {
  assert(p->ctx);

  // draw the base (pos at height 0)
  Vector2f pos = (Vector2f){p->pos.x, p->pos.y};
  Vector3f p0 = (Vector3f){pos.x,                    pos.y,                    0.f};
  Vector3f p1 = (Vector3f){pos.x + p->hitbox.size.x, pos.y,                    0.f};
  Vector3f p2 = (Vector3f){pos.x + p->hitbox.size.x, pos.y + p->hitbox.size.y, 0.f};
  Vector3f p3 = (Vector3f){pos.x,                    pos.y + p->hitbox.size.y, 0.f};
  Color col = {0};

  // actual pos with height considered
  pos = two_5d_to_2d(p->pos);
  Vector3f p0_ = (Vector3f){pos.x,                    pos.y,                    0.f};
  Vector3f p1_ = (Vector3f){pos.x + p->hitbox.size.x, pos.y,                    0.f};
  Vector3f p2_ = (Vector3f){pos.x + p->hitbox.size.x, pos.y + p->hitbox.size.y, 0.f};
  Vector3f p3_ = (Vector3f){pos.x,                    pos.y + p->hitbox.size.y, 0.f};

  col = COLOR_WHITE;
  draw_imm_line(p->ctx, p0, p0_, col, col);
  draw_imm_line(p->ctx, p1, p1_, col, col);

  col = COLOR_GREEN;
  draw_imm_quad(p->ctx, p0, p1, p2, p3, col, col, col, col);

  col = COLOR_RED;
  draw_imm_quad(p->ctx, p0_, p1_, p2_, p3_, col, col, col, col);

  col = COLOR_WHITE;
  draw_imm_line(p->ctx, p2, p2_, col, col);
  draw_imm_line(p->ctx, p3, p3_, col, col);

  /* draw_sprite(p->ctx, &p->spr); */
}
