#include <clock/clock_resource_manager.h>
#define STB_DS_IMPLEMENTATION
#include <stb/stb_ds.h>
#include <stdlib.h>
#include <assert.h>
#include <commonlib.h>

Texture* Resman_load_texture_from_file(Resource_manager* res_man, const char* filepath) {
  Texture_KV* tex_kv = shgetp_null(res_man->texture_map, filepath);

  if (tex_kv != NULL) return tex_kv->value;

  Texture* tex = (Texture*)calloc(1, sizeof(Texture));

  if (!Texture_load_from_file(tex, filepath)) {
    free(tex);
    return NULL;
  };

  shput(res_man->texture_map, filepath, tex);

  assert(tex != NULL);

  arrput(res_man->texture_names, filepath);

  log_f(LOG_INFO, "Loaded texture '%s' (%dx%d) in slot %d", filepath, tex->size.x, tex->size.y, tex->slot);

  return tex;
}

Texture* Resman_load_texture_uninitialized(Resource_manager* res_man, const char* filepath) {
  Texture_KV* tex_kv = shgetp_null(res_man->texture_map, filepath);

  if (tex_kv != NULL) return tex_kv->value;

  Texture* tex = (Texture*)calloc(1, sizeof(Texture));

  shput(res_man->texture_map, filepath, tex);

  assert(tex != NULL);

  arrput(res_man->texture_names, filepath);

  log_f(LOG_INFO, "Loaded uninitialized texture '%s' (%dx%d) in slot %d", filepath, tex->size.x, tex->size.y, tex->slot);

  return tex;
}

void Resman_unload_texture(Resource_manager* res_man, const char* filepath) {
  Texture_KV* tex_kv = shgetp_null(res_man->texture_map, filepath);
  if (tex_kv != NULL) {
    const int size_x = tex_kv->value->size.x, size_y = tex_kv->value->size.y, slot = tex_kv->value->slot;
    free(tex_kv->value);
    shdel(res_man->texture_map, filepath); // ignore retval
    log_f(LOG_INFO, "Unloaded texture '%s' (%dx%d) in slot %d", filepath, size_x, size_y, slot);
  }
}

void Resman_unload_every_texture(Resource_manager* res_man) {
  for (size_t i = 0; i < arrlenu(res_man->texture_names); ++i) {
    assert(shgeti(res_man->texture_map, res_man->texture_names[i]) != -1);

    Resman_unload_texture(res_man, res_man->texture_names[i]);
  }

  arrfree(res_man->texture_names);
  hmfree (res_man->texture_map);
}
