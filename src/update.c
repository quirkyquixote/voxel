
#include "update.h"

#include "context.h"
#include "recipes.h"


int chunks_by_priority(const void *p1, const void *p2)
{
	const struct chunk *c1, *c2;
	c1 = *(const void **)p1;
	c2 = *(const void **)p2;
	return c1->priority - c2->priority;
}

void texcoord_by_material(uint8_t m, struct aab2f *tc)
{
	tc->x0 = (m % 16) / 16.;
	tc->y0 = (m / 16) / 16.;
	tc->x1 = tc->x0 + 1 / 16.;
	tc->y1 = tc->y0 + 1 / 16.;
}

static const char has_left_side[256] = {
	0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1
};

static const char has_right_side[256] = {
	0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1
};

static const char has_down_side[256] = {
	0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1
};

static const char has_up_side[256] = {
	0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1
};

static const char has_back_side[256] = {
	0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1
};

static const char has_front_side[256] = {
	0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1
};

void texcoord_from_material(int mat, struct v2f *tc)
{
	GLfloat x0 = (mat & 0xf) / 16.;
	GLfloat y0 = (mat >> 4) / 16.;
	GLfloat x1 = x0 + 1 / 16.;
	GLfloat y1 = y0 + 1 / 16.;
	tc[0] = v2f(x0, y0);
	tc[1] = v2f(x1, y0);
	tc[2] = v2f(x0, y1);
	tc[3] = v2f(x1, y1);
}

