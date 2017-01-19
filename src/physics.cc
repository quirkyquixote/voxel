/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "physics.h"

#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <stdio.h>

#include "world.h"

Body::Body(Space *s)
{
}

Body::~Body()
{
}

Space::Space(World *w)
	: world(w)
{
	geom[SHAPE_BLOCK_DN].push_back(box3f(0., 0., 0., 1., 1., 1.));
	geom[SHAPE_BLOCK_UP].push_back(box3f(0., 0., 0., 1., 1., 1.));
	geom[SHAPE_BLOCK_LF].push_back(box3f(0., 0., 0., 1., 1., 1.));
	geom[SHAPE_BLOCK_RT].push_back(box3f(0., 0., 0., 1., 1., 1.));
	geom[SHAPE_BLOCK_BK].push_back(box3f(0., 0., 0., 1., 1., 1.));
	geom[SHAPE_BLOCK_FT].push_back(box3f(0., 0., 0., 1., 1., 1.));
	geom[SHAPE_SLAB_DN].push_back(box3f(0., 0., 0., 1., .5, 1.));
	geom[SHAPE_SLAB_UP].push_back(box3f(0., .5, 0., 1., 1., 1.));
	geom[SHAPE_SLAB_LF].push_back(box3f(0., 0., 0., .5, 1., 1.));
	geom[SHAPE_SLAB_RT].push_back(box3f(.5, 0., 0., 1., 1., 1.));
	geom[SHAPE_SLAB_BK].push_back(box3f(0., 0., 0., 1., 1., .5));
	geom[SHAPE_SLAB_FT].push_back(box3f(0., 0., .5, 1., 1., 1.));
	geom[SHAPE_STAIRS_DB].push_back(box3f(0., 0., 0., 1., .5, 1.));
	geom[SHAPE_STAIRS_DB].push_back(box3f(0., 0., 0., 1., 1., .5));
	geom[SHAPE_STAIRS_DL].push_back(box3f(0., 0., 0., 1., .5, 1.));
	geom[SHAPE_STAIRS_DL].push_back(box3f(0., 0., 0., .5, 1., 1.));
	geom[SHAPE_STAIRS_DF].push_back(box3f(0., 0., 0., 1., .5, 1.));
	geom[SHAPE_STAIRS_DF].push_back(box3f(0., 0., .5, 1., 1., 1.));
	geom[SHAPE_STAIRS_DR].push_back(box3f(0., 0., 0., 1., .5, 1.));
	geom[SHAPE_STAIRS_DR].push_back(box3f(.5, 0., 0., 1., 1., 1.));
	geom[SHAPE_STAIRS_UB].push_back(box3f(0., .5, 0., 1., 1., 1.));
	geom[SHAPE_STAIRS_UB].push_back(box3f(0., 0., 0., 1., 1., .5));
	geom[SHAPE_STAIRS_UL].push_back(box3f(0., .5, 0., 1., 1., 1.));
	geom[SHAPE_STAIRS_UL].push_back(box3f(0., 0., 0., .5, 1., 1.));
	geom[SHAPE_STAIRS_UF].push_back(box3f(0., .5, 0., 1., 1., 1.));
	geom[SHAPE_STAIRS_UF].push_back(box3f(0., 0., .5, 1., 1., 1.));
	geom[SHAPE_STAIRS_UR].push_back(box3f(0., .5, 0., 1., 1., 1.));
	geom[SHAPE_STAIRS_UR].push_back(box3f(.5, 0., 0., 1., 1., 1.));
	geom[SHAPE_PANE_X].push_back(box3f(.46875, 0., 0., .53125, 1., 1.));
	geom[SHAPE_PANE_Y].push_back(box3f(0., .46875, 0., 1., .53125, 1.));
	geom[SHAPE_PANE_Z].push_back(box3f(0., 0., .46875, 1., 1., .53125));
	geom[SHAPE_PANE_DN].push_back(box3f(0., 0., 0., 1., .0625, 1.));
	geom[SHAPE_PANE_UP].push_back(box3f(0., .9375, 0., 1., 1., 1.));
	geom[SHAPE_PANE_LF].push_back(box3f(0., 0., 0., .0625, 1., 1.));
	geom[SHAPE_PANE_RT].push_back(box3f(.9375, 0., 0., 1., 1., 1.));
	geom[SHAPE_PANE_BK].push_back(box3f(0., 0., 0., 1., 1., .0625));
	geom[SHAPE_PANE_FT].push_back(box3f(0., 0., .9375, 1., 1., 1.));
}

Space::~Space()
{
}

