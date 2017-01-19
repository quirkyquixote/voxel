/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "physics.h"

#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <stdio.h>

#include "world.h"

class BlockGeom : public Geom {
 public:
	BlockGeom() : Geom() { }
	~BlockGeom() { }

	bool test_xpos(const v3ll &p, const box3f &bb, float *best)
	{
		box3f bb2(p.x, p.y, p.z, p.x + 1, p.y + 1, p.z + 1);
		if (!overlap(bb, bb2))
			return false;
		if (*best < bb2.x0)
			return false;
		*best = bb2.x0;
		return true;
	}

	bool test_xneg(const v3ll &p, const box3f &bb, float *best)
	{
		box3f bb2(p.x, p.y, p.z, p.x + 1, p.y + 1, p.z + 1);
		if (!overlap(bb, bb2))
			return false;
		if (*best > bb2.x1)
			return false;
		*best = bb2.x1;
		return true;
	}

	bool test_ypos(const v3ll &p, const box3f &bb, float *best)
	{
		box3f bb2(p.x, p.y, p.z, p.x + 1, p.y + 1, p.z + 1);
		if (!overlap(bb, bb2))
			return false;
		if (*best < bb2.y0)
			return false;
		*best = bb2.y0;
		return true;
	}

	bool test_yneg(const v3ll &p, const box3f &bb, float *best)
	{
		box3f bb2(p.x, p.y, p.z, p.x + 1, p.y + 1, p.z + 1);
		if (!overlap(bb, bb2))
			return false;
		if (*best > bb2.y1)
			return false;
		*best = bb2.y1;
		return true;
	}

	bool test_zpos(const v3ll &p, const box3f &bb, float *best)
	{
		box3f bb2(p.x, p.y, p.z, p.x + 1, p.y + 1, p.z + 1);
		if (!overlap(bb, bb2))
			return false;
		if (*best < bb2.z0)
			return false;
		*best = bb2.z0;
		return true;
	}

	bool test_zneg(const v3ll &p, const box3f &bb, float *best)
	{
		box3f bb2(p.x, p.y, p.z, p.x + 1, p.y + 1, p.z + 1);
		if (!overlap(bb, bb2))
			return false;
		if (*best > bb2.z1)
			return false;
		*best = bb2.z1;
		return true;
	}
};

Body::Body(Space *s)
{
}

Body::~Body()
{
}

Space::Space(World *w)
	: world(w)
{
	memset(geom, 0, sizeof(geom));
	geom[SHAPE_BLOCK_DN] = new BlockGeom();
	geom[SHAPE_BLOCK_UP] = new BlockGeom();
	geom[SHAPE_BLOCK_LF] = new BlockGeom();
	geom[SHAPE_BLOCK_RT] = new BlockGeom();
	geom[SHAPE_BLOCK_BK] = new BlockGeom();
	geom[SHAPE_BLOCK_FT] = new BlockGeom();
}

Space::~Space()
{
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
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
	0xff,
};



int Space::cell_at(const int *masks, int64_t x, int64_t y, int64_t z)
{
	int shape, mask;
	shape = world->get_shape(v3ll(x >> 1, y >> 1, z >> 1));
	mask = (1 << (((x & 1) * 4) + ((y & 1) * 2) + (z & 1)));
	return masks[shape] & mask;
}

void Space::move_xpos(Body *b, float dt)
{
	box3f bb;
	box3ll bbc;
	bool collide;
	float best;
	v3ll best_p;

	bb = b->bb;
	bb.x0 = bb.x1;
	bb.x1 += b->v.x * dt;

	bbc.x0 = floor(bb.x0);
	bbc.y0 = floor(bb.y0);
	bbc.z0 = floor(bb.z0);
	bbc.x1 = ceil(bb.x1);
	bbc.y1 = ceil(bb.y1);
	bbc.z1 = ceil(bb.z1);

	collide = false;
	best = bb.x1;

	for (auto p : bbc) {
		Geom *g = geom[world->get_shape(p)];
		if (g != nullptr && g->test_xpos(p, bb, &best)) {
			collide = true;
			best_p = p;
		}
	}
	if (collide) {
		b->v.x = 0;
		b->p.x = best - b->s.x - impulse;
		if (b->cb_func)
			b->cb_func(b, best_p, FACE_LF);
		return;
	}
	b->p.x += b->v.x * dt;
}

