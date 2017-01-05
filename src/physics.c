
#include "physics.h"

#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <stdio.h>

struct body *body(struct space *s)
{
	struct body *b = calloc(1, sizeof(*b));
	list_link(&s->body, &b->list);
	return b;
}

void body_destroy(struct body *b)
{
	list_unlink(&b->list);
	free(b);
}

struct space *space(struct world *world)
{
	struct space *s = calloc(1, sizeof(*s));
	s->world = world;
	list_init(&s->body);
	return s;
}

void space_destroy(struct space *s)
{
	free(s);
}

static const int shape_masks[] = {
	0x00,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	CELL_LDB | CELL_LDF | CELL_RDB | CELL_RDF,
	CELL_LUB | CELL_LUF | CELL_RUB | CELL_RUF,
	CELL_LUB | CELL_LUF | CELL_LDB | CELL_LDF,
	CELL_RUB | CELL_RUF | CELL_RDB | CELL_RDF,
	CELL_LDB | CELL_LUB | CELL_RDB | CELL_RDB,
	CELL_LDF | CELL_LUF | CELL_RDF | CELL_RDF,
	0xff,
	0xff,
	0xff,
	CELL_LDB | CELL_LDF | CELL_RDB | CELL_RDF | CELL_LUF | CELL_RUF,
	CELL_LDB | CELL_LDF | CELL_RDB | CELL_RDF | CELL_LUF | CELL_LUB,
	CELL_LDB | CELL_LDF | CELL_RDB | CELL_RDF | CELL_LUB | CELL_RUB,
	CELL_LDB | CELL_LDF | CELL_RDB | CELL_RDF | CELL_RUF | CELL_RUB,
	CELL_LUB | CELL_LUF | CELL_RUB | CELL_RUF | CELL_LDF | CELL_RDF,
	CELL_LUB | CELL_LUF | CELL_RUB | CELL_RUF | CELL_LDF | CELL_LDB,
	CELL_LUB | CELL_LUF | CELL_RUB | CELL_RUF | CELL_LDB | CELL_RDB,
	CELL_LUB | CELL_LUF | CELL_RUB | CELL_RUF | CELL_RDF | CELL_RDB,
};

int cell_at(struct space *s, const int *masks, int64_t x, int64_t y, int64_t z)
{
	int shape, mask;
	shape = WORLD_AT(s->world, shape, x >> 1, y >> 1, z >> 1);
	mask = (1 << (((x & 1) * 4) + ((y & 1) * 2) + (z & 1)));
	return masks[shape] & mask;
}

void move_xpos(struct space *s, struct body *b, float dt)
{
	int64_t x, y, z, y0, y1, z0, z1;
	int step_up = 0;

	x = floor((b->bb.x1 + b->v.x * dt) * 2);
	y0 = floor(b->bb.y0 * 2);
	z0 = floor(b->bb.z0 * 2);
	y1 = floor(b->bb.y1 * 2);
	z1 = floor(b->bb.z1 * 2);
	for (y = y0; y <= y1; ++y) {
		for (z = z0; z <= z1; ++z) {
			if (cell_at(s, shape_masks, x, y, z)) {
				if (y == y0) {
					step_up = 1;
				} else {
					b->v.x = 0;
					b->p.x = 0.5 * x - b->s.x - s->impulse;
					return;
				}
			}
		}
	}
	if (step_up)
		b->p.y = 0.5 * (y0 + 1) + b->s.y + s->impulse;
	b->p.x += b->v.x * dt;
}

void move_xneg(struct space *s, struct body *b, float dt)
{
	int64_t x, y, z, y0, y1, z0, z1;
	int step_up = 0;

	x = floor((b->bb.x0 + b->v.x * dt) * 2);
	y0 = floor(b->bb.y0 * 2);
	z0 = floor(b->bb.z0 * 2);
	y1 = floor(b->bb.y1 * 2);
	z1 = floor(b->bb.z1 * 2);
	for (y = y0; y <= y1; ++y) {
		for (z = z0; z <= z1; ++z) {
			if (cell_at(s, shape_masks, x, y, z)) {
				if (y == y0) {
					step_up = 1;
				} else {
					b->v.x = 0;
					b->p.x = 0.5 * (x + 1) + b->s.x + s->impulse;
					return;
				}
			}
		}
	}
	if (step_up)
		b->p.y = 0.5 * (y0 + 1) + b->s.y + s->impulse;
	b->p.x += b->v.x * dt;
}

