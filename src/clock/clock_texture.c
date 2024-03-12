#include <clock/clock_texture.h>
#include <commonlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <clock/clock_core.h>

// unsigned int last_used_texture_slot = 0;

bool Texture_load_from_file(Texture* t, const char* filename){
  /* stbi_set_flip_vertically_on_load(1); */

  t->local_data = stbi_load(filename, &t->size.x, &t->size.y, &t->comps, 4);

  if (t->local_data == NULL){
    log_f(LOG_ERROR, "Could not load image '%s'", filename);
    return false;
  }
  assert(t->local_data != NULL);

  gl(glGenTextures(1, &t->id));

  gl(glBindTexture(GL_TEXTURE_2D, t->id));

  gl(glTextureParameteri(t->id, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
  gl(glTextureParameteri(t->id, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
  gl(glTextureParameteri(t->id, GL_TEXTURE_WRAP_S, GL_REPEAT));
  gl(glTextureParameteri(t->id, GL_TEXTURE_WRAP_T, GL_REPEAT));

  gl(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, t->size.x, t->size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, t->local_data));

  gl(glBindTexture(GL_TEXTURE_2D, 0));

  stbi_image_free(t->local_data);

  //  t->slot = last_used_texture_slot++;

  log_f(LOG_INFO, "Loaded texture '%s' (%dx%d)", filename, t->size.x, t->size.y);

  return true;
}

void Texture_deinit(Texture* t){
  //  if (last_used_texture_slot > 0) last_used_texture_slot--;
  gl(glDeleteTextures(1, &t->id));
}
