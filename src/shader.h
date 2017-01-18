
#ifndef VOXEL_SHADER_H_
#define VOXEL_SHADER_H_

#define GL_GLEXT_PROTOTYPES 1
#include "SDL2/SDL_opengl.h"
#include "GL/glext.h"
#undef GL_GLEXT_PROTOTYPES


class Shader {
public:
	Shader(const char *vpath, const char *fpath);
	~Shader();

	inline void enable()
	{
		glUseProgram(pobj);
	}

	inline void disable()
	{
		glUseProgram(0);
	}

	inline GLint get_uniform_location(const GLchar *name)
	{
		return glGetUniformLocation(pobj, name);
	}

private:
	GLuint vobj;
	GLuint fobj;
	GLuint pobj;
};


#endif

