#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <clock/clock_vector.h>
#include <clock/clock_core.h>
#include <clock/clock_rect.h>

typedef struct Player Player;

#define DEFAULT_PLAYER_SPEED     100.f
#define DEFAULT_PLAYER_MAX_SPEED 300.f
#define DEFAULT_PLAYER_SIZE  32

#define PLAYER_MOVE_LEFT_KEY  GLFW_KEY_A
#define PLAYER_MOVE_RIGHT_KEY GLFW_KEY_D
#define PLAYER_MOVE_UP_KEY    GLFW_KEY_W
#define PLAYER_MOVE_DOWN_KEY  GLFW_KEY_S

#define LEFT  0
#define RIGHT 1

struct Player {
  Vector2f pos;
  Vector2f vel;
  Vector2f acc;
  Rect     hitbox;
  float    speed;
  float    max_speed;
  float    fric;
  bool     is_moving;
  Sprite   spr;
  Context* ctx;
  int      last_move_key;
};

bool Player_init(Player* player, Context* ctx, Texture* tex);
void Player_update(Player* player);
void Player_control(Player* player);
void Player_draw(Player* player, bool debug);

#endif /* _PLAYER_H_ */
