#ifndef _CLOCK_TEXTURE_H_
#define _CLOCK_TEXTURE_H_

#include <clock/clock_vector.h>
#include <stdbool.h>

static unsigned int last_used_texture_slot;

typedef struct {
  unsigned char* local_data;
  unsigned int id;
  Vector2i size;
  int comps;
  int slot;
} Texture;

bool Texture_load_from_file(Texture* texture, const char* filename);
void Texture_deinit(Texture* texture);

#endif /* _CLOCK_TEXTURE_H_ */
