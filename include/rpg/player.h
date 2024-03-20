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
};

bool Player_init(Player* player, Context* ctx, Texture* tex);
void Player_update(Player* player);
void Player_control(Player* player);
void Player_draw(Player* player, bool debug);

#endif /* _PLAYER_H_ */
