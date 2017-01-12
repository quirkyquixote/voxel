
#include "vertex_buffer.h"

#include <stdlib.h>
#include <assert.h>

#include "lighting.h"

struct vertex_array *vertex_array(void)
{
	struct vertex_array *b = calloc(1, sizeof(*b));
	return b;
}

void vertex_array_destroy(struct vertex_array *b)
{
	if (b->data != NULL)
		free(b->data);
	free(b);
}

void vertex_array_push(struct vertex_array *buf, struct v3f p, struct v2f t0, struct v2f t1, struct v4c c)
{
	if (buf->size == buf->alloc) {
		if (buf->alloc == 0)
			buf->alloc = 2;
		else
			buf->alloc *= 2;
		buf->data = realloc(buf->data, buf->alloc * sizeof(*buf->data));
	}
	buf->data[buf->size].x = p.x;
	buf->data[buf->size].y = p.y;
	buf->data[buf->size].z = p.z;
	buf->data[buf->size].u0 = t0.x;
	buf->data[buf->size].v0 = t0.y;
	buf->data[buf->size].u1 = t1.x;
	buf->data[buf->size].v1 = t1.y;
	buf->data[buf->size].r = c.x;
	buf->data[buf->size].g = c.y;
	buf->data[buf->size].b = c.z;
	buf->data[buf->size].a = c.w;
	++buf->size;
}

void vertex_array_left(struct vertex_array *buf, struct v3f p, GLfloat h, GLfloat d, struct v2f *lt, struct v2f *mt)
{
	struct v4c c = v4c(224, 224, 224, 255);
	struct v3f p0 = v3_add(p, v3f(0, 0, 0));
	struct v3f p1 = v3_add(p, v3f(0, 0, d));
	struct v3f p2 = v3_add(p, v3f(0, h, 0));
	struct v3f p3 = v3_add(p, v3f(0, h, d));
	vertex_array_push(buf, p0, lt[0], mt[0], c);
	vertex_array_push(buf, p1, lt[1], mt[1], c);
	vertex_array_push(buf, p2, lt[2], mt[2], c);
	vertex_array_push(buf, p2, lt[2], mt[2], c);
	vertex_array_push(buf, p1, lt[1], mt[1], c);
	vertex_array_push(buf, p3, lt[3], mt[3], c);
}

void vertex_array_right(struct vertex_array *buf, struct v3f p, GLfloat h, GLfloat d, struct v2f *lt, struct v2f *mt)
{
	struct v4c c = v4c(224, 224, 224, 255);
	struct v3f p0 = v3_add(p, v3f(0, 0, d));
	struct v3f p1 = v3_add(p, v3f(0, 0, 0));
	struct v3f p2 = v3_add(p, v3f(0, h, d));
	struct v3f p3 = v3_add(p, v3f(0, h, 0));
	vertex_array_push(buf, p0, lt[0], mt[0], c);
	vertex_array_push(buf, p1, lt[1], mt[1], c);
	vertex_array_push(buf, p2, lt[2], mt[2], c);
	vertex_array_push(buf, p2, lt[2], mt[2], c);
	vertex_array_push(buf, p1, lt[1], mt[1], c);
	vertex_array_push(buf, p3, lt[3], mt[3], c);
}

void vertex_array_down(struct vertex_array *buf, struct v3f p, GLfloat w, GLfloat d, struct v2f *lt, struct v2f *mt)
{
	struct v4c c = v4c(160, 160, 160, 255);
	struct v3f p0 = v3_add(p, v3f(0, 0, 0));
	struct v3f p1 = v3_add(p, v3f(w, 0, 0));
	struct v3f p2 = v3_add(p, v3f(0, 0, d));
	struct v3f p3 = v3_add(p, v3f(w, 0, d));
	vertex_array_push(buf, p0, lt[0], mt[0], c);
	vertex_array_push(buf, p1, lt[1], mt[1], c);
	vertex_array_push(buf, p2, lt[2], mt[2], c);
	vertex_array_push(buf, p2, lt[2], mt[2], c);
	vertex_array_push(buf, p1, lt[1], mt[1], c);
	vertex_array_push(buf, p3, lt[3], mt[3], c);
}