void move_zpos(struct space *s, struct body *b, float dt)
{
	int64_t x, y, z, x0, x1, y0, y1;
	int step_up = 0;

	z = floor((b->bb.z1 + b->v.z * dt) * 2);
	x0 = floor(b->bb.x0 * 2);
	y0 = floor(b->bb.y0 * 2);
	x1 = floor(b->bb.x1 * 2);
	y1 = floor(b->bb.y1 * 2);
	for (x = x0; x <= x1; ++x) {
		for (y = y0; y <= y1; ++y) {
			if (cell_at(s, shape_masks, x, y, z)) {
				if (y == y0) {
					step_up = 1;
				} else {
					b->v.z = 0;
					b->p.z = 0.5 * z - b->s.x - s->impulse;
					return;
				}
			}
		}
	}
	if (step_up)
		b->p.y = 0.5 * (y0 + 1) + b->s.y + s->impulse;
	b->p.z += b->v.z * dt;
}

void move_zneg(struct space *s, struct body *b, float dt)
{
	int64_t x, y, z, x0, x1, y0, y1;
	int step_up = 0;

	z = floor((b->bb.z0 + b->v.z * dt) * 2);
	x0 = floor(b->bb.x0 * 2);
	y0 = floor(b->bb.y0 * 2);
	x1 = floor(b->bb.x1 * 2);
	y1 = floor(b->bb.y1 * 2);
	for (x = x0; x <= x1; ++x) {
		for (y = y0; y <= y1; ++y) {
			if (cell_at(s, shape_masks, x, y, z)) {
				if (y == y0) {
					step_up = 1;
				} else {
					b->v.z = 0;
					b->p.z = 0.5 * (z + 1) + b->s.x + s->impulse;
					return;
				}
			}
		}
	}
	if (step_up)
		b->p.y = 0.5 * (y0 + 1) + b->s.y + s->impulse;
	b->p.z += b->v.z * dt;
}

void move_ypos(struct space *s, struct body *b, float dt)
{
	int64_t x, y, z, x0, x1, z0, z1;

	y = floor((b->bb.y1 + b->v.y * dt) * 2);
	x0 = floor(b->bb.x0 * 2);
	z0 = floor(b->bb.z0 * 2);
	x1 = floor(b->bb.x1 * 2);
	z1 = floor(b->bb.z1 * 2);
	for (x = x0; x <= x1; ++x) {
		for (z = z0; z <= z1; ++z) {
			if (cell_at(s, shape_masks, x, y, z)) {
				b->v.y = 0;
				b->p.y = 0.5 * y - b->s.y - s->impulse;
				return;
			}
		}
	}
	b->p.y += b->v.y * dt;
}

void move_yneg(struct space *s, struct body *b, float dt)
{
	int64_t x, y, z, x0, x1, z0, z1;

	y = floor((b->bb.y0 + b->v.y * dt) * 2);
	x0 = floor(b->bb.x0 * 2);
	z0 = floor(b->bb.z0 * 2);
	x1 = floor(b->bb.x1 * 2);
	z1 = floor(b->bb.z1 * 2);
	for (x = x0; x <= x1; ++x) {
		for (z = z0; z <= z1; ++z) {
			if (cell_at(s, shape_masks, x, y, z)) {
				b->v.y = 0;
				b->p.y = 0.5 * (y + 1) + b->s.y + s->impulse;
				return;
			}
		}
	}
	b->p.y += b->v.y * dt;
}

