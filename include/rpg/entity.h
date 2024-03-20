#ifndef _ENTITY_H_
#define _ENTITY_H_

#include <clock/clock_core.h>

#define ENTITY_MEMBERS		     \
  Vector2f pos;			     \
  Vector2f vel;			     \
  Vector2f acc;			     \
  Rect hitbox;			     \
  float speed;			     \
  float max_speed;		     \
  float    fric;		     \
  bool     is_moving;		     \
  Sprite   spr;			     \
  Context* ctx;			     \
  Move_dir last_move_dir

#endif /* _ENTITY_H_ */