void vertex_array_up(struct vertex_array *buf, struct v3f p, GLfloat w, GLfloat d, struct v2f *lt, struct v2f *mt)
{
	struct v4c c = v4c(255, 255, 255, 255);
	struct v3f p0 = v3_add(p, v3f(w, 0, d));
	struct v3f p1 = v3_add(p, v3f(w, 0, 0));
	struct v3f p2 = v3_add(p, v3f(0, 0, d));
	struct v3f p3 = v3_add(p, v3f(0, 0, 0));
	vertex_array_push(buf, p0, lt[0], mt[0], c);
	vertex_array_push(buf, p1, lt[1], mt[1], c);
	vertex_array_push(buf, p2, lt[2], mt[2], c);
	vertex_array_push(buf, p2, lt[2], mt[2], c);
	vertex_array_push(buf, p1, lt[1], mt[1], c);
	vertex_array_push(buf, p3, lt[3], mt[3], c);
}

void vertex_array_back(struct vertex_array *buf, struct v3f p, GLfloat w, GLfloat h, struct v2f *lt, struct v2f *mt)
{
	struct v4c c = v4c(192, 192, 192, 255);
	struct v3f p0 = v3_add(p, v3f(w, 0, 0));
	struct v3f p1 = v3_add(p, v3f(0, 0, 0));
	struct v3f p2 = v3_add(p, v3f(w, h, 0));
	struct v3f p3 = v3_add(p, v3f(0, h, 0));
	vertex_array_push(buf, p0, lt[0], mt[0], c);
	vertex_array_push(buf, p1, lt[1], mt[1], c);
	vertex_array_push(buf, p2, lt[2], mt[2], c);
	vertex_array_push(buf, p2, lt[2], mt[2], c);
	vertex_array_push(buf, p1, lt[1], mt[1], c);
	vertex_array_push(buf, p3, lt[3], mt[3], c);
}

void vertex_array_front(struct vertex_array *buf, struct v3f p, GLfloat w, GLfloat h, struct v2f *lt, struct v2f *mt)
{
	struct v4c c = v4c(192, 192, 192, 255);
	struct v3f p0 = v3_add(p, v3f(0, 0, 0));
	struct v3f p1 = v3_add(p, v3f(w, 0, 0));
	struct v3f p2 = v3_add(p, v3f(0, h, 0));
	struct v3f p3 = v3_add(p, v3f(w, h, 0));
	vertex_array_push(buf, p0, lt[0], mt[0], c);
	vertex_array_push(buf, p1, lt[1], mt[1], c);
	vertex_array_push(buf, p2, lt[2], mt[2], c);
	vertex_array_push(buf, p2, lt[2], mt[2], c);
	vertex_array_push(buf, p1, lt[1], mt[1], c);
	vertex_array_push(buf, p3, lt[3], mt[3], c);
}

struct vertex_buffer *vertex_buffer(int vbo_count)
{
	struct vertex_buffer *r = calloc(1, sizeof(*r));
	r->vbo_count = vbo_count;
	r->vbo_names = calloc(vbo_count, sizeof(*r->vbo_names));
	r->vbo_sizes = calloc(vbo_count, sizeof(*r->vbo_sizes));
	glGenBuffers(r->vbo_count, r->vbo_names);
	assert(glGetError() == GL_NO_ERROR);
	return r;
}

void vertex_buffer_destroy(struct vertex_buffer *r)
{
	assert(glGetError() == GL_NO_ERROR);
	glDeleteBuffers(r->vbo_count, r->vbo_names);
	free(r->vbo_names);
	free(r->vbo_sizes);
	free(r);
}

void vertex_buffer_enable(struct vertex_buffer *r)
{
	assert(glGetError() == GL_NO_ERROR);
}

void vertex_buffer_draw(struct vertex_buffer *r, GLenum mode, size_t buf)
{
	assert(buf < r->vbo_count);
	glBindBuffer(GL_ARRAY_BUFFER, r->vbo_names[buf]);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(struct vertex), (char *)0);
	glTexCoordPointer(4, GL_FLOAT, sizeof(struct vertex), (char *)offsetof(struct vertex, u0));
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct vertex), (char *)offsetof(struct vertex, r));
	glDrawArrays(mode, 0, r->vbo_sizes[buf]);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	assert(glGetError() == GL_NO_ERROR);
}

void vertex_buffer_disable(struct vertex_buffer *r)
{
	assert(glGetError() == GL_NO_ERROR);
}

void vertex_buffer_update(struct vertex_buffer *r, size_t buf, const void *data, size_t size)
{
	assert(glGetError() == GL_NO_ERROR);
	assert(buf < r->vbo_count);
	glBindBuffer(GL_ARRAY_BUFFER, r->vbo_names[buf]);
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(struct vertex), data, GL_STATIC_DRAW);
	r->vbo_sizes[buf] = size;
	assert(glGetError() == GL_NO_ERROR);
}


