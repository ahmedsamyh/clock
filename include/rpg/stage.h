#ifndef _STAGE_H_
#define _STAGE_H_

#include <rpg/tile.h>
#include <rpg/player.h>

typedef struct Stage Stage;

struct Stage {
  const char* name;
  Tile* tiles;
  Vector2f size;
  Context* ctx;
};

void Stage_init(Stage* stage, Context* ctx, const char* name);
// pass NULL to player to not check for collisions against player
void Stage_update(Stage* stage, Player* player);
void Stage_draw(Stage* stage, bool debug);
bool Stage_add_tile(Stage* stage, Vector2i type);
void Stage_remove_tile(Stage* stage);
void Stage_deinit(Stage* stage);

// IO
bool Stage_save_to_file(Stage* stage);
bool Stage_load_from_file(Stage* stage);

#endif /* _STAGE_H_ */