void Space::move_xpos(Body *b, float dt)
{
	box3f bb, bb2;
	box3ll bbc;
	bool collide;
	float best, best_y;
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
		for (auto bb2 : geom[world->get_shape(p)]) {
			bb2.x0 += p.x;
			bb2.y0 += p.y;
			bb2.z0 += p.z;
			bb2.x1 += p.x;
			bb2.y1 += p.y;
			bb2.z1 += p.z;
			if (overlap(bb, bb2) && best > bb2.x0) {
				collide = true;
				best = bb2.x0;
				best_y = bb2.y1;
				best_p = p;
			}
		}
	}
	if (collide) {
		if (best_y < b->bb.y0 + b->step_size) {
			auto p = b->p;
			b->p.y = best_y + b->s.y + impulse;
			b->bb.y0 = b->p.y - b->s.y;
			b->bb.y1 = b->p.y + b->s.y;
			move_xpos(b, dt);
			if (p.x == b->p.x) {
				b->p.y = p.y;
				b->bb.y0 = b->p.y - b->s.y;
				b->bb.y1 = b->p.y + b->s.y;
			}
			return;
		}
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
	box3f bb, bb2;
	box3ll bbc;
	bool collide;
	float best, best_y;
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
		for (auto bb2 : geom[world->get_shape(p)]) {
			bb2.x0 += p.x;
			bb2.y0 += p.y;
			bb2.z0 += p.z;
			bb2.x1 += p.x;
			bb2.y1 += p.y;
			bb2.z1 += p.z;
			if (overlap(bb, bb2) && best < bb2.x1) {
				collide = true;
				best = bb2.x1;
				best_y = bb2.y1;
				best_p = p;
			}
		}
	}
	if (collide) {
		if (best_y < b->bb.y0 + b->step_size) {
			auto p = b->p;
			b->p.y = best_y + b->s.y + impulse;
			b->bb.y0 = b->p.y - b->s.y;
			b->bb.y1 = b->p.y + b->s.y;
			move_xneg(b, dt);
			if (p.x == b->p.x) {
				b->p.y = p.y;
				b->bb.y0 = b->p.y - b->s.y;
				b->bb.y1 = b->p.y + b->s.y;
			}
			return;
		}
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
	box3f bb, bb2;
	box3ll bbc;
	bool collide;
	float best, best_y;
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
		for (auto bb2 : geom[world->get_shape(p)]) {
			bb2.x0 += p.x;
			bb2.y0 += p.y;
			bb2.z0 += p.z;
			bb2.x1 += p.x;
			bb2.y1 += p.y;
			bb2.z1 += p.z;
			if (overlap(bb, bb2) && best > bb2.z0) {
				collide = true;
				best = bb2.z0;
				best_y = bb2.y1;
				best_p = p;
			}
		}
	}
	if (collide) {
		if (best_y < b->bb.y0 + b->step_size) {
			auto p = b->p;
			b->p.y = best_y + b->s.y + impulse;
			b->bb.y0 = b->p.y - b->s.y;
			b->bb.y1 = b->p.y + b->s.y;
			move_zpos(b, dt);
			if (p.z == b->p.z) {
				b->p.y = p.y;
				b->bb.y0 = b->p.y - b->s.y;
				b->bb.y1 = b->p.y + b->s.y;
			}
			return;
		}
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
	box3f bb, bb2;
	box3ll bbc;
	bool collide;
	float best, best_y;
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
		for (auto bb2 : geom[world->get_shape(p)]) {
			bb2.x0 += p.x;
			bb2.y0 += p.y;
			bb2.z0 += p.z;
			bb2.x1 += p.x;
			bb2.y1 += p.y;
			bb2.z1 += p.z;
			if (overlap(bb, bb2) && best < bb2.z1) {
				collide = true;
				best = bb2.z1;
				best_y = bb2.y1;
				best_p = p;
			}
		}
	}
	if (collide) {
		if (best_y < b->bb.y0 + b->step_size) {
			auto p = b->p;
			b->p.y = best_y + b->s.y + impulse;
			b->bb.y0 = b->p.y - b->s.y;
			b->bb.y1 = b->p.y + b->s.y;
			move_zneg(b, dt);
			if (p.z == b->p.z) {
				b->p.y = p.y;
				b->bb.y0 = b->p.y - b->s.y;
				b->bb.y1 = b->p.y + b->s.y;
			}
			return;
		}
		b->v.z = 0;
		b->p.z = best + b->s.x + impulse;
		if (b->cb_func)
			b->cb_func(b, best_p, FACE_FT);
		return;
	}
	b->p.z += b->v.z * dt;
}

