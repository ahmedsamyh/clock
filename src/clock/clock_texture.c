#include <clock/clock_texture.h>
#include <commonlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <clock/clock_core.h>
#include <clock/clock_font.h>

unsigned int last_used_texture_slot = 0;

void Texture_load_font_atlas(Texture* t, int width, int height) {
  if (t->id == 0 ) {
    gl(glGenTextures(1, &t->id));
    gl(glBindTexture(GL_TEXTURE_2D, t->id));
    gl(glTextureParameteri(t->id, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    gl(glTextureParameteri(t->id, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    gl(glTextureParameteri(t->id, GL_TEXTURE_WRAP_S,     GL_REPEAT));
    gl(glTextureParameteri(t->id, GL_TEXTURE_WRAP_T,     GL_REPEAT));

    int max_tex_image_slots;
    gl(glGetIntegerv(GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS, &max_tex_image_slots));
    t->slot = (max_tex_image_slots-2);

    gl(glBindTexture(GL_TEXTURE_2D, 0));
  }

  t->size.x = width;
  t->size.y = height;

  gl(glBindTexture(GL_TEXTURE_2D, t->id));

  gl(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, t->size.x, t->size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));

  gl(glBindTexture(GL_TEXTURE_2D, 0));

  /* log_f(LOG_INFO, "Initialized font atlas texture '%s' (%dx%d) in slot %d", t->name, t->size.x, t->size.y, t->slot); */
}

void Texture_load_empty(Texture* t, int width, int height) {
  gl(glGenTextures(1, &t->id));

  t->size.x = width;
  t->size.y = height;

  gl(glBindTexture(GL_TEXTURE_2D, t->id));

  gl(glTextureParameteri(t->id, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
  gl(glTextureParameteri(t->id, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
  gl(glTextureParameteri(t->id, GL_TEXTURE_WRAP_S,     GL_REPEAT));
  gl(glTextureParameteri(t->id, GL_TEXTURE_WRAP_T,     GL_REPEAT));

  gl(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, t->size.x, t->size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));

  gl(glBindTexture(GL_TEXTURE_2D, 0));

  int max_tex_image_slots;

  gl(glGetIntegerv(GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS, &max_tex_image_slots));

  t->slot = (last_used_texture_slot++ % (max_tex_image_slots-2)); // last slot is used for ren_tex, second last is used for font atlas

  log_f(LOG_INFO, "Initialized texture '%s' (%dx%d) in slot %d", t->name, t->size.x, t->size.y, t->slot);
}

bool Texture_load_from_file(Texture* t, const char* filename) {
  /* stbi_set_flip_vertically_on_load(1); */

  t->local_data = stbi_load(filename, &t->size.x, &t->size.y, &t->comps, 4);
  t->name = filename;

  if (t->local_data == NULL){
    log_f(LOG_ERROR, "Could not load image '%s'", filename);
    return false;
  }
  assert(t->local_data != NULL);

  gl(glGenTextures(1, &t->id));

  gl(glBindTexture(GL_TEXTURE_2D, t->id));

  gl(glTextureParameteri(t->id, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
  gl(glTextureParameteri(t->id, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
  gl(glTextureParameteri(t->id, GL_TEXTURE_WRAP_S,     GL_REPEAT));
  gl(glTextureParameteri(t->id, GL_TEXTURE_WRAP_T,     GL_REPEAT));

  gl(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, t->size.x, t->size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, t->local_data));

  gl(glBindTexture(GL_TEXTURE_2D, 0));

  stbi_image_free(t->local_data);

  int max_tex_image_slots;

  gl(glGetIntegerv(GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS, &max_tex_image_slots));

  t->slot = (last_used_texture_slot++ % (max_tex_image_slots-2)); // last slot is used for ren_tex

  return true;
}

bool Texture_load_from_memory(Texture* t, unsigned char* data) {
  (void)t;
  (void)data;
  assert(0 && "Unfinished!");

  /* t->local_data = data; */

  /* // TODO: pass the name here */
  /* /\* t->name = *\/ */

  /* assert(t->local_data != NULL); */

  /* gl(glGenTextures(1, &t->id)); */

  /* gl(glBindTexture(GL_TEXTURE_2D, t->id)); */

  /* gl(glTextureParameteri(t->id, GL_TEXTURE_MIN_FILTER, GL_NEAREST)); */
  /* gl(glTextureParameteri(t->id, GL_TEXTURE_MAG_FILTER, GL_NEAREST)); */
  /* gl(glTextureParameteri(t->id, GL_TEXTURE_WRAP_S,     GL_REPEAT)); */
  /* gl(glTextureParameteri(t->id, GL_TEXTURE_WRAP_T,     GL_REPEAT)); */

  /* gl(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, t->size.x, t->size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, t->local_data)); */

  /* gl(glBindTexture(GL_TEXTURE_2D, 0)); */

  /* int max_tex_image_slots; */

  /* free(t->local_data); */

  /* gl(glGetIntegerv(GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS, &max_tex_image_slots)); */

  /* t->slot = (last_used_texture_slot++ % (max_tex_image_slots-2)); // last slot is used for ren_tex */

  return true;
}

void Texture_deinit(Texture* t) {
  if (last_used_texture_slot > 0) last_used_texture_slot--;
  gl(glDeleteTextures(1, &t->id));
}
