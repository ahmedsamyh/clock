#ifndef _CLOCK_TEXTURE_H_
#define _CLOCK_TEXTURE_H_

#include <clock/clock_vector.h>
#include <commonlib.h>
#include <stdbool.h>

static unsigned int last_used_texture_slot;

typedef struct {
  cstr name;
  uint8* local_data;
  uint32 id;
  Vector2i size;
  int comps;
  int slot;
} Texture;

void Texture_load_empty(Texture* texture, int width, int height);
bool Texture_load_from_file(Texture* texture, const char* filename);
bool Texture_load_from_memory(Texture* texture, unsigned char* data);
void Texture_deinit(Texture* texture);

#endif /* _CLOCK_TEXTURE_H_ */