void update_cell(struct context *ctx, struct vertex_array *buf, int64_t x, int64_t y, int64_t z)
{
	int8_t s, l, d, b, m, g;
	struct v2f lt[4];
	struct v2f mt[4];

	s = WORLD_AT(ctx->w, shape, x, y, z);
	l = WORLD_AT(ctx->w, shape, x - 1, y, z);
	d = y == 0 ? 0 : WORLD_AT(ctx->w, shape, x, y - 1, z);
	b = WORLD_AT(ctx->w, shape, x, y, z - 1);
	if (has_down_side[s] && !has_up_side[d]) {
		m = WORLD_AT(ctx->w, mat, x, y, z);
		g = WORLD_AT(ctx->w, light, x, y - 1, z);
		lt[0] = texcoord_from_light(g);
		lt[1] = texcoord_from_light(g);
		lt[2] = texcoord_from_light(g);
		lt[3] = texcoord_from_light(g);
		texcoord_from_material(m, mt);
		vertex_array_down(buf, v3f(x, y, z), 1, 1, lt, mt);
	}
	if (has_up_side[d] && !has_down_side[s]) {
		m = WORLD_AT(ctx->w, mat, x, y - 1, z);
		g = WORLD_AT(ctx->w, light, x, y, z);
		lt[0] = texcoord_from_light(g);
		lt[1] = texcoord_from_light(g);
		lt[2] = texcoord_from_light(g);
		lt[3] = texcoord_from_light(g);
		texcoord_from_material(m, mt);
		vertex_array_up(buf, v3f(x, y, z), 1, 1, lt, mt);
	}
	if (has_left_side[s] && !has_right_side[l]) {
		m = WORLD_AT(ctx->w, mat, x, y, z);
		g = WORLD_AT(ctx->w, light, x - 1, y, z);
		lt[0] = texcoord_from_light(g);
		lt[1] = texcoord_from_light(g);
		lt[2] = texcoord_from_light(g);
		lt[3] = texcoord_from_light(g);
		texcoord_from_material(m, mt);
		vertex_array_left(buf, v3f(x, y, z), 1, 1, lt, mt);
	}
	if (has_right_side[l] && !has_left_side[s]) {
		m = WORLD_AT(ctx->w, mat, x - 1, y, z);
		g = WORLD_AT(ctx->w, light, x, y, z);
		lt[0] = texcoord_from_light(g);
		lt[1] = texcoord_from_light(g);
		lt[2] = texcoord_from_light(g);
		lt[3] = texcoord_from_light(g);
		texcoord_from_material(m, mt);
		vertex_array_right(buf, v3f(x, y, z), 1, 1, lt, mt);
	}
	if (has_back_side[s] && !has_front_side[b]) {
		m = WORLD_AT(ctx->w, mat, x, y, z);
		g = WORLD_AT(ctx->w, light, x, y, z - 1);
		lt[0] = texcoord_from_light(g);
		lt[1] = texcoord_from_light(g);
		lt[2] = texcoord_from_light(g);
		lt[3] = texcoord_from_light(g);
		texcoord_from_material(m, mt);
		vertex_array_back(buf, v3f(x, y, z), 1, 1, lt, mt);
	}
	if (has_front_side[b] && !has_back_side[s]) {
		m = WORLD_AT(ctx->w, mat, x, y, z - 1);
		g = WORLD_AT(ctx->w, light, x, y, z);
		lt[0] = texcoord_from_light(g);
		lt[1] = texcoord_from_light(g);
		lt[2] = texcoord_from_light(g);
		lt[3] = texcoord_from_light(g);
		texcoord_from_material(m, mt);
		vertex_array_front(buf, v3f(x, y, z), 1, 1, lt, mt);
	}
	if (s == SHAPE_SLAB_DN) {
		m = WORLD_AT(ctx->w, mat, x, y, z);
		g = WORLD_AT(ctx->w, light, x, y, z);
		lt[0] = texcoord_from_light(g);
		lt[1] = texcoord_from_light(g);
		lt[2] = texcoord_from_light(g);
		lt[3] = texcoord_from_light(g);
		texcoord_from_material(m, mt);
		vertex_array_up(buf, v3f(x, y + 0.5, z), 1, 1, lt, mt);
		vertex_array_left(buf, v3f(x, y, z), 0.5, 1, lt, mt);
		vertex_array_right(buf, v3f(x + 1, y, z), 0.5, 1, lt, mt);
		vertex_array_back(buf, v3f(x, y, z), 1, 0.5, lt, mt);
		vertex_array_front(buf, v3f(x, y, z + 1), 1, 0.5, lt, mt);
	} else if (s == SHAPE_SLAB_UP) {
		m = WORLD_AT(ctx->w, mat, x, y, z);
		g = WORLD_AT(ctx->w, light, x, y, z);
		lt[0] = texcoord_from_light(g);
		lt[1] = texcoord_from_light(g);
		lt[2] = texcoord_from_light(g);
		lt[3] = texcoord_from_light(g);
		texcoord_from_material(m, mt);
		vertex_array_down(buf, v3f(x, y + 0.5, z), 1, 1, lt, mt);
		vertex_array_left(buf, v3f(x, y + 0.5, z), 0.5, 1, lt, mt);
		vertex_array_right(buf, v3f(x + 1, y + 0.5, z), 0.5, 1, lt, mt);
		vertex_array_back(buf, v3f(x, y + 0.5, z), 1, 0.5, lt, mt);
		vertex_array_front(buf, v3f(x, y + 0.5, z + 1), 1, 0.5, lt, mt);
	} else if (s == SHAPE_STAIRS_DF) {
		m = WORLD_AT(ctx->w, mat, x, y, z);
		g = WORLD_AT(ctx->w, light, x, y, z);
		lt[0] = texcoord_from_light(g);
		lt[1] = texcoord_from_light(g);
		lt[2] = texcoord_from_light(g);
		lt[3] = texcoord_from_light(g);
		texcoord_from_material(m, mt);
		vertex_array_up(buf, v3f(x, y + 1, z + 0.5), 1, 0.5, lt, mt);
		vertex_array_up(buf, v3f(x, y + 0.5, z), 1, 0.5, lt, mt);
		vertex_array_left(buf, v3f(x, y, z), 0.5, 1, lt, mt);
		vertex_array_left(buf, v3f(x, y + 0.5, z + 0.5), 0.5, 0.5, lt, mt);
		vertex_array_right(buf, v3f(x + 1, y, z), 0.5, 1, lt, mt);
		vertex_array_right(buf, v3f(x + 1, y + 0.5, z + 0.5), 0.5, 0.5, lt, mt);
		vertex_array_back(buf, v3f(x, y, z), 1, 0.5, lt, mt);
		vertex_array_back(buf, v3f(x, y + 0.5, z + 0.5), 1, 0.5, lt, mt);
	} else if (s == SHAPE_STAIRS_DL) {
		m = WORLD_AT(ctx->w, mat, x, y, z);
		g = WORLD_AT(ctx->w, light, x, y, z);
		lt[0] = texcoord_from_light(g);
		lt[1] = texcoord_from_light(g);
		lt[2] = texcoord_from_light(g);
		lt[3] = texcoord_from_light(g);
		texcoord_from_material(m, mt);
		vertex_array_up(buf, v3f(x, y + 1, z), 0.5, 1, lt, mt);
		vertex_array_up(buf, v3f(x + 0.5, y + 0.5, z), 0.5, 1, lt, mt);
		vertex_array_right(buf, v3f(x + 1, y, z), 0.5, 1, lt, mt);
		vertex_array_right(buf, v3f(x + 0.5, y + 0.5, z), 0.5, 1, lt, mt);
		vertex_array_back(buf, v3f(x, y, z), 1, 0.5, lt, mt);
		vertex_array_back(buf, v3f(x, y + 0.5, z), 0.5, 0.5, lt, mt);
		vertex_array_front(buf, v3f(x, y, z + 1), 1, 0.5, lt, mt);
		vertex_array_front(buf, v3f(x, y + 0.5, z + 1), 0.5, 0.5, lt, mt);
	} else if (s == SHAPE_STAIRS_DB) {
		m = WORLD_AT(ctx->w, mat, x, y, z);
		g = WORLD_AT(ctx->w, light, x, y, z);
		lt[0] = texcoord_from_light(g);
		lt[1] = texcoord_from_light(g);
		lt[2] = texcoord_from_light(g);
		lt[3] = texcoord_from_light(g);
		texcoord_from_material(m, mt);
		vertex_array_up(buf, v3f(x, y + 1, z), 1, 0.5, lt, mt);
		vertex_array_up(buf, v3f(x, y + 0.5, z + 0.5), 1, 0.5, lt, mt);
		vertex_array_left(buf, v3f(x, y, z), 0.5, 1, lt, mt);
		vertex_array_left(buf, v3f(x, y + 0.5, z), 0.5, 0.5, lt, mt);
		vertex_array_right(buf, v3f(x + 1, y, z), 0.5, 1, lt, mt);
		vertex_array_right(buf, v3f(x + 1, y + 0.5, z), 0.5, 0.5, lt, mt);
		vertex_array_front(buf, v3f(x, y, z + 1), 1, 0.5, lt, mt);
		vertex_array_front(buf, v3f(x, y + 0.5, z + 0.5), 1, 0.5, lt, mt);
	} else if (s == SHAPE_STAIRS_DR) {
		m = WORLD_AT(ctx->w, mat, x, y, z);
		g = WORLD_AT(ctx->w, light, x, y, z);
		lt[0] = texcoord_from_light(g);
		lt[1] = texcoord_from_light(g);
		lt[2] = texcoord_from_light(g);
		lt[3] = texcoord_from_light(g);
		texcoord_from_material(m, mt);
		vertex_array_up(buf, v3f(x + 0.5, y + 1, z), 0.5, 1, lt, mt);
		vertex_array_up(buf, v3f(x, y + 0.5, z), 0.5, 1, lt, mt);
		vertex_array_left(buf, v3f(x, y, z), 0.5, 1, lt, mt);
		vertex_array_left(buf, v3f(x + 0.5, y + 0.5, z), 0.5, 1, lt, mt);
		vertex_array_back(buf, v3f(x, y, z), 1, 0.5, lt, mt);
		vertex_array_back(buf, v3f(x + 0.5, y + 0.5, z), 0.5, 0.5, lt, mt);
		vertex_array_front(buf, v3f(x, y, z + 1), 1, 0.5, lt, mt);
		vertex_array_front(buf, v3f(x + 0.5, y + 0.5, z + 1), 0.5, 0.5, lt, mt);
	} else if (s == SHAPE_STAIRS_UF) {
		m = WORLD_AT(ctx->w, mat, x, y, z);
		g = WORLD_AT(ctx->w, light, x, y, z);
		lt[0] = texcoord_from_light(g);
		lt[1] = texcoord_from_light(g);
		lt[2] = texcoord_from_light(g);
		lt[3] = texcoord_from_light(g);
		texcoord_from_material(m, mt);
		vertex_array_down(buf, v3f(x, y, z + 0.5), 1, 0.5, lt, mt);
		vertex_array_down(buf, v3f(x, y + 0.5, z), 1, 0.5, lt, mt);
		vertex_array_left(buf, v3f(x, y + 0.5, z), 0.5, 1, lt, mt);
		vertex_array_left(buf, v3f(x, y, z + 0.5), 0.5, 0.5, lt, mt);
		vertex_array_right(buf, v3f(x + 1, y + 0.5, z), 0.5, 1, lt, mt);
		vertex_array_right(buf, v3f(x + 1, y, z + 0.5), 0.5, 0.5, lt, mt);
		vertex_array_back(buf, v3f(x, y + 0.5, z), 1, 0.5, lt, mt);
		vertex_array_back(buf, v3f(x, y, z + 0.5), 1, 0.5, lt, mt);
	} else if (s == SHAPE_STAIRS_UL) {
		m = WORLD_AT(ctx->w, mat, x, y, z);
		g = WORLD_AT(ctx->w, light, x, y, z);
		lt[0] = texcoord_from_light(g);
		lt[1] = texcoord_from_light(g);
		lt[2] = texcoord_from_light(g);
		lt[3] = texcoord_from_light(g);
		texcoord_from_material(m, mt);
		vertex_array_down(buf, v3f(x, y, z), 0.5, 1, lt, mt);
		vertex_array_down(buf, v3f(x + 0.5, y + 0.5, z), 0.5, 1, lt, mt);
		vertex_array_right(buf, v3f(x + 1, y + 0.5, z), 0.5, 1, lt, mt);
		vertex_array_right(buf, v3f(x + 0.5, y, z), 0.5, 1, lt, mt);
		vertex_array_back(buf, v3f(x, y + 0.5, z), 1, 0.5, lt, mt);
		vertex_array_back(buf, v3f(x, y, z), 0.5, 0.5, lt, mt);
		vertex_array_front(buf, v3f(x, y + 0.5, z + 1), 1, 0.5, lt, mt);
		vertex_array_front(buf, v3f(x, y, z + 1), 0.5, 0.5, lt, mt);
	} else if (s == SHAPE_STAIRS_UB) {
		m = WORLD_AT(ctx->w, mat, x, y, z);
		g = WORLD_AT(ctx->w, light, x, y, z);
		lt[0] = texcoord_from_light(g);
		lt[1] = texcoord_from_light(g);
		lt[2] = texcoord_from_light(g);
		lt[3] = texcoord_from_light(g);
		texcoord_from_material(m, mt);
		vertex_array_down(buf, v3f(x, y, z), 1, 0.5, lt, mt);
		vertex_array_down(buf, v3f(x, y + 0.5, z + 0.5), 1, 0.5, lt, mt);
		vertex_array_left(buf, v3f(x, y + 0.5, z), 0.5, 1, lt, mt);
		vertex_array_left(buf, v3f(x, y, z), 0.5, 0.5, lt, mt);
		vertex_array_right(buf, v3f(x + 1, y + 0.5, z), 0.5, 1, lt, mt);
		vertex_array_right(buf, v3f(x + 1, y, z), 0.5, 0.5, lt, mt);
		vertex_array_front(buf, v3f(x, y + 0.5, z + 1), 1, 0.5, lt, mt);
		vertex_array_front(buf, v3f(x, y, z + 0.5), 1, 0.5, lt, mt);
	} else if (s == SHAPE_STAIRS_UR) {
		m = WORLD_AT(ctx->w, mat, x, y, z);
		g = WORLD_AT(ctx->w, light, x, y, z);
		lt[0] = texcoord_from_light(g);
		lt[1] = texcoord_from_light(g);
		lt[2] = texcoord_from_light(g);
		lt[3] = texcoord_from_light(g);
		texcoord_from_material(m, mt);
		vertex_array_down(buf, v3f(x + 0.5, y, z), 0.5, 1, lt, mt);
		vertex_array_down(buf, v3f(x, y + 0.5, z), 0.5, 1, lt, mt);
		vertex_array_left(buf, v3f(x, y + 0.5, z), 0.5, 1, lt, mt);
		vertex_array_left(buf, v3f(x + 0.5, y, z), 0.5, 1, lt, mt);
		vertex_array_back(buf, v3f(x, y + 0.5, z), 1, 0.5, lt, mt);
		vertex_array_back(buf, v3f(x + 0.5, y, z), 0.5, 0.5, lt, mt);
		vertex_array_front(buf, v3f(x, y + 0.5, z + 1), 1, 0.5, lt, mt);
		vertex_array_front(buf, v3f(x + 0.5, y, z + 1), 0.5, 0.5, lt, mt);
	} else if (s == SHAPE_SLAB_LF) {
		m = WORLD_AT(ctx->w, mat, x, y, z);
		g = WORLD_AT(ctx->w, light, x, y, z);
		lt[0] = texcoord_from_light(g);
		lt[1] = texcoord_from_light(g);
		lt[2] = texcoord_from_light(g);
		lt[3] = texcoord_from_light(g);
		texcoord_from_material(m, mt);
		vertex_array_right(buf, v3f(x + 0.5, y, z), 1, 1, lt, mt);
		vertex_array_down(buf, v3f(x, y, z), 0.5, 1, lt, mt);
		vertex_array_up(buf, v3f(x, y + 1, z), 0.5, 1, lt, mt);
		vertex_array_back(buf, v3f(x, y, z), 0.5, 1, lt, mt);
		vertex_array_front(buf, v3f(x, y, z + 1), 0.5, 1, lt, mt);
	} else if (s == SHAPE_SLAB_RT) {
		m = WORLD_AT(ctx->w, mat, x, y, z);
		g = WORLD_AT(ctx->w, light, x, y, z);
		lt[0] = texcoord_from_light(g);
		lt[1] = texcoord_from_light(g);
		lt[2] = texcoord_from_light(g);
		lt[3] = texcoord_from_light(g);
		texcoord_from_material(m, mt);
		vertex_array_left(buf, v3f(x + 0.5, y, z), 1, 1, lt, mt);
		vertex_array_down(buf, v3f(x + 0.5, y, z), 0.5, 1, lt, mt);
		vertex_array_up(buf, v3f(x + 0.5, y + 1, z), 0.5, 1, lt, mt);
		vertex_array_back(buf, v3f(x + 0.5, y, z), 0.5, 1, lt, mt);
		vertex_array_front(buf, v3f(x + 0.5, y, z + 1), 0.5, 1, lt, mt);
	} else if (s == SHAPE_SLAB_BK) {
		m = WORLD_AT(ctx->w, mat, x, y, z);
		g = WORLD_AT(ctx->w, light, x, y, z);
		lt[0] = texcoord_from_light(g);
		lt[1] = texcoord_from_light(g);
		lt[2] = texcoord_from_light(g);
		lt[3] = texcoord_from_light(g);
		texcoord_from_material(m, mt);
		vertex_array_front(buf, v3f(x, y, z + 0.5), 1, 1, lt, mt);
		vertex_array_down(buf, v3f(x, y, z), 1, 0.5, lt, mt);
		vertex_array_up(buf, v3f(x, y + 1, z), 1, 0.5, lt, mt);
		vertex_array_left(buf, v3f(x, y, z), 1, 0.5, lt, mt);
		vertex_array_right(buf, v3f(x + 1, y, z), 1, 0.5, lt, mt);
	} else if (s == SHAPE_SLAB_FT) {
		m = WORLD_AT(ctx->w, mat, x, y, z);
		g = WORLD_AT(ctx->w, light, x, y, z);
		lt[0] = texcoord_from_light(g);
		lt[1] = texcoord_from_light(g);
		lt[2] = texcoord_from_light(g);
		lt[3] = texcoord_from_light(g);
		texcoord_from_material(m, mt);
		vertex_array_back(buf, v3f(x, y, z + 0.5), 1, 1, lt, mt);
		vertex_array_down(buf, v3f(x, y, z + 0.5), 1, 0.5, lt, mt);
		vertex_array_up(buf, v3f(x, y + 1, z + 0.5), 1, 0.5, lt, mt);
		vertex_array_left(buf, v3f(x, y, z + 0.5), 1, 0.5, lt, mt);
		vertex_array_right(buf, v3f(x + 1, y, z + 0.5), 1, 0.5, lt, mt);
	} else if (s == SHAPE_PANE_Y) {
		m = WORLD_AT(ctx->w, mat, x, y, z);
		g = WORLD_AT(ctx->w, light, x, y, z);
		lt[0] = texcoord_from_light(g);
		lt[1] = texcoord_from_light(g);
		lt[2] = texcoord_from_light(g);
		lt[3] = texcoord_from_light(g);
		texcoord_from_material(m, mt);
		vertex_array_down(buf, v3f(x, y + 0.46875, z), 1, 1, lt, mt);
		vertex_array_up(buf, v3f(x, y + 0.53125, z), 1, 1, lt, mt);
		vertex_array_left(buf, v3f(x, y + 0.46875, z), 0.0625, 1, lt, mt);
		vertex_array_right(buf, v3f(x + 1, y + 0.46875, z), 0.0625, 1, lt, mt);
		vertex_array_back(buf, v3f(x, y + 0.46875, z), 1, 0.0625, lt, mt);
		vertex_array_front(buf, v3f(x, y + 0.46875, z + 1), 1, 0.0625, lt, mt);
	} else if (s == SHAPE_PANE_X) {
		m = WORLD_AT(ctx->w, mat, x, y, z);
		g = WORLD_AT(ctx->w, light, x, y, z);
		lt[0] = texcoord_from_light(g);
		lt[1] = texcoord_from_light(g);
		lt[2] = texcoord_from_light(g);
		lt[3] = texcoord_from_light(g);
		texcoord_from_material(m, mt);
		vertex_array_left(buf, v3f(x + 0.46875, y, z), 1, 1, lt, mt);
		vertex_array_right(buf, v3f(x + 0.53125, y, z), 1, 1, lt, mt);
		vertex_array_down(buf, v3f(x + 0.46875, y, z), 0.0625, 1, lt, mt);
		vertex_array_up(buf, v3f(x + 0.46875, y + 1, z), 0.0625, 1, lt, mt);
		vertex_array_back(buf, v3f(x + 0.46875, y, z), 0.0625, 1, lt, mt);
		vertex_array_front(buf, v3f(x + 0.46875, y, z + 1), 0.0625, 1, lt, mt);
	} else if (s == SHAPE_PANE_Z) {
		m = WORLD_AT(ctx->w, mat, x, y, z);
		g = WORLD_AT(ctx->w, light, x, y, z);
		lt[0] = texcoord_from_light(g);
		lt[1] = texcoord_from_light(g);
		lt[2] = texcoord_from_light(g);
		lt[3] = texcoord_from_light(g);
		texcoord_from_material(m, mt);
		vertex_array_back(buf, v3f(x, y, z + 0.46875), 1, 1, lt, mt);
		vertex_array_front(buf, v3f(x, y, z + 0.53125), 1, 1, lt, mt);
		vertex_array_down(buf, v3f(x, y, z + 0.46875), 1, 0.0625, lt, mt);
		vertex_array_up(buf, v3f(x, y + 1, z + 0.46875), 1, 0.0625, lt, mt);
		vertex_array_left(buf, v3f(x, y, z + 0.46875), 1, 0.0625, lt, mt);
		vertex_array_right(buf, v3f(x + 1, y, z + 0.46875), 1, 0.0625, lt, mt);
	} else if (s >= SHAPE_FLUID1 && s <= SHAPE_FLUID16) {
		if (d == SHAPE_NONE || (d >= SHAPE_FLUID1 && d <= SHAPE_FLUID15))
			return;
		m = WORLD_AT(ctx->w, mat, x, y, z);
		g = WORLD_AT(ctx->w, light, x, y, z);
		lt[0] = texcoord_from_light(g);
		lt[1] = texcoord_from_light(g);
		lt[2] = texcoord_from_light(g);
		lt[3] = texcoord_from_light(g);
		texcoord_from_material(0, mt);
		vertex_array_up(buf, v3f(x, y + (s - SHAPE_FLUID1 + 1) / 16., z), 1, 1, lt, mt);
	}
}

