#ifndef _RENDER_TARGET_H_
#define _RENDER_TARGET_H_

#include <clock/clock_core.h>

typedef struct Render_target Render_target;

struct Render_target {
  unsigned int color;
  unsigned int depth;
  unsigned int fbo;
  Context* ctx;
  unsigned int width, height;
};

bool Rentar_init(Render_target* rt, Context* ctx, unsigned int width, unsigned int height);
void Rentar_set_to_current(Render_target* rt);
void Rentar_blit(Render_target* rt);
void Rentar_deinit(Render_target* rt);

#endif /* _RENDER_TARGET_H_ */
