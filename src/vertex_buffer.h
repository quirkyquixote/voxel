
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

struct vertex {
	GLfloat x, y, z;
	GLfloat u0, v0;
	GLfloat u1, v1;
	GLubyte r;
	GLubyte g;
	GLubyte b;
	GLubyte a;
};

struct vertex_array {
	struct vertex *data;
	size_t size;
	size_t alloc;
};

struct vertex_buffer {
	size_t vbo_count;
	GLuint *vbo_names;
	size_t *vbo_sizes;
};

struct vertex_array *vertex_array(void);
void vertex_array_destroy(struct vertex_array *b);
void vertex_array_push(struct vertex_array *buf, struct v3f v, struct v2f t0, struct v2f t1, struct v4c c);
void vertex_array_left(struct vertex_array *buf, struct v3f, GLfloat h, GLfloat d, struct v2f *lt, struct v2f *mt);
void vertex_array_right(struct vertex_array *buf, struct v3f, GLfloat h, GLfloat d, struct v2f *lt, struct v2f *mt);
void vertex_array_down(struct vertex_array *buf, struct v3f, GLfloat w, GLfloat d, struct v2f *lt, struct v2f *mt);
void vertex_array_up(struct vertex_array *buf, struct v3f, GLfloat w, GLfloat d, struct v2f *lt, struct v2f *mt);
void vertex_array_back(struct vertex_array *buf, struct v3f, GLfloat w, GLfloat h, struct v2f *lt, struct v2f *mt);
void vertex_array_front(struct vertex_array *buf, struct v3f, GLfloat w, GLfloat h, struct v2f *lt, struct v2f *mt);

struct vertex_buffer *vertex_buffer(int nbufs);
void vertex_buffer_destroy(struct vertex_buffer *r);
void vertex_buffer_enable(struct vertex_buffer *r);
void vertex_buffer_draw(struct vertex_buffer *r, GLenum mode, size_t buf);
void vertex_buffer_disable(struct vertex_buffer *r);
void vertex_buffer_update(struct vertex_buffer *r, size_t buf, const void *data, size_t size);

#endif