void update_vbo(struct context *ctx, int id, int64_t x0, int64_t y0, int64_t z0)
{
	int64_t x1, y1, z1;
	int64_t x, y, z;
	struct vertex_array *buf;

	x1 = x0 + SHARD_W;
	y1 = y0 + SHARD_H;
	z1 = z0 + SHARD_D;
	buf = vertex_array();

	for (x = x0; x < x1; ++x) {
		for (y = y0; y < y1; ++y) {
			for (z = z0; z < z1; ++z) {
				update_cell(ctx, buf, x, y, z);
			}
		}
	}

	vertex_buffer_update(ctx->shard_vertex_buffer, id, buf->data, buf->size);
	vertex_array_destroy(buf);
}

void use_inventory(struct context *ctx, struct inventory *inv)
{
	struct v3ll p = ctx->cur.p;
	struct v3f q = ctx->cur.q;
	int obj = ctx->inv->slots[ctx->tool].obj;
	int mat = ctx->inv->slots[ctx->tool].mat;
	int num = ctx->inv->slots[ctx->tool].num;
	int side = sqrt(inv->size);
	int slot = side * floor(q.x * side) + floor(q.z * side);
	int obj2 = inv->slots[slot].obj;
	int mat2 = inv->slots[slot].mat;
	int num2 = inv->slots[slot].num;
	if (ctx->act == 1) {
		if (ctx->move.y0) {
			if (num2 == 0) {
				printf("nothing to take\n");
				return;
			}
			int acc = inventory_add(ctx->inv, obj2, mat2, inv->slots[slot].num);
			inv->slots[slot].num -= acc;
			if (acc == 0)
				printf("no space to take\n");
			else
				printf("take %s %s %d\n", mat_names[mat], obj_names[obj], acc);
		} else {
			if (num2)
				printf("take %s %s %d\n", mat_names[mat2], obj_names[obj2], num2);
			if (num)
				printf("left %s %s %d\n", mat_names[mat], obj_names[obj], num);
			inv->slots[slot].obj = obj;
			inv->slots[slot].mat = mat;
			inv->slots[slot].num = num;
			ctx->inv->slots[ctx->tool].obj = obj2;
			ctx->inv->slots[ctx->tool].mat = mat2;
			ctx->inv->slots[ctx->tool].num = num2;
		}
	} else if (ctx->use == 1) {
		if (ctx->move.y0) {
			if (num2 == 0) {
				printf("nothing to take\n");
				return;
			}
			int acc = inventory_add(ctx->inv, obj2, mat2, 1);
			inv->slots[slot].num -= acc;
			if (acc == 0)
				printf("no space to take\n");
			else
				printf("take %s %s 1\n", mat_names[mat], obj_names[obj], acc);
		} else {
			if (num == 0) {
				printf("nothing to leave\n");
				return;
			}
			if (num2 >= 64) {
				printf("no space to leave\n");
				return;
			}
			if (num2 == 0) {
				inv->slots[slot].obj = obj;
				inv->slots[slot].mat = mat;
			} else if (obj != obj2 || mat != mat2) {
				printf("not the same object\n");
				return;
			}
			++inv->slots[slot].num;
			--ctx->inv->slots[ctx->tool].num;
			printf("left %s %s 1\n", mat_names[mat], obj_names[obj]);
		}
	}
	if (WORLD_AT(ctx->w, shape, p.x, p.y, p.z) == SHAPE_WORKBENCH) {
		const struct recipe *r;
		int i;
		for (r = recipes; r->num != 0; ++r) {
			if (recipe_match(r, inv)) {
				do {
					for (i = 0; i < 9; ++i) {
						if (inv->slots[i].num)
							--inv->slots[i].num;
					}
					inventory_add(ctx->inv, r->obj, 255, r->num);
					printf("take %s %s %d\n", mat_names[255], obj_names[r->obj], r->num);
				} while (recipe_match(r, inv));
				break;
			}
		}
	}
}

