/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "shader.h"

#include <stdio.h>
#include <stdlib.h>

#include "context.h"

int load_shader(GLuint type, const char *path, GLuint *obj)
{
	FILE *f;
	GLint len;
	GLchar *buf;
	GLchar lbuf[1024];
	GLint llen;

	f = fopen(path, "r");
	if (f == NULL) {
		perror(path);
		return -1;
	}
	fseek(f, 0, SEEK_END);
	len = ftell(f);
	fseek(f, 0, SEEK_SET);
	buf = new GLchar[len + 1];
	fread(buf, sizeof(*buf), len, f);
	fclose(f);
	*obj = glCreateShader(type);
	glShaderSource(*obj, 1, const_cast<const GLchar **>(&buf), &len);
	delete [] buf;
	glGetShaderInfoLog(*obj, sizeof(lbuf), &llen, lbuf);
	if (llen == 0)
		return 0;
	log_error("%s", lbuf);
	return -1;
}

int link_shaders(GLuint vobj, GLuint fobj, GLuint *pobj)
{
	GLchar lbuf[1024];
	GLint llen;

	*pobj = glCreateProgram();
	glAttachShader(*pobj, vobj);
	glAttachShader(*pobj, fobj);
	glLinkProgram(*pobj);
	glGetProgramInfoLog(*pobj, sizeof(lbuf), &llen, lbuf);
	if (llen == 0)
		return 0;
	log_error("%s", lbuf);
	return -1;
}

Shader::Shader(const char *vpath, const char *fpath)
{
	if (load_shader(GL_VERTEX_SHADER, vpath, &vobj) != 0)
		abort();
	if (load_shader(GL_FRAGMENT_SHADER, fpath, &fobj) != 0)
		abort();
	if (link_shaders(vobj, fobj, &pobj) != 0)
		abort();
}

Shader::~Shader()
{
}