void Space::move_ypos(Body *b, float dt)
{
	box3f bb, bb2;
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
		for (auto bb2 : geom[world->get_shape(p)]) {
			bb2.x0 += p.x;
			bb2.y0 += p.y;
			bb2.z0 += p.z;
			bb2.x1 += p.x;
			bb2.y1 += p.y;
			bb2.z1 += p.z;
			if (overlap(bb, bb2) && best > bb2.y0) {
				collide = true;
				best = bb2.y0;
				best_p = p;
			}
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
	box3f bb, bb2;
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
		for (auto bb2 : geom[world->get_shape(p)]) {
			bb2.x0 += p.x;
			bb2.y0 += p.y;
			bb2.z0 += p.z;
			bb2.x1 += p.x;
			bb2.y1 += p.y;
			bb2.z1 += p.z;
			if (overlap(bb, bb2) && best < bb2.y1) {
				collide = true;
				best = bb2.y1;
				best_p = p;
			}
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
		if (b->v.z > 0)
			move_zpos(b, dt);
		else if (b->v.z < 0)
			move_zneg(b, dt);
		b->bb.z0 = b->p.z - b->s.x;
		b->bb.z1 = b->p.z + b->s.x;
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


void Space::query_xpos(const v3f &p, const v3f &v, Query *q, float *best_t)
{
	box3f bb;
	box3ll bbc;

	bb = fix(box3f(p.x, p.y, p.z, p.x + v.x, p.y + v.y, p.z + v.z));
	bbc.x0 = floor(bb.x0);
	bbc.y0 = floor(bb.y0);
	bbc.z0 = floor(bb.z0);
	bbc.x1 = ceil(bb.x1) + 1;
	bbc.y1 = ceil(bb.y1) + 1;
	bbc.z1 = ceil(bb.z1) + 1;

	for (auto c : bbc) {
		for (auto bb2 : geom[world->get_shape(c)]) {
			bb2.x0 += c.x;
			bb2.y0 += c.y;
			bb2.z0 += c.z;
			bb2.x1 += c.x;
			bb2.y1 += c.y;
			bb2.z1 += c.z;
			float t = (bb2.x0 - p.x) / v.x;
			if (t < *best_t) {
				float y = p.y + t * v.y;
				float z = p.z + t * v.z;
				if (y >= bb2.y0 && y <= bb2.y1 && z >= bb2.z0 && z <= bb2.z1) {
					q->face = FACE_LF;
					q->p = c;
					q->q = p + v * t - v3f(c);
					*best_t = t;
				}
			}
		}
	}
}

void Space::query_xneg(const v3f &p, const v3f &v, Query *q, float *best_t)
{
	box3f bb;
	box3ll bbc;

	bb = fix(box3f(p.x, p.y, p.z, p.x + v.x, p.y + v.y, p.z + v.z));
	bbc.x0 = floor(bb.x0);
	bbc.y0 = floor(bb.y0);
	bbc.z0 = floor(bb.z0);
	bbc.x1 = ceil(bb.x1) + 1;
	bbc.y1 = ceil(bb.y1) + 1;
	bbc.z1 = ceil(bb.z1) + 1;

	for (auto c : bbc) {
		for (auto bb2 : geom[world->get_shape(c)]) {
			bb2.x0 += c.x;
			bb2.y0 += c.y;
			bb2.z0 += c.z;
			bb2.x1 += c.x;
			bb2.y1 += c.y;
			bb2.z1 += c.z;
			float t = (bb2.x1 - p.x) / v.x;
			if (t < *best_t) {
				float y = p.y + t * v.y;
				float z = p.z + t * v.z;
				if (y >= bb2.y0 && y <= bb2.y1 && z >= bb2.z0 && z <= bb2.z1) {
					q->face = FACE_RT;
					q->p = c;
					q->q = p + v * t - v3f(c);
					*best_t = t;
				}
			}
		}
	}
}

void Space::query_zpos(const v3f &p, const v3f &v, Query *q, float *best_t)
{
	box3f bb;
	box3ll bbc;

	bb = fix(box3f(p.x, p.y, p.z, p.x + v.x, p.y + v.y, p.z + v.z));
	bbc.x0 = floor(bb.x0);
	bbc.y0 = floor(bb.y0);
	bbc.z0 = floor(bb.z0);
	bbc.x1 = ceil(bb.x1) + 1;
	bbc.y1 = ceil(bb.y1) + 1;
	bbc.z1 = ceil(bb.z1) + 1;

	for (auto c : bbc) {
		for (auto bb2 : geom[world->get_shape(c)]) {
			bb2.x0 += c.x;
			bb2.y0 += c.y;
			bb2.z0 += c.z;
			bb2.x1 += c.x;
			bb2.y1 += c.y;
			bb2.z1 += c.z;
			float t = (bb2.z0 - p.z) / v.z;
			if (t < *best_t) {
				float y = p.y + t * v.y;
				float x = p.x + t * v.x;
				if (y >= bb2.y0 && y <= bb2.y1 && x >= bb2.x0 && x <= bb2.x1) {
					q->face = FACE_BK;
					q->p = c;
					q->q = p + v * t - v3f(c);
					*best_t = t;
				}
			}
		}
	}
}

void Space::query_zneg(const v3f &p, const v3f &v, Query *q, float *best_t)
{
	box3f bb;
	box3ll bbc;

	bb = fix(box3f(p.x, p.y, p.z, p.x + v.x, p.y + v.y, p.z + v.z));
	bbc.x0 = floor(bb.x0);
	bbc.y0 = floor(bb.y0);
	bbc.z0 = floor(bb.z0);
	bbc.x1 = ceil(bb.x1) + 1;
	bbc.y1 = ceil(bb.y1) + 1;
	bbc.z1 = ceil(bb.z1) + 1;

	for (auto c : bbc) {
		for (auto bb2 : geom[world->get_shape(c)]) {
			bb2.x0 += c.x;
			bb2.y0 += c.y;
			bb2.z0 += c.z;
			bb2.x1 += c.x;
			bb2.y1 += c.y;
			bb2.z1 += c.z;
			float t = (bb2.z1 - p.z) / v.z;
			if (t < *best_t) {
				float y = p.y + t * v.y;
				float x = p.x + t * v.x;
				if (y >= bb2.y0 && y <= bb2.y1 && x >= bb2.x0 && x <= bb2.x1) {
					q->face = FACE_FT;
					q->p = c;
					q->q = p + v * t - v3f(c);
					*best_t = t;
				}
			}
		}
	}
}

void Space::query_ypos(const v3f &p, const v3f &v, Query *q, float *best_t)
{
	box3f bb;
	box3ll bbc;

	bb = fix(box3f(p.x, p.y, p.z, p.x + v.x, p.y + v.y, p.z + v.z));
	bbc.x0 = floor(bb.x0);
	bbc.y0 = floor(bb.y0);
	bbc.z0 = floor(bb.z0);
	bbc.x1 = ceil(bb.x1) + 1;
	bbc.y1 = ceil(bb.y1) + 1;
	bbc.z1 = ceil(bb.z1) + 1;

	for (auto c : bbc) {
		for (auto bb2 : geom[world->get_shape(c)]) {
			bb2.x0 += c.x;
			bb2.y0 += c.y;
			bb2.z0 += c.z;
			bb2.x1 += c.x;
			bb2.y1 += c.y;
			bb2.z1 += c.z;
			float t = (bb2.y0 - p.y) / v.y;
			if (t < *best_t) {
				float x = p.x + t * v.x;
				float z = p.z + t * v.z;
				if (x >= bb2.x0 && x <= bb2.x1 && z >= bb2.z0 && z <= bb2.z1) {
					q->face = FACE_DN;
					q->p = c;
					q->q = p + v * t - v3f(c);
					*best_t = t;
				}
			}
		}
	}
}

void Space::query_yneg(const v3f &p, const v3f &v, Query *q, float *best_t)
{
	box3f bb;
	box3ll bbc;

	bb = fix(box3f(p.x, p.y, p.z, p.x + v.x, p.y + v.y, p.z + v.z));
	bbc.x0 = floor(bb.x0);
	bbc.y0 = floor(bb.y0);
	bbc.z0 = floor(bb.z0);
	bbc.x1 = ceil(bb.x1) + 1;
	bbc.y1 = ceil(bb.y1) + 1;
	bbc.z1 = ceil(bb.z1) + 1;

	for (auto c : bbc) {
		for (auto bb2 : geom[world->get_shape(c)]) {
			bb2.x0 += c.x;
			bb2.y0 += c.y;
			bb2.z0 += c.z;
			bb2.x1 += c.x;
			bb2.y1 += c.y;
			bb2.z1 += c.z;
			float t = (bb2.y1 - p.y) / v.y;
			if (t < *best_t) {
				float x = p.x + t * v.x;
				float z = p.z + t * v.z;
				if (x >= bb2.x0 && x <= bb2.x1 && z >= bb2.z0 && z <= bb2.z1) {
					q->face = FACE_UP;
					q->p = c;
					q->q = p + v * t - v3f(c);
					*best_t = t;
				}
			}
		}
	}
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
