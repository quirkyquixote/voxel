/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_MEDIA_H_
#define SRC_MEDIA_H_

#define GL_GLEXT_PROTOTYPES 1
#include "SDL2/SDL_opengl.h"
#include "GL/glext.h"
#undef GL_GLEXT_PROTOTYPES

GLuint texture(const char *path);
void texture_destroy(GLuint tex);

#endif  // SRC_MEDIA_H_