void use_tool(struct context *ctx)
{
	struct v3ll p = ctx->cur.p;
	struct v3f q = ctx->cur.q;
	int s;
	int f = ctx->cur.face;
	int obj = ctx->inv->slots[ctx->tool].obj;
	int mat = ctx->inv->slots[ctx->tool].mat;
	int num = ctx->inv->slots[ctx->tool].num;

	if (f == FACE_LF)
		p = v3_add(p, v3c(-1, 0, 0));
	else if (f == FACE_RT)
		p = v3_add(p, v3c(1, 0, 0));
	else if (f == FACE_DN)
		p = v3_add(p, v3c(0, -1, 0));
	else if (f == FACE_UP)
		p = v3_add(p, v3c(0, 1, 0));
	else if (f == FACE_BK)
		p = v3_add(p, v3c(0, 0, -1));
	else if (f == FACE_FT)
		p = v3_add(p, v3c(0, 0, 1));
	s = WORLD_AT(ctx->w, shape, p.x, p.y, p.z);
	if (!(s == SHAPE_NONE || (s >= SHAPE_FLUID1 && s <= SHAPE_FLUID16)))
		return;
	if (obj == OBJ_BLOCK) {
		world_set(ctx->w, p, SHAPE_BLOCK_DN, 255, NULL);
	} else if (obj == OBJ_SLAB) {
		if (f == FACE_UP) {
			world_set(ctx->w, p, SHAPE_SLAB_DN, 255, NULL);
		} else if (f == FACE_DN) {
			world_set(ctx->w, p, SHAPE_SLAB_UP, 255, NULL);
		} else if (q.y > 0.5) {
			world_set(ctx->w, p, SHAPE_SLAB_UP, 255, NULL);
		} else {
			world_set(ctx->w, p, SHAPE_SLAB_DN, 255, NULL);
		}
	} else if (obj == OBJ_STAIRS) {
		if (f == FACE_UP) {
			world_set(ctx->w, p, SHAPE_STAIRS_DF + (ctx->roty + 2) % 4, 255, NULL);
		} else if (f == FACE_DN) {
			world_set(ctx->w, p, SHAPE_STAIRS_UF + (ctx->roty + 2) % 4, 255, NULL);
		} else if (q.y > 0.5) {
			world_set(ctx->w, p, SHAPE_STAIRS_UF + (ctx->roty + 2) % 4, 255, NULL);
		} else {
			world_set(ctx->w, p, SHAPE_STAIRS_DF + (ctx->roty + 2) % 4, 255, NULL);
		}
	} else if (obj == OBJ_PANE) {
		if (ctx->roty & 1)
			world_set(ctx->w, p, SHAPE_PANE_X, 255, NULL);
		else
			world_set(ctx->w, p, SHAPE_PANE_Z, 255, NULL);
	} else if (obj == OBJ_WORKBENCH) {
		world_set(ctx->w, p, SHAPE_WORKBENCH, 255, inventory(9));
	} else if (obj == OBJ_CRATE) {
		world_set(ctx->w, p, SHAPE_CRATE, 255, inventory(16));
	} else if (obj == OBJ_FLUID) {
		if (s >= SHAPE_FLUID1 && s <= SHAPE_FLUID8)
			world_set(ctx->w, p, s + 8, 255, NULL);
		else if (s >= SHAPE_FLUID9 && s <= SHAPE_FLUID16)
			world_set(ctx->w, p, SHAPE_FLUID16, 255, NULL);
		else
			world_set(ctx->w, p, SHAPE_FLUID8, 255, NULL);
		flowsim_add_head(ctx->flowsim, p);
	} else if (obj == OBJ_PIPE) {
		if (f == FACE_LF || f == FACE_RT)
			world_set(ctx->w, p, SHAPE_PIPE_X, 255, inventory(1));
		else if (f == FACE_UP || f == FACE_DN)
			world_set(ctx->w, p, SHAPE_PIPE_Y, 255, inventory(1));
		else if (f == FACE_BK || f == FACE_FT)
			world_set(ctx->w, p, SHAPE_PIPE_Z, 255, inventory(1));
	}
}

