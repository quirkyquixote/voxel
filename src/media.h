
#ifndef VOXEL_MEDIA_H_
#define VOXEL_MEDIA_H_

#include <SDL2/SDL_opengl.h>

GLuint texture(const char *path);
void texture_destroy(GLuint tex);

#endif