void Space::move_xneg(Body *b, float dt)
{
	box3f bb;
	box3ll bbc;
	bool collide;
	float best;
	v3ll best_p;

	bb = b->bb;
	bb.x1 = bb.x0;
	bb.x0 += b->v.x * dt;

	bbc.x0 = floor(bb.x0);
	bbc.y0 = floor(bb.y0);
	bbc.z0 = floor(bb.z0);
	bbc.x1 = ceil(bb.x1);
	bbc.y1 = ceil(bb.y1);
	bbc.z1 = ceil(bb.z1);

	collide = false;
	best = bb.x0;

	for (auto p : bbc) {
		Geom *g = geom[world->get_shape(p)];
		if (g != nullptr && g->test_xneg(p, bb, &best)) {
			collide = true;
			best_p = p;
		}
	}
	if (collide) {
		b->v.x = 0;
		b->p.x = best + b->s.x + impulse;
		if (b->cb_func)
			b->cb_func(b, best_p, FACE_RT);
		return;
	}
	b->p.x += b->v.x * dt;
}

void Space::move_zpos(Body *b, float dt)
{
	box3f bb;
	box3ll bbc;
	bool collide;
	float best;
	v3ll best_p;

	bb = b->bb;
	bb.z0 = bb.z1;
	bb.z1 += b->v.z * dt;

	bbc.x0 = floor(bb.x0);
	bbc.y0 = floor(bb.y0);
	bbc.z0 = floor(bb.z0);
	bbc.x1 = ceil(bb.x1);
	bbc.y1 = ceil(bb.y1);
	bbc.z1 = ceil(bb.z1);

	collide = false;
	best = bb.z1;

	for (auto p : bbc) {
		Geom *g = geom[world->get_shape(p)];
		if (g != nullptr && g->test_zpos(p, bb, &best)) {
			collide = true;
			best_p = p;
		}
	}
	if (collide) {
		b->v.z = 0;
		b->p.z = best - b->s.x - impulse;
		if (b->cb_func)
			b->cb_func(b, best_p, FACE_BK);
		return;
	}
	b->p.z += b->v.z * dt;
}

void Space::move_zneg(Body *b, float dt)
{
	box3f bb;
	box3ll bbc;
	bool collide;
	float best;
	v3ll best_p;

	bb = b->bb;
	bb.z1 = bb.z0;
	bb.z0 += b->v.z * dt;

	bbc.x0 = floor(bb.x0);
	bbc.y0 = floor(bb.y0);
	bbc.z0 = floor(bb.z0);
	bbc.x1 = ceil(bb.x1);
	bbc.y1 = ceil(bb.y1);
	bbc.z1 = ceil(bb.z1);

	collide = false;
	best = bb.z0;

	for (auto p : bbc) {
		Geom *g = geom[world->get_shape(p)];
		if (g != nullptr && g->test_zneg(p, bb, &best)) {
			collide = true;
			best_p = p;
		}
	}
	if (collide) {
		b->v.z = 0;
		b->p.z = best + b->s.x + impulse;
		if (b->cb_func)
			b->cb_func(b, best_p, FACE_FT);
		return;
	}
	b->p.z += b->v.z * dt;
}

/*
	if (step_up)
		b->p.y = 0.5 * (y0 + b->step_size) + b->s.y + impulse;
	b->p.z += b->v.z * dt;
*/

void Space::move_ypos(Body *b, float dt)
{
	box3f bb;
	box3ll bbc;
	bool collide;
	float best;
	v3ll best_p;

	bb = b->bb;
	bb.y0 = bb.y1;
	bb.y1 += b->v.y * dt;

	bbc.x0 = floor(bb.x0);
	bbc.y0 = floor(bb.y0);
	bbc.z0 = floor(bb.z0);
	bbc.x1 = ceil(bb.x1);
	bbc.y1 = ceil(bb.y1);
	bbc.z1 = ceil(bb.z1);

	collide = false;
	best = bb.y1;

	for (auto p : bbc) {
		Geom *g = geom[world->get_shape(p)];
		if (g != nullptr && g->test_ypos(p, bb, &best)) {
			collide = true;
			best_p = p;
		}
	}
	if (collide) {
		b->v.y = 0;
		b->p.y = best - b->s.y - impulse;
		if (b->cb_func)
			b->cb_func(b, best_p, FACE_DN);
		return;
	}
	b->p.y += b->v.y * dt;
}

