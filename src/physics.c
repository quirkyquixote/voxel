
#include "physics.h"

#include <stdlib.h>
#include <string.h>
#include <float.h>

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

void move_xpos(struct space *s, struct body *b, float dt)
{
	int64_t x, y, z, y0, y1, z0, z1;

	x = floor(b->bb.x1 + b->v.x * dt);
	y0 = floor(b->bb.y0);
	z0 = floor(b->bb.z0);
	y1 = floor(b->bb.y1);
	z1 = floor(b->bb.z1);
	for (y = y0; y <= y1; ++y) {
		for (z = z0; z <= z1; ++z) {
			if (WORLD_AT(s->world, shape, x, y, z) == 1) {
				b->v.x = 0;
				b->p.x = x - b->s.x - s->impulse;
				return;
			}
		}
	}
	b->p.x += b->v.x * dt;
}

void move_xneg(struct space *s, struct body *b, float dt)
{
	int64_t x, y, z, y0, y1, z0, z1;

	x = floor(b->bb.x0 + b->v.x * dt);
	y0 = floor(b->bb.y0);
	z0 = floor(b->bb.z0);
	y1 = floor(b->bb.y1);
	z1 = floor(b->bb.z1);
	for (y = y0; y <= y1; ++y) {
		for (z = z0; z <= z1; ++z) {
			if (WORLD_AT(s->world, shape, x, y, z) == 1) {
				b->v.x = 0;
				b->p.x = x + b->s.x + 1 + s->impulse;
				return;
			}
		}
	}
	b->p.x += b->v.x * dt;
}

void move_zpos(struct space *s, struct body *b, float dt)
{
	int64_t x, y, z, x0, x1, y0, y1;

	z = floor(b->bb.z1 + b->v.z * dt);
	x0 = floor(b->bb.x0);
	y0 = floor(b->bb.y0);
	x1 = floor(b->bb.x1);
	y1 = floor(b->bb.y1);
	for (x = x0; x <= x1; ++x) {
		for (y = y0; y <= y1; ++y) {
			if (WORLD_AT(s->world, shape, x, y, z) == 1) {
				b->v.z = 0;
				b->p.z = z - b->s.x - s->impulse;
				return;
			}
		}
	}
	b->p.z += b->v.z * dt;
}

void move_zneg(struct space *s, struct body *b, float dt)
{
	int64_t x, y, z, x0, x1, y0, y1;

	z = floor(b->bb.z0 + b->v.z * dt);
	x0 = floor(b->bb.x0);
	y0 = floor(b->bb.y0);
	x1 = floor(b->bb.x1);
	y1 = floor(b->bb.y1);
	for (x = x0; x <= x1; ++x) {
		for (y = y0; y <= y1; ++y) {
			if (WORLD_AT(s->world, shape, x, y, z) == 1) {
				b->v.z = 0;
				b->p.z = z + b->s.x + 1 + s->impulse;
				return;
			}
		}
	}
	b->p.z += b->v.z * dt;
}

void move_ypos(struct space *s, struct body *b, float dt)
{
	int64_t x, y, z, x0, x1, z0, z1;

	y = floor(b->bb.y1 + b->v.y * dt);
	x0 = floor(b->bb.x0);
	z0 = floor(b->bb.z0);
	x1 = floor(b->bb.x1);
	z1 = floor(b->bb.z1);
	for (x = x0; x <= x1; ++x) {
		for (z = z0; z <= z1; ++z) {
			if (WORLD_AT(s->world, shape, x, y, z) == 1) {
				b->v.y = 0;
				b->p.y = y - b->s.y - s->impulse;
				return;
			}
		}
	}
	b->p.y += b->v.y * dt;
}

