#include <clock/clock_resource_manager.h>
#define STB_DS_IMPLEMENTATION
#include <stb/stb_ds.h>
#include <stdlib.h>
#include <assert.h>

Texture* Resman_load_texture(Resource_manager* res_man, const char* filepath) {
  Texture_KV* tex_kv = hmgetp_null(res_man->texture_map, filepath);

  if (tex_kv != NULL) return tex_kv->value;

  Texture* tex = (Texture*)calloc(1, sizeof(Texture));

  if (!Texture_load_from_file(tex, filepath)) {
    free(tex);
    return NULL;
  };

  hmput(res_man->texture_map, filepath, tex);

  assert(tex != NULL);

  return tex;
}

void Resman_unload_texture(Resource_manager* res_man, const char* filepath) {
  Texture_KV* tex = hmgetp_null(res_man->texture_map, filepath);
  if (tex != NULL) {
    free(tex->value);
    hmdel(res_man->texture_map, filepath); // ignore retval
  }
}
