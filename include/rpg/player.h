#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <clock/clock_vector.h>
#include <clock/clock_core.h>
#include <clock/clock_rect.h>
#include <rpg/config.h>
#include <rpg/entity.h>

typedef struct Player Player;

struct Player {
  ENTITY_MEMBERS;
  bool hitting;
};

bool Player_init(Player* player, Context* ctx, Texture* tex);
void Player_update(Player* player);
void Player_control(Player* player);
void Player_draw(Player* player, bool debug);
void Player_set_pos_to_hitbox_pos(Player* player);

#endif /* _PLAYER_H_ */
