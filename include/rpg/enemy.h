#ifndef _ENEMY_H_
#define _ENEMY_H_

#include <clock/clock_vector.h>
#include <rpg/entity.h>
#include <rpg/config.h>

typedef struct Enemy Enemy;

struct Enemy {
  ENTITY_MEMBERS;
};

bool Enemy_init(Enemy* enemy, Context* ctx, Texture* tex);
void Enemy_update(Enemy* enemy);
void Enemy_control(Enemy* enemy);
void Enemy_draw(Enemy* enemy, bool debug);


#endif /* _ENEMY_H_ */
