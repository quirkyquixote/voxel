/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "lighting.h"

#include "box2.h"
#include "v2.h"
#include "types.h"

static const int opaque_shape[] = {
	0,
	1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1,
	0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0
};

struct Lighting {
	World *w;
	std::vector<v3ll> s1;
	std::vector<v3ll> s2;
};

static inline void find_boundary(Lighting *l, const v3ll &p, int k)
{
	int k2;
	if (p.y < 0 || p.y >= WORLD_H)
		return;
	if (opaque_shape[l->w->get_shape(p)])
		return;
	k2 = l->w->get_light(p);
	if (k2 == k) {
		l->s1.push_back(p);
	} else if (k2 > k) {
		l->s2.push_back(p);
	}
}

static inline void lit_up(Lighting *l, const v3ll &p, int k, int f1, int f2)
{
	int k2;
	if (p.y < 0 || p.y >= WORLD_H) {
		return;
	}
	if (opaque_shape[l->w->get_shape(p)]) {
		return;
	}
	k2 = l->w->get_light(p);
	if (k2 < k - f1) {
		l->w->set_light(p, k - f1);
		l->s2.push_back(p);
	} else if (k < k2 - f2) {
		l->s2.push_back(p);
	}
}

void copy_value(World *w, const v3ll &p, int *k)
{
	int k2;
	if (opaque_shape[w->get_shape(p)])
		return;
	k2 = w->get_light(p);
	if (*k < k2)
		*k = k2;
}

