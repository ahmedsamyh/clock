#include <rpg/player.h>
#include <assert.h>

bool Player_init(Player* p, Context* ctx, Texture* head_tex, Texture* torso_tex, Texture* arm_tex, Texture* leg_tex) {
  p->pos = (Vector3f){0.f, 0.f, 0.f};
  p->vel = (Vector2f){0.f, 0.f};
  p->acc = (Vector2f){0.f, 0.f};
  p->hitbox = (Rect){{0.f, 0.f}, {DEFAULT_PLAYER_SIZE, DEFAULT_PLAYER_SIZE}};
  p->speed = DEFAULT_PLAYER_SPEED;
  p->max_speed = DEFAULT_PLAYER_MAX_SPEED;
  p->fric  = 0.8f;
  p->is_moving = false;
  p->ctx = ctx;

  if (!Sprite_init(&p->head_spr, head_tex, 1, 1)) return false;
  if (!Sprite_init(&p->torso_spr, torso_tex, 1, 1)) return false;
  for (size_t i = 0; i < 2; ++i) {
    if (!Sprite_init(&p->arm_spr[i], arm_tex, 1, 1)) return false;
    if (!Sprite_init(&p->leg_spr[i], leg_tex, 1, 1)) return false;
    Sprite* a = &p->arm_spr[i];
    a->origin = (Vector2f){a->size.x * 0.5f, 0.f};
    a = &p->leg_spr[i];
    a->origin = (Vector2f){a->size.x * 0.5f, 0.f};
  }

  Sprite_center_origin(&p->head_spr);
  Sprite* t = &p->torso_spr;
  t->origin = (Vector2f){t->size.x * 0.5f, 0.f};

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
  const float torso_height = p->torso_spr.tex_rect.size.y * 0.85f;
  const float head_height  = torso_height + (p->head_spr.tex_rect.size.y * 0.5f);
  const float arm_height   = torso_height * 0.75f;
  const float leg_height   = 0.f;//p->leg_spr[LEFT].tex_rect.size.y;

  p->hitbox.pos = two_5d_to_2d(p->pos);
  p->head_spr.pos  = (Vector2f){p->pos.x, p->pos.y - head_height};
  p->torso_spr.pos = (Vector2f){p->pos.x, p->pos.y - torso_height};
  p->arm_spr[LEFT].pos   = (Vector2f){p->pos.x - (p->torso_spr.tex_rect.size.x * 0.4f), p->pos.y - arm_height};
  p->arm_spr[RIGHT].pos  = (Vector2f){p->pos.x + (p->torso_spr.tex_rect.size.x * 0.4f), p->pos.y - arm_height};
  p->leg_spr[LEFT].pos   = (Vector2f){p->pos.x - (p->torso_spr.tex_rect.size.x * 0.4f), p->pos.y - leg_height};
  p->leg_spr[RIGHT].pos  = (Vector2f){p->pos.x + (p->torso_spr.tex_rect.size.x * 0.4f), p->pos.y - leg_height};

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

  draw_sprite(p->ctx, &p->leg_spr[LEFT]);
  draw_sprite(p->ctx, &p->leg_spr[RIGHT]);
  draw_sprite(p->ctx, &p->torso_spr);
  draw_sprite(p->ctx, &p->arm_spr[LEFT]);
  draw_sprite(p->ctx, &p->arm_spr[RIGHT]);
  draw_sprite(p->ctx, &p->head_spr);
}
