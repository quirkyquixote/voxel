
#include "renderer.h"

#include <stdlib.h>
#include <assert.h>

void vertex2_update_vbo(const void *data, size_t size);
void vertex2_enable_client_states(void);
void vertex2_disable_client_states(void);

void vertex3_update_vbo(const void *data, size_t size);
void vertex3_enable_client_states(void);
void vertex3_disable_client_states(void);

const struct vertex_traits vertex2_traits = {
	vertex2_update_vbo,
	vertex2_enable_client_states,
	vertex2_disable_client_states
};

const struct vertex_traits vertex3_traits = {
	vertex3_update_vbo,
	vertex3_enable_client_states,
	vertex3_disable_client_states
};

void vertex2_update_vbo(const void *data, size_t size)
{
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(struct vertex2), data, GL_STATIC_DRAW);
}

void vertex2_enable_client_states(void)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(2, GL_FLOAT, sizeof(struct vertex2), (char *)0);
	glTexCoordPointer(2, GL_FLOAT, sizeof(struct vertex2), (char *)offsetof(struct vertex2, u));
}

void vertex2_disable_client_states(void)
{
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void vertex3_update_vbo(const void *data, size_t size)
{
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(struct vertex3), data, GL_STATIC_DRAW);
}

void vertex3_enable_client_states(void)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(struct vertex3), (char *)0);
	glTexCoordPointer(2, GL_FLOAT, sizeof(struct vertex3), (char *)offsetof(struct vertex2, u));
}

void vertex3_disable_client_states(void)
{
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

struct renderer *renderer(int vbo_count, const struct vertex_traits *traits)
{
	struct renderer *r = calloc(1, sizeof(*r));
	r->traits = traits;
	r->vbo_count = vbo_count;
	r->vbo_data = calloc(vbo_count, sizeof(*r->vbo_data));
	return r;
}

void renderer_destroy(struct renderer *r)
{
	glDeleteBuffers(r->vbo_count, r->vbo_data);
	free(r->vbo_data);
	free(r);
}

void renderer_begin(struct renderer *r, size_t buf)
{
	assert(buf < r->vbo_count);
	glBindBuffer(GL_ARRAY_BUFFER, r->vbo_data[buf]);
	r->traits->enable_client_states();
}

void renderer_slice(struct renderer *r, GLenum mode, size_t offset, size_t size)
{
	glDrawArrays(mode, offset, size);
}

void renderer_end(struct renderer *r)
{
	r->traits->disable_client_states();
}

void renderer_update(struct renderer *r, size_t buf, const void *data, size_t size)
{
	assert(buf < r->vbo_count);
	glBindBuffer(GL_ARRAY_BUFFER, r->vbo_data[buf]);
	r->traits->update_vbo(data, size);
}


