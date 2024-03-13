#ifndef _CLOCK_RESOURCE_MANAGER_H_
#define _CLOCK_RESOURCE_MANAGER_H_

#include <clock/clock_texture.h>
#include <stb/stb_ds.h>

typedef struct Resource_manager Resource_manager;
typedef struct Texture_KV Texture_KV;

struct Texture_KV {
  const char* key;
  Texture*    value;
};

struct Resource_manager {
  Texture_KV* texture_map;
};

Texture* Resman_load_texture(Resource_manager* res_man, const char* filepath);
void     Resman_unload_texture(Resource_manager* res_man, const char* filepath);

#endif /* _CLOCK_RESOURCE_MANAGER_H_ */
