#ifndef _CONFIG_H_
#define _CONFIG_H_


#define SCALE 4.f

#define UNSCALED_TILE_SIZE 16.f
#define TILE_SIZE (UNSCALED_TILE_SIZE*SCALE)

#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT  960

typedef enum {
  MOVE_DIR_LEFT,
  MOVE_DIR_RIGHT,
  MOVE_DIR_UP,
  MOVE_DIR_DOWN,
  MOVE_DIR_COUNT,
} Move_dir;

// PLAYER
#define PLAYER_SPEED           100.f
#define PLAYER_MAX_SPEED       300.f
#define PLAYER_WIDTH           (8.f*SCALE)
#define PLAYER_HEIGHT          (7.f*SCALE)
#define PLAYER_HITBOX_OFFSET_X (4.f*SCALE)
#define PLAYER_HITBOX_OFFSET_Y (9.f*SCALE)

#define PLAYER_MOVE_LEFT_KEY  GLFW_KEY_A
#define PLAYER_MOVE_RIGHT_KEY GLFW_KEY_D
#define PLAYER_MOVE_UP_KEY    GLFW_KEY_W
#define PLAYER_MOVE_DOWN_KEY  GLFW_KEY_S

#define LEFT  0
#define RIGHT 1

// ENEMY
#define ENEMY_SPEED     PLAYER_SPEED
#define ENEMY_MAX_SPEED PLAYER_MAX_SPEED
#define ENEMY_WIDTH      PLAYER_WIDTH
#define ENEMY_HEIGHT     PLAYER_HEIGHT

#endif /* _CONFIG_H_ */
