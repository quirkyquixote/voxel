
#ifndef VOXEL_MEDIA_H_
#define VOXEL_MEDIA_H_

#define GL_GLEXT_PROTOTYPES 1
#include "SDL2/SDL_opengl.h"
#include "GL/glext.h"
#undef GL_GLEXT_PROTOTYPES

GLuint texture(const char *path);
void texture_destroy(GLuint tex);

#endif