void update_player(struct context *ctx)
{
	struct v3ll p = ctx->cur.p;

	if (ctx->cur.face == FACE_UP) {
		struct inventory *inv = WORLD_AT(ctx->w, data, p.x, p.y, p.z);
		if (inv != NULL) {
			use_inventory(ctx, inv);
			return;
		}
	}
	if (ctx->act == 1) {
		if (ctx->cur.face != -1) {
			if (WORLD_AT(ctx->w, shape, p.x, p.y, p.z) != 0) {
				world_set(ctx->w, p, 0, 0, NULL);
				flowsim_add_head(ctx->flowsim, p);
			}
		}
	}
	if (ctx->use == 1) {
		if (ctx->cur.face != -1) {
			use_tool(ctx);
		}
	}
	if (ctx->pick == 1) {
	}
}

void update_camera(struct context *ctx)
{
	int w, h;
	int x, y;
	int buttons;
	struct v3f r, v;

	SDL_GetWindowSize(ctx->ml->windows->sdl_window, &w, &h);
	buttons = SDL_GetMouseState(&x, &y);
	SDL_WarpMouseInWindow(ctx->ml->windows->sdl_window, w / 2, h / 2);

	r = body_get_rotation(ctx->player);
	r.y += (x - w / 2) * .005;
	r.x -= (y - h / 2) * .005;
	if (r.x < -M_PI_2 * .99)
		r.x = -M_PI_2 * .99;
	else if (r.x > M_PI_2 * .99)
		r.x = M_PI_2 *.99;
	body_set_rotation(ctx->player, r);

	v = body_get_velocity(ctx->player);
	if (ctx->move.x0 == 0 && ctx->move.x1 == 0 && ctx->move.z0 == 0 && ctx->move.z1 == 0)
		ctx->run = 0;
	v.x = (ctx->move.x1 - ctx->move.x0) * (ctx->run ? .18 : 0.15);
	v.y += (ctx->move.y1 - ctx->move.y0) * (ctx->run ? .18 : 0.15);
	v.z = (ctx->move.z1 - ctx->move.z0) * (ctx->run ? .18 : 0.15);
	v = v3_roty(v, r.y);
	body_set_velocity(ctx->player, v);
	body_set_step_size(ctx->player, 1 + ctx->run);

	camera_set_fovy(ctx->cam, (ctx->cam->fovy + (ctx->run ? 70.f : 60.f)) * 0.5);
	camera_set_position(ctx->cam, v3_add(ctx->player->p, v3f(0, .8, 0)));
	camera_set_rotation(ctx->cam, ctx->player->r);

	ctx->rotx = (unsigned int)floor(0.5 + r.x / M_PI_2) & 3;
	ctx->roty = (unsigned int)floor(0.5 + r.y / M_PI_2) & 3;

	v = v3f(0, 0, -5);
	v = v3_rotx(v, r.x);
	v = v3_roty(v, r.y);
	space_query(ctx->space, ctx->cam->p, v, &ctx->cur);
}

