
#include "shader.h"

#include <stdio.h>
#include <stdlib.h>

int load_shader(GLuint type, const char *path, GLuint *obj)
{
	FILE *f;
	GLchar *buf;
	GLint len;
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
	buf = calloc(len + 1, sizeof(*buf));
	fread(buf, sizeof(*buf), len, f);
	fclose(f);
	*obj = glCreateShader(type);
	glShaderSourceARB(*obj, 1, (void *)&buf, &len);
	free(buf);
	glGetShaderInfoLog(*obj, sizeof(lbuf), &llen, lbuf);
	if (llen == 0)
		return 0;
	fprintf(stderr, "%s: %s\n", __func__, lbuf);
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
	fprintf(stderr, "%s: %s\n", __func__, lbuf);
	return -1;
}

struct shader *shader(const char *vpath, const char *fpath)
{
	GLuint vobj, fobj, pobj;
	if (load_shader(GL_VERTEX_SHADER, vpath, &vobj) != 0)
		return NULL;
	if (load_shader(GL_FRAGMENT_SHADER, fpath, &fobj) != 0)
		return NULL;
	if (link_shaders(vobj, fobj, &pobj) != 0)
		return NULL;
	struct shader *s = calloc(1, sizeof(*s));
	s->vobj = vobj;
	s->fobj = fobj;
	s->pobj = pobj;
	return s;
}

int shader_enable(struct shader *s)
{
	if (s == NULL)
		glUseProgram(0);
	else
		glUseProgram(s->pobj);
	return 0;
}