void Space::move_yneg(Body *b, float dt)
{
	box3f bb;
	box3ll bbc;
	bool collide;
	float best;
	v3ll best_p;

	bb = b->bb;
	bb.y1 = bb.y0;
	bb.y0 += b->v.y * dt;

	bbc.x0 = floor(bb.x0);
	bbc.y0 = floor(bb.y0);
	bbc.z0 = floor(bb.z0);
	bbc.x1 = ceil(bb.x1);
	bbc.y1 = ceil(bb.y1);
	bbc.z1 = ceil(bb.z1);

	collide = false;
	best = bb.y0;

	for (auto p : bbc) {
		Geom *g = geom[world->get_shape(p)];
		if (g != nullptr && g->test_yneg(p, bb, &best)) {
			collide = true;
			best_p = p;
		}
	}
	if (collide) {
		b->v.y = 0;
		b->p.y = best + b->s.y + impulse;
		if (b->cb_func)
			b->cb_func(b, best_p, FACE_UP);
		return;
	}
	b->p.y += b->v.y * dt;
}

void Space::step(float dt)
{
	for (auto &b : bodies) {
		b->v.y += gravity * dt;
		if (b->v.x < -terminal_speed)
			b->v.x = -terminal_speed;
		else if (b->v.x > terminal_speed)
			b->v.x = terminal_speed;
		if (b->v.y < -terminal_speed)
			b->v.y = -terminal_speed;
		else if (b->v.y > terminal_speed)
			b->v.y = terminal_speed;
		if (b->v.z < -terminal_speed)
			b->v.z = -terminal_speed;
		else if (b->v.z > terminal_speed)
			b->v.z = terminal_speed;
	}

	for (auto &b : bodies) {
		b->bb.x0 = b->p.x - b->s.x;
		b->bb.x1 = b->p.x + b->s.x;
		b->bb.z0 = b->p.z - b->s.x;
		b->bb.z1 = b->p.z + b->s.x;
		b->bb.y0 = b->p.y - b->s.y;
		b->bb.y1 = b->p.y + b->s.y;
		if (b->v.x > 0)
			move_xpos(b, dt);
		else if (b->v.x < 0)
			move_xneg(b, dt);
		b->bb.x0 = b->p.x - b->s.x;
		b->bb.x1 = b->p.x + b->s.x;
		b->bb.y0 = b->p.y - b->s.y;
		b->bb.y1 = b->p.y + b->s.y;
		if (b->v.z > 0)
			move_zpos(b, dt);
		else if (b->v.z < 0)
			move_zneg(b, dt);
		b->bb.z0 = b->p.z - b->s.x;
		b->bb.z1 = b->p.z + b->s.x;
		b->bb.y0 = b->p.y - b->s.y;
		b->bb.y1 = b->p.y + b->s.y;
		if (b->v.y > 0)
			move_ypos(b, dt);
		else if (b->v.y < 0)
			move_yneg(b, dt);
		b->bb.y0 = b->p.y - b->s.y;
		b->bb.y1 = b->p.y + b->s.y;
	}
}

void Space::run()
{
	int i;
	for (i = 0; i < iterations; ++i)
		step(1.0 / iterations);
}


int Space::query_xpos(const v3f &p, const v3f &v, Query *q, float *best_t)
{
	int64_t x0, x1, x, y, z;
	float t;
	v3f p1;

	x0 = ceil(p.x * 2);
	x1 = floor((p.x + v.x) * 2);
	for (x = x0; x <= x1; ++x) {
		t = (0.5 * x - p.x) / v.x;
		if (t >= *best_t)
			return 0;
		p1 = p + v * t;
		y = floor(p1.y * 2);
		z = floor(p1.z * 2);
		if (cell_at(shape_masks, x, y, z)) {
			q->face = FACE_LF;
			q->p = v3ll(x >> 1, y >> 1, z >> 1);
			q->q = p1 - v3f(q->p);
			*best_t = t;
			return 1;
		}
	}
	return 0;
}

