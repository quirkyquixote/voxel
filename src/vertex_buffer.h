
#ifndef VOXEL_RENDERER_H_
#define VOXEL_RENDERER_H_

#define GL_GLEXT_PROTOTYPES 1
#include "SDL2/SDL_opengl.h"
#include "GL/glext.h"
#undef GL_GLEXT_PROTOTYPES

#include "v2.h"
#include "v3.h"
#include "v4.h"
#include "aab2.h"
#include "stack.h"

struct vertex {
	GLfloat x, y, z;
	GLfloat u0, v0;
	GLfloat u1, v1;
	GLubyte r;
	GLubyte g;
	GLubyte b;
	GLubyte a;
};

struct vertex_buffer {
	size_t vbo_count;
	GLuint *vbo_names;
	size_t *vbo_sizes;
};

void vertices_push(struct stack *buf, struct v3f v, struct v2f t0, struct v2f t1, struct v4c c);
void vertices_left(struct stack *buf, struct v3f, GLfloat h, GLfloat d, struct v2f *lt, struct v2f *mt);
void vertices_right(struct stack *buf, struct v3f, GLfloat h, GLfloat d, struct v2f *lt, struct v2f *mt);
void vertices_down(struct stack *buf, struct v3f, GLfloat w, GLfloat d, struct v2f *lt, struct v2f *mt);
void vertices_up(struct stack *buf, struct v3f, GLfloat w, GLfloat d, struct v2f *lt, struct v2f *mt);
void vertices_back(struct stack *buf, struct v3f, GLfloat w, GLfloat h, struct v2f *lt, struct v2f *mt);
void vertices_front(struct stack *buf, struct v3f, GLfloat w, GLfloat h, struct v2f *lt, struct v2f *mt);

struct vertex_buffer *vertex_buffer(int nbufs);
void vertex_buffer_destroy(struct vertex_buffer *r);
void vertex_buffer_enable(struct vertex_buffer *r);
void vertex_buffer_draw(struct vertex_buffer *r, GLenum mode, size_t buf);
void vertex_buffer_disable(struct vertex_buffer *r);
void vertex_buffer_update(struct vertex_buffer *r, size_t buf, const void *data, size_t size);

#endif