void update_chunks(struct context *ctx)
{
	int x, z, i, j, k;
	struct chunk *c;
	struct chunk *out_of_date[CHUNKS_PER_WORLD * CHUNKS_PER_WORLD];
	struct aab2ll bb;
	struct v3ll m;

	bb.x0 = floor(ctx->player->p.x) - CHUNK_W * CHUNKS_PER_WORLD / 2;
	bb.y0 = floor(ctx->player->p.z) - CHUNK_D * CHUNKS_PER_WORLD / 2;
	bb.x1 = bb.x0 + CHUNK_W * CHUNKS_PER_WORLD;
	bb.y1 = bb.y0 + CHUNK_D * CHUNKS_PER_WORLD;
	m.x = CHUNK_W * CHUNKS_PER_WORLD * floor(bb.x1 / (CHUNK_W * CHUNKS_PER_WORLD));
	m.y = CHUNK_D * CHUNKS_PER_WORLD * floor(bb.y1 / (CHUNK_D * CHUNKS_PER_WORLD));

	i = 0;
	for (x = 0; x < CHUNKS_PER_WORLD; ++x) {
		for (z = 0; z < CHUNKS_PER_WORLD; ++z) {
			c = ctx->w->chunks[x][z];/*
						    if (c->x < bb.x0 || c->x >= bb.x1 || c->z < bb.y0 || c->z >= bb.y1) {
						    c->up_to_date = 0;
						    c->x = m.x + x * CHUNK_W;
						    if (c->x >= bb.x1)
						    c->x -= CHUNK_W * CHUNKS_PER_WORLD;
						    c->z = m.y + z * CHUNK_D;
						    if (c->z >= bb.y1)
						    c->z -= CHUNK_D * CHUNKS_PER_WORLD;
						    if (load_chunk(c, ctx->dir) != 0)
						    terraform(0, c);
						    }*/
			if (c->flags != 0) {
				c->priority = hypot((double)c->x - ctx->player->p.x,
						(double)c->z - ctx->player->p.z);
				out_of_date[i++] = c;
			}
		}
	}
	if (i == 0)
		return;
	qsort(out_of_date, i, sizeof(*out_of_date), chunks_by_priority);
	i = i < ctx->chunks_per_tick ? i : ctx->chunks_per_tick;
	for (j = 0; j < i; ++j) {
		c = out_of_date[j];
		fprintf(stdout, "Update chunk %d (%d,%d); priority:%d", c->id, c->x, c->z, c->priority);
		if ((c->flags & CHUNK_UNLOADED) != 0) {
			printf("; load from file");
			/* load this chunk */
			c->flags ^= CHUNK_UNLOADED;
		}
		if ((c->flags & CHUNK_UNRENDERED) != 0) {
			printf("; update vertex buffers");
			for (k = 0; k < SHARDS_PER_CHUNK; ++k)
				update_vbo(ctx, c->shards[k]->id, c->x, k * SHARD_H, c->z);
			c->flags ^= CHUNK_UNRENDERED;
		}
		fprintf(stdout, "\n");
	}
}

void update(void *data)
{
	struct context *ctx = data;
	long long x, y, z;
	x = floor(ctx->cam->p.x);
	y = floor(ctx->cam->p.y);
	z = floor(ctx->cam->p.z);
	space_run(ctx->space);
	if ((ctx->tick & 1) == 0)
		flowsim_step(ctx->flowsim);
	update_player(ctx);
	update_camera(ctx);
	if (ctx->act > 0) {
		if (++ctx->act >= 8)
			ctx->act = 1;
	}
	if (ctx->use > 0) {
		if (++ctx->use >= 8)
			ctx->use = 1;
	}
	if (ctx->pick > 0) {
		if (++ctx->pick >= 8)
			ctx->pick = 1;
	}
	update_chunks(ctx);
	tone_mapper_update(ctx->tone_mapper, (WORLD_AT(ctx->w, light, x, y, z) << 4) / 255., 0);
	++ctx->tick;
}