int Space::query_xneg(const v3f &p, const v3f &v, Query *q, float *best_t)
{
	int64_t x0, x1, x, y, z;
	float t;
	v3f p1;

	x0 = floor(p.x * 2);
	x1 = floor((p.x + v.x) * 2);
	for (x = x0; x >= x1; --x) {
		t = (0.5 * x - p.x) / v.x;
		if (t >= *best_t)
			return 0;
		p1 = p + v * t;
		y = floor(p1.y * 2);
		z = floor(p1.z * 2);
		if (cell_at(shape_masks, x - 1, y, z)) {
			q->face = FACE_RT;
			q->p = v3ll((x - 1) >> 1, y >> 1, z >> 1);
			q->q = p1 - v3f(q->p);
			*best_t = t;
			return 1;
		}
	}
	return 0;
}

int Space::query_zpos(const v3f &p, const v3f &v, Query *q, float *best_t)
{
	int64_t z0, z1, x, y, z;
	float t;
	v3f p1;

	z0 = ceil(p.z * 2);
	z1 = floor((p.z + v.z) * 2);
	for (z = z0; z <= z1; ++z) {
		t = (0.5 * z - p.z) / v.z;
		if (t >= *best_t)
			return 0;
		p1 = p + v * t;
		x = floor(p1.x * 2);
		y = floor(p1.y * 2);
		if (cell_at(shape_masks, x, y, z)) {
			q->face = FACE_BK;
			q->p = v3ll(x >> 1, y >> 1, z >> 1);
			q->q = p1 - v3f(q->p);
			*best_t = t;
			return 1;
		}
	}
	return 0;
}

int Space::query_zneg(const v3f &p, const v3f &v, Query *q, float *best_t)
{
	int64_t z0, z1, x, y, z;
	float t;
	v3f p1;

	z0 = floor(p.z * 2);
	z1 = floor((p.z + v.z) * 2);
	for (z = z0; z >= z1; --z) {
		t = (0.5 * z - p.z) / v.z;
		if (t >= *best_t)
			return 0;
		p1 = p + v * t;
		x = floor(p1.x * 2);
		y = floor(p1.y * 2);
		if (cell_at(shape_masks, x, y, z - 1)) {
			q->face = FACE_FT;
			q->p = v3ll(x >> 1, y >> 1, (z - 1) >> 1);
			q->q = p1 - v3f(q->p);
			*best_t = t;
			return 1;
		}
	}
	return 0;
}

int Space::query_ypos(const v3f &p, const v3f &v, Query *q, float *best_t)
{
	int64_t y0, y1, x, y, z;
	float t;
	v3f p1;

	y0 = ceil(p.y * 2);
	y1 = floor((p.y + v.y) * 2);
	for (y = y0; y <= y1; ++y) {
		t = (0.5 * y - p.y) / v.y;
		if (t >= *best_t)
			return 0;
		p1 = p + v * t;
		x = floor(p1.x * 2);
		z = floor(p1.z * 2);
		if (cell_at(shape_masks, x, y, z)) {
			q->face = FACE_DN;
			q->p = v3ll(x >> 1, y >> 1, z >> 1);
			q->q = p1 - v3f(q->p);
			*best_t = t;
			return 1;
		}
	}
	return 0;
}

int Space::query_yneg(const v3f &p, const v3f &v, Query *q, float *best_t)
{
	int64_t y0, y1, x, y, z;
	float t;
	v3f p1;

	y0 = floor(p.y * 2);
	y1 = floor((p.y + v.y) * 2);
	for (y = y0; y >= y1; --y) {
		t = (0.5 * y - p.y) / v.y;
		if (t >= *best_t)
			return 0;
		p1 = p + v * t;
		x = floor(p1.x * 2);
		z = floor(p1.z * 2);
		if (cell_at(shape_masks, x, y - 1, z)) {
			q->face = FACE_UP;
			q->p = v3ll(x >> 1, (y - 1) >> 1, z >> 1);
			q->q = p1 - v3f(q->p);
			*best_t = t;
			return 1;
		}
	}
	return 0;
}

int Space::query(const v3f &p, const v3f &v, Query *q)
{
	float t = FLT_MAX;

	q->face = -1;
	if (v.x < 0)
		query_xneg(p, v, q, &t);
	else if (v.x > 0)
		query_xpos(p, v, q, &t);
	if (v.y < 0)
		query_yneg(p, v, q, &t);
	else if (v.y > 0)
		query_ypos(p, v, q, &t);
	if (v.z < 0)
		query_zneg(p, v, q, &t);
	else if (v.z > 0)
		query_zpos(p, v, q, &t);
	return q->face;
}
