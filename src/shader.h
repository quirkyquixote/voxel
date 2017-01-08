
#ifndef VOXEL_SHADER_H_
#define VOXEL_SHADER_H_

#define GL_GLEXT_PROTOTYPES 1
#include "SDL2/SDL_opengl.h"
#include "GL/glext.h"
#undef GL_GLEXT_PROTOTYPES


struct shader {
	GLuint vobj;
	GLuint fobj;
	GLuint pobj;
};

struct shader *shader(const char *vpath, const char *fpath);
void shader_destroy(struct shader *s);
int shader_enable(struct shader *s);


#endif

