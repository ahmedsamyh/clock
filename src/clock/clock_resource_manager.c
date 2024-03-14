#include <clock/clock_resource_manager.h>
#define STB_DS_IMPLEMENTATION
#include <stb/stb_ds.h>
#include <stdlib.h>
#include <assert.h>
#include <commonlib.h>

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

  arrput(res_man->texture_names, filepath);

  return tex;
}

void Resman_unload_texture(Resource_manager* res_man, const char* filepath) {
  Texture_KV* tex_kv = hmgetp_null(res_man->texture_map, filepath);
  if (tex_kv != NULL) {
    const int size_x = tex_kv->value->size.x, size_y = tex_kv->value->size.y, slot = tex_kv->value->slot;
    free(tex_kv->value);
    hmdel(res_man->texture_map, filepath); // ignore retval
    log_f(LOG_INFO, "Unloaded texture '%s' (%dx%d) in slot %d", filepath, size_x, size_y, slot);
  }
}

void Resman_unload_every_texture(Resource_manager* res_man) {
  for (size_t i = 0; i < arrlenu(res_man->texture_names); ++i) {
    assert(hmgeti(res_man->texture_map, res_man->texture_names[i]) != -1);

    Resman_unload_texture(res_man, res_man->texture_names[i]);
  }

  arrfree(res_man->texture_names);
  hmfree (res_man->texture_map);
}