void space_step(struct space *s, float dt)
{
	struct body *b;

	list_foreach(b, &s->body, list) {
		b->v = v3_addx(b->v, s->gravity, dt);
		if (b->v.x < -s->terminal_speed)
			b->v.x = -s->terminal_speed;
		else if (b->v.x > s->terminal_speed)
			b->v.x = s->terminal_speed;
		if (b->v.y < -s->terminal_speed)
			b->v.y = -s->terminal_speed;
		else if (b->v.y > s->terminal_speed)
			b->v.y = s->terminal_speed;
		if (b->v.z < -s->terminal_speed)
			b->v.z = -s->terminal_speed;
		else if (b->v.z > s->terminal_speed)
			b->v.z = s->terminal_speed;
	}

	list_foreach(b, &s->body, list) {
		b->bb.x0 = b->p.x - b->s.x;
		b->bb.x1 = b->p.x + b->s.x;
		b->bb.z0 = b->p.z - b->s.x;
		b->bb.z1 = b->p.z + b->s.x;
		b->bb.y0 = b->p.y - b->s.y;
		b->bb.y1 = b->p.y + b->s.y;
		if (b->v.x > 0)
			move_xpos(s, b, dt);
		else if (b->v.x < 0)
			move_xneg(s, b, dt);
		b->bb.x0 = b->p.x - b->s.x;
		b->bb.x1 = b->p.x + b->s.x;
		b->bb.y0 = b->p.y - b->s.y;
		b->bb.y1 = b->p.y + b->s.y;
		if (b->v.z > 0)
			move_zpos(s, b, dt);
		else if (b->v.z < 0)
			move_zneg(s, b, dt);
		b->bb.z0 = b->p.z - b->s.x;
		b->bb.z1 = b->p.z + b->s.x;
		b->bb.y0 = b->p.y - b->s.y;
		b->bb.y1 = b->p.y + b->s.y;
		if (b->v.y > 0)
			move_ypos(s, b, dt);
		else if (b->v.y < 0)
			move_yneg(s, b, dt);
		b->bb.y0 = b->p.y - b->s.y;
		b->bb.y1 = b->p.y + b->s.y;
	}
}

void space_run(struct space *s)
{
	int i;
	for (i = 0; i < s->iterations; ++i)
		space_step(s, 1.0 / s->iterations);
}


int query_xpos(struct space *s, struct v3f p, struct v3f v, struct query *q, float *best_t)
{
	int64_t x0, x1, x, y, z;
	float t;

	x0 = ceil(p.x * 2);
	x1 = floor((p.x + v.x) * 2);
	for (x = x0; x <= x1; ++x) {
		t = (0.5 * x - p.x) / v.x;
		if (t >= *best_t)
			return 0;
		y = floor((p.y + v.y * t) * 2);
		z = floor((p.z + v.z * t) * 2);
		if (cell_at(s, shape_masks, x, y, z)) {
			q->cell = ((x & 1) << 2) | ((y & 1) << 1) | (z & 1);
			q->face = FACE_LF;
			q->p = v3ll(x >> 1, y >> 1, z >> 1);
			*best_t = t;
			return 1;
		}
	}
	return 0;
}

int query_xneg(struct space *s, struct v3f p, struct v3f v, struct query *q, float *best_t)
{
	int64_t x0, x1, x, y, z;
	float t;

	x0 = floor(p.x * 2);
	x1 = floor((p.x + v.x) * 2);
	for (x = x0; x >= x1; --x) {
		t = (0.5 * x - p.x) / v.x;
		if (t >= *best_t)
			return 0;
		y = floor((p.y + v.y * t) * 2);
		z = floor((p.z + v.z * t) * 2);
		if (cell_at(s, shape_masks, x - 1, y, z)) {
			q->cell = ((x & 1) << 2) | ((y & 1) << 1) | (z & 1);
			q->face = FACE_RT;
			q->p = v3ll((x - 1) >> 1, y >> 1, z >> 1);
			*best_t = t;
			return 1;
		}
	}
	return 0;
}

