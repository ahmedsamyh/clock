#include <clock/clock_render_target.h>
#include <clock/clock_core.h>
#include <assert.h>

bool Rentar_init(Render_target* rt, Context* ctx, unsigned int width, unsigned int height) {
  rt->ctx = ctx;
  rt->width = width;
  rt->height = height;

  // color attachment
  gl(glGenTextures(1, &rt->color));
  gl(glBindTexture(GL_TEXTURE_2D, rt->color));

  gl(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
  gl(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
  gl(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
  gl(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

  gl(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rt->width, rt->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL))

  gl(glBindTexture(GL_TEXTURE_2D, 0));

  // depth attachment
  gl(glGenRenderbuffers(1, &rt->depth));
  gl(glBindRenderbuffer(GL_RENDERBUFFER, rt->depth));
  gl(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, rt->width, rt->height));
  gl(glBindRenderbuffer(GL_RENDERBUFFER, 0));

  gl(glGenFramebuffers(1, &rt->fbo));
  gl(glBindFramebuffer(GL_FRAMEBUFFER, rt->fbo));
  gl(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rt->color, 0));
  gl(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rt->depth));

  gl(GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER));
  if (status != GL_FRAMEBUFFER_COMPLETE) {
    log_f(LOG_ERROR, "Failed to create Framebuffer Object!");
    return false;
  }
  gl(glBindFramebuffer(GL_FRAMEBUFFER, 0));

  return true;
}

void Rentar_set_to_current(Render_target* rt) {
  gl(glBindFramebuffer(GL_FRAMEBUFFER, rt->fbo));
}

void Rentar_blit(Render_target* rt) {
  assert(rt->ctx);
  Context* ctx = rt->ctx;
  const unsigned int default_fbo = 0;
  gl(glBlitNamedFramebuffer(rt->fbo, default_fbo,
			    0, 0, rt->width,       rt->height,
			    0, 0, ctx->win->width, ctx->win->height,
			    GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
			    GL_NEAREST));

  gl(glBindFramebuffer(GL_FRAMEBUFFER, default_fbo));
}

void Rentar_deinit(Render_target* rt) {
  gl(glDeleteTextures(1, &rt->color));
  gl(glDeleteRenderbuffers(1, &rt->depth));
  gl(glDeleteFramebuffers(1, &rt->fbo));
}
