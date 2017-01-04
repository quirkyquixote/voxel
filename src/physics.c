
#include "physics.h"

#include <stdlib.h>
#include <string.h>

#define MAX_JOINTS 1024

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

struct joint *joint(struct space *s)
{
	struct joint *j = calloc(1, sizeof(*j));
	list_link(&s->joint, &j->list);
	return j;
}

void joint_destroy(struct joint *j)
{
	list_unlink(&j->list);
	free(j);
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

	y = floor(b->bb.x1 + b->v.x * dt);
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
	struct body *b, *b1, *b2;
	struct joint joints[MAX_JOINTS];
	struct joint *j;
	int jointc, i;
	int64_t x0, y0, z0, x1, y1, z1, x, y, z;
	struct aab3f bb, depth;
	int mask;

	/* Update X coordinate */

	list_foreach(b, &s->body, list) {
		b->v = v3faddx(b->v, s->gravity, dt);
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