void update_lighting(World *w, const box3ll &bb, box3ll *rbb)
{
	Lighting *l = new Lighting();
	int k;
	box3ll bb2;
	v3ll p;

	l->w = w;

	for (auto p : bb) {
		l->s1.push_back(p);
		l->s2.push_back(p);
	}

	while (!l->s1.empty()) {
		p = l->s1.back();
		l->s1.pop_back();
		k = w->get_light(p);
		if (k > 0) {
			w->set_light(p, 0);
			find_boundary(l, v3ll(p.x, p.y - 1, p.z), k);
			if (k > 1) {
				find_boundary(l, v3ll(p.x, p.y + 1, p.z), k - 1);
				find_boundary(l, v3ll(p.x - 1, p.y, p.z), k - 1);
				find_boundary(l, v3ll(p.x + 1, p.y, p.z), k - 1);
				find_boundary(l, v3ll(p.x, p.y, p.z - 1), k - 1);
				find_boundary(l, v3ll(p.x, p.y, p.z + 1), k - 1);
			}
		}
	}

	if (bb.y1 == WORLD_H) {
		box2ll top(bb.x0, bb.z0, bb.x1, bb.z1);
		for (auto &q : top)
			w->set_light(v3ll(q.x, WORLD_H - 1, q.y), 15);
	}

	if (rbb != NULL)
		*rbb = bb;
	bb2 = bb;
	while (!l->s2.empty()) {
		p = l->s2.back();
		l->s2.pop_back();
		if (p.x < bb2.x0)
			bb2.x0 = p.x;
		else if (p.x >= bb2.x1)
			bb2.x1 = p.x + 1;
		if (p.y < bb2.y0)
			bb2.y0 = p.y;
		else if (p.y >= bb2.y1)
			bb2.y1 = p.y + 1;
		if (p.z < bb2.z0)
			bb2.z0 = p.z;
		else if (p.z >= bb2.z1)
			bb2.z1 = p.z + 1;
		k = w->get_light(p);
		lit_up(l, v3ll(p.x, p.y - 1, p.z), k, 0, 1);
		lit_up(l, v3ll(p.x, p.y + 1, p.z), k, 1, 0);
		lit_up(l, v3ll(p.x - 1, p.y, p.z), k, 1, 1);
		lit_up(l, v3ll(p.x + 1, p.y, p.z), k, 1, 1);
		lit_up(l, v3ll(p.x, p.y, p.z - 1), k, 1, 1);
		lit_up(l, v3ll(p.x, p.y, p.z + 1), k, 1, 1);
	}
	delete l;

	for (auto p : bb2) {
		int s = w->get_shape(p);
		if (s == SHAPE_SLAB_DN) {
			k = w->get_light(p);
			copy_value(w, v3ll(p.x, p.y + 1, p.z), &k);
			copy_value(w, v3ll(p.x - 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x + 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x, p.y, p.z - 1), &k);
			copy_value(w, v3ll(p.x, p.y, p.z + 1), &k);
			w->set_light(p, k);
		} else if (s == SHAPE_SLAB_UP) {
			k = w->get_light(p);
			copy_value(w, v3ll(p.x, p.y - 1, p.z), &k);
			copy_value(w, v3ll(p.x - 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x + 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x, p.y, p.z - 1), &k);
			copy_value(w, v3ll(p.x, p.y, p.z + 1), &k);
			w->set_light(p, k);
		} else if (s == SHAPE_SLAB_LF) {
			k = w->get_light(p);
			copy_value(w, v3ll(p.x, p.y - 1, p.z), &k);
			copy_value(w, v3ll(p.x, p.y + 1, p.z), &k);
			copy_value(w, v3ll(p.x + 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x, p.y, p.z - 1), &k);
			copy_value(w, v3ll(p.x, p.y, p.z + 1), &k);
			w->set_light(p, k);
		} else if (s == SHAPE_SLAB_RT) {
			k = w->get_light(p);
			copy_value(w, v3ll(p.x, p.y - 1, p.z), &k);
			copy_value(w, v3ll(p.x, p.y + 1, p.z), &k);
			copy_value(w, v3ll(p.x - 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x, p.y, p.z - 1), &k);
			copy_value(w, v3ll(p.x, p.y, p.z + 1), &k);
			w->set_light(p, k);
		} else if (s == SHAPE_SLAB_BK) {
			k = w->get_light(p);
			copy_value(w, v3ll(p.x, p.y - 1, p.z), &k);
			copy_value(w, v3ll(p.x, p.y + 1, p.z), &k);
			copy_value(w, v3ll(p.x - 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x + 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x, p.y, p.z + 1), &k);
			w->set_light(p, k);
		} else if (s == SHAPE_SLAB_FT) {
			k = w->get_light(p);
			copy_value(w, v3ll(p.x, p.y - 1, p.z), &k);
			copy_value(w, v3ll(p.x, p.y + 1, p.z), &k);
			copy_value(w, v3ll(p.x - 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x + 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x, p.y, p.z - 1), &k);
			w->set_light(p, k);
		} else if (s == SHAPE_STAIRS_DF) {
			k = w->get_light(p);
			copy_value(w, v3ll(p.x, p.y + 1, p.z), &k);
			copy_value(w, v3ll(p.x - 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x + 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x, p.y, p.z - 1), &k);
			w->set_light(p, k);
		} else if (s == SHAPE_STAIRS_DB) {
			k = w->get_light(p);
			copy_value(w, v3ll(p.x, p.y + 1, p.z), &k);
			copy_value(w, v3ll(p.x - 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x + 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x, p.y, p.z + 1), &k);
			w->set_light(p, k);
		} else if (s == SHAPE_STAIRS_DL) {
			k = w->get_light(p);
			copy_value(w, v3ll(p.x, p.y + 1, p.z), &k);
			copy_value(w, v3ll(p.x + 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x, p.y, p.z - 1), &k);
			copy_value(w, v3ll(p.x, p.y, p.z + 1), &k);
			w->set_light(p, k);
		} else if (s == SHAPE_STAIRS_DR) {
			k = w->get_light(p);
			copy_value(w, v3ll(p.x, p.y + 1, p.z), &k);
			copy_value(w, v3ll(p.x - 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x, p.y, p.z - 1), &k);
			copy_value(w, v3ll(p.x, p.y, p.z + 1), &k);
			w->set_light(p, k);
		} else if (s == SHAPE_STAIRS_UF) {
			k = w->get_light(p);
			copy_value(w, v3ll(p.x, p.y - 1, p.z), &k);
			copy_value(w, v3ll(p.x - 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x + 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x, p.y, p.z - 1), &k);
			w->set_light(p, k);
		} else if (s == SHAPE_STAIRS_UB) {
			k = w->get_light(p);
			copy_value(w, v3ll(p.x, p.y - 1, p.z), &k);
			copy_value(w, v3ll(p.x - 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x + 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x, p.y, p.z + 1), &k);
			w->set_light(p, k);
		} else if (s == SHAPE_STAIRS_UL) {
			k = w->get_light(p);
			copy_value(w, v3ll(p.x, p.y - 1, p.z), &k);
			copy_value(w, v3ll(p.x + 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x, p.y, p.z - 1), &k);
			copy_value(w, v3ll(p.x, p.y, p.z + 1), &k);
			w->set_light(p, k);
		} else if (s == SHAPE_STAIRS_UR) {
			k = w->get_light(p);
			copy_value(w, v3ll(p.x, p.y - 1, p.z), &k);
			copy_value(w, v3ll(p.x, p.y + 1, p.z), &k);
			copy_value(w, v3ll(p.x - 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x, p.y, p.z - 1), &k);
			copy_value(w, v3ll(p.x, p.y, p.z + 1), &k);
			w->set_light(p, k);
		}
	}
}