void move_yneg(struct space *s, struct body *b, float dt)
{
	int64_t x, y, z, x0, x1, z0, z1;

	y = floor(b->bb.y0 + b->v.y * dt);
	x0 = floor(b->bb.x0);
	z0 = floor(b->bb.z0);
	x1 = floor(b->bb.x1);
	z1 = floor(b->bb.z1);
	for (x = x0; x <= x1; ++x) {
		for (z = z0; z <= z1; ++z) {
			if (WORLD_AT(s->world, shape, x, y, z) == 1) {
				b->v.y = 0;
				b->p.y = y + b->s.y + 1 + s->impulse;
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
		if (b->v.z > 0)
			move_zpos(s, b, dt);
		else if (b->v.z < 0)
			move_zneg(s, b, dt);
		b->bb.z0 = b->p.z - b->s.x;
		b->bb.z1 = b->p.z + b->s.x;
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


int test_distance1(float p, float v, float r, float t0, float t1, float *t)
{
	if ((p + v * t0) * v >= 0)
		return 0;
	if (v < 0)
		*t = (r - p) / v;
	else
		*t = (-r - p) / v;
	return (*t >= t0 && *t < t1);
}

int query_xpos(struct space *s, struct v3f p, struct v3f v, struct v3ll *q, float *best_t)
{
	int64_t x0, x1, x, y, z;
	float t;

	x0 = floor(p.x);
	x1 = floor(p.x + v.x);
	for (x = x0; x <= x1; ++x) {
		t = (x - p.x) / v.x;
		if (t >= *best_t)
			return 0;
		y = floor(p.y + v.y * t);
		z = floor(p.z + v.z * t);
		if (WORLD_AT(s->world, shape, x, y, z) == 1) {
			*q = v3ll(x, y, z);
			*best_t = t;
			return 1;
		}
	}
	return 0;
}

int query_xneg(struct space *s, struct v3f p, struct v3f v, struct v3ll *q, float *best_t)
{
	int64_t x0, x1, x, y, z;
	float t;

	x0 = floor(p.x);
	x1 = floor(p.x + v.x);
	for (x = x0; x >= x1; --x) {
		t = (x - p.x) / v.x;
		if (t >= *best_t)
			return 0;
		y = floor(p.y + v.y * t);
		z = floor(p.z + v.z * t);
		if (WORLD_AT(s->world, shape, x - 1, y, z) == 1) {
			*q = v3ll(x - 1, y, z);
			*best_t = t;
			return 1;
		}
	}
	return 0;
}

int query_zpos(struct space *s, struct v3f p, struct v3f v, struct v3ll *q, float *best_t)
{
	int64_t z0, z1, x, y, z;
	float t;

	z0 = floor(p.z);
	z1 = floor(p.z + v.z);
	for (z = z0; z <= z1; ++z) {
		t = (z - p.z) / v.z;
		if (t >= *best_t)
			return 0;
		x = floor(p.x + v.x * t);
		y = floor(p.y + v.y * t);
		if (WORLD_AT(s->world, shape, x, y, z) == 1) {
			*q = v3ll(x, y, z);
			*best_t = t;
			return 1;
		}
	}
	return 0;
}

int query_zneg(struct space *s, struct v3f p, struct v3f v, struct v3ll *q, float *best_t)
{
	int64_t z0, z1, x, y, z;
	float t;

	z0 = floor(p.z);
	z1 = floor(p.z + v.z);
	for (z = z0; z >= z1; --z) {
		t = (z - p.z) / v.z;
		if (t >= *best_t)
			return 0;
		x = floor(p.x + v.x * t);
		y = floor(p.y + v.y * t);
		if (WORLD_AT(s->world, shape, x, y, z - 1) == 1) {
			*q = v3ll(x, y, z - 1);
			*best_t = t;
			return 1;
		}
	}
	return 0;
}

int query_ypos(struct space *s, struct v3f p, struct v3f v, struct v3ll *q, float *best_t)
{
	int64_t y0, y1, x, y, z;
	float t;

	y0 = floor(p.y);
	y1 = floor(p.y + v.y);
	for (y = y0; y <= y1; ++y) {
		t = (y - p.y) / v.y;
		if (t >= *best_t)
			return 0;
		x = floor(p.x + v.x * t);
		z = floor(p.z + v.z * t);
		if (WORLD_AT(s->world, shape, x, y, z) == 1) {
			*q = v3ll(x, y, z);
			*best_t = t;
			return 1;
		}
	}
	return 0;
}

int query_yneg(struct space *s, struct v3f p, struct v3f v, struct v3ll *q, float *best_t)
{
	int64_t y0, y1, x, y, z;
	float t;

	y0 = floor(p.y);
	y1 = floor(p.y + v.y);
	for (y = y0; y >= y1; --y) {
		t = (y - p.y) / v.y;
		if (t >= *best_t)
			return 0;
		x = floor(p.x + v.x * t);
		z = floor(p.z + v.z * t);
		if (WORLD_AT(s->world, shape, x, y - 1, z) == 1) {
			*q = v3ll(x, y - 1, z);
			*best_t = t;
			return 1;
		}
	}
	return 0;
}

int space_query(struct space *s, struct v3f p, struct v3f v, struct v3ll *q)
{
	float t = FLT_MAX;
	int ret = 0;

	if (v.x < 0) {
		if (query_xneg(s, p, v, q, &t))
			ret = QUERY_FACE_LF;
	} else if (v.x > 0) {
		if (query_xpos(s, p, v, q, &t))
			ret = QUERY_FACE_RT;
	}
	if (v.y < 0) {
		if (query_yneg(s, p, v, q, &t))
			ret = QUERY_FACE_DN;
	} else if (v.y > 0) {
		if (query_ypos(s, p, v, q, &t))
			ret = QUERY_FACE_UP;
	}
	if (v.z < 0) {
		if (query_zneg(s, p, v, q, &t))
			ret = QUERY_FACE_BK;
	} else if (v.z > 0) {
		if (query_zpos(s, p, v, q, &t))
			ret = QUERY_FACE_FT;
	}
	return ret;
}