int query_zpos(struct space *s, struct v3f p, struct v3f v, struct query *q, float *best_t)
{
	int64_t z0, z1, x, y, z;
	float t;

	z0 = ceil(p.z * 2);
	z1 = floor((p.z + v.z) * 2);
	for (z = z0; z <= z1; ++z) {
		t = (0.5 * z - p.z) / v.z;
		if (t >= *best_t)
			return 0;
		x = floor((p.x + v.x * t) * 2);
		y = floor((p.y + v.y * t) * 2);
		if (cell_at(s, shape_masks, x, y, z)) {
			q->cell = ((x & 1) << 2) | ((y & 1) << 1) | (z & 1);
			q->face = FACE_BK;
			q->p = v3ll(x >> 1, y >> 1, z >> 1);
			*best_t = t;
			return 1;
		}
	}
	return 0;
}

int query_zneg(struct space *s, struct v3f p, struct v3f v, struct query *q, float *best_t)
{
	int64_t z0, z1, x, y, z;
	float t;

	z0 = floor(p.z * 2);
	z1 = floor((p.z + v.z) * 2);
	for (z = z0; z >= z1; --z) {
		t = (0.5 * z - p.z) / v.z;
		if (t >= *best_t)
			return 0;
		x = floor((p.x + v.x * t) * 2);
		y = floor((p.y + v.y * t) * 2);
		if (cell_at(s, shape_masks, x, y, z - 1)) {
			q->cell = ((x & 1) << 2) | ((y & 1) << 1) | (z & 1);
			q->face = FACE_FT;
			q->p = v3ll(x >> 1, y >> 1, (z - 1) >> 1);
			*best_t = t;
			return 1;
		}
	}
	return 0;
}

int query_ypos(struct space *s, struct v3f p, struct v3f v, struct query *q, float *best_t)
{
	int64_t y0, y1, x, y, z;
	float t;

	y0 = ceil(p.y * 2);
	y1 = floor((p.y + v.y) * 2);
	for (y = y0; y <= y1; ++y) {
		t = (0.5 * y - p.y) / v.y;
		if (t >= *best_t)
			return 0;
		x = floor((p.x + v.x * t) * 2);
		z = floor((p.z + v.z * t) * 2);
		if (cell_at(s, shape_masks, x, y, z)) {
			q->cell = ((x & 1) << 2) | ((y & 1) << 1) | (z & 1);
			q->face = FACE_DN;
			q->p = v3ll(x >> 1, y >> 1, z >> 1);
			*best_t = t;
			return 1;
		}
	}
	return 0;
}

int query_yneg(struct space *s, struct v3f p, struct v3f v, struct query *q, float *best_t)
{
	int64_t y0, y1, x, y, z;
	float t;

	y0 = floor(p.y * 2);
	y1 = floor((p.y + v.y) * 2);
	for (y = y0; y >= y1; --y) {
		t = (0.5 * y - p.y) / v.y;
		if (t >= *best_t)
			return 0;
		x = floor((p.x + v.x * t) * 2);
		z = floor((p.z + v.z * t) * 2);
		if (cell_at(s, shape_masks, x, y - 1, z)) {
			q->cell = ((x & 1) << 2) | ((y & 1) << 1) | (z & 1);
			q->face = FACE_UP;
			q->p = v3ll(x >> 1, (y - 1) >> 1, z >> 1);
			*best_t = t;
			return 1;
		}
	}
	return 0;
}

int space_query(struct space *s, struct v3f p, struct v3f v, struct query *q)
{
	float t = FLT_MAX;

	q->face = -1;
	if (v.x < 0)
		query_xneg(s, p, v, q, &t);
	else if (v.x > 0)
		query_xpos(s, p, v, q, &t);
	if (v.y < 0)
		query_yneg(s, p, v, q, &t);
	else if (v.y > 0)
		query_ypos(s, p, v, q, &t);
	if (v.z < 0)
		query_zneg(s, p, v, q, &t);
	else if (v.z > 0)
		query_zpos(s, p, v, q, &t);
	return q->face;
}
