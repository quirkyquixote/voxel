/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "lighting.h"

#include <queue>

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

Lighting::Lighting(World *w)
	: w(w)
{
}

Lighting::~Lighting()
{
}

void Lighting::init(const box2ll &bb)
{
	std::queue<v3ll> queue;

	for (auto p : bb) {
		int k = 15;
		int opaque1 = -1, opaque2;
		v3ll q(p.x, World::H - 1, p.y);
		while (q.y > 0) {
			opaque2 = opaque_shape[w->get_shape(q)];
			if (opaque2) {
				k = 0;
			} else if (opaque1 == 1) {
				queue.push(q);
			}
			opaque1 = opaque2;
			w->set_light(q, k);
			--q.y;
		}
	}

	while (!queue.empty()) {
		v3ll p = queue.front();
		queue.pop();
		v3ll pl = p - v3ll(1, 0, 0);
		v3ll pr = p + v3ll(1, 0, 0);
		v3ll pd = p - v3ll(0, 1, 0);
		v3ll pu = p + v3ll(0, 1, 0);
		v3ll pb = p - v3ll(0, 0, 1);
		v3ll pf = p + v3ll(0, 0, 1);
		int k = w->get_light(p);
		int kl = (p.x > bb.x0) ? w->get_light(pl) : k;
		int kr = (p.x < bb.x1) ? w->get_light(pr) : k;
		int kd = (p.y > 0) ? w->get_light(pd) : k;
		int ku = (p.y < World::H - 1) ? w->get_light(pu) : k;
		int kb = (p.z > bb.y0) ? w->get_light(pb) : k;
		int kf = (p.z < bb.y1) ? w->get_light(pf) : k;

		if (k < kl - 1)
			k = kl - 1;
		if (k < kr - 1)
			k = kr - 1;
		if (k < ku - 1)
			k = ku - 1;
		if (k < kd - 1)
			k = kd - 1;
		if (k < kb - 1)
			k = kb - 1;
		if (k < kf - 1)
			k = kf - 1;
		w->set_light(p, k);

		if (p.x > bb.x0 && kl < k - 1 && !opaque_shape[w->get_shape(pl)]) {
			w->set_light(pl, k - 1);
			queue.push(pl);
		}
		if (p.x < bb.x1 && kr < k - 1 && !opaque_shape[w->get_shape(pr)]) {
			w->set_light(pr, k - 1);
			queue.push(pr);
		}
		if (p.y > 0 && kd < k - 1 && !opaque_shape[w->get_shape(pd)]) {
			w->set_light(pd, k - 1);
			queue.push(pd);
		}
		if (p.y < World::H && ku < k - 1 && !opaque_shape[w->get_shape(pu)]) {
			w->set_light(pu, k - 1);
			queue.push(pu);
		}
		if (p.z > bb.y0 && kb < k - 1 && !opaque_shape[w->get_shape(pb)]) {
			w->set_light(pb, k - 1);
			queue.push(pb);
		}
		if (p.z < bb.y1 && kf < k - 1 && !opaque_shape[w->get_shape(pf)]) {
			w->set_light(pf, k - 1);
			queue.push(pf);
		}
	}

	strange_blocks(box3ll(bb.x0, 0, bb.y0, bb.x1, World::H - 1, bb.y1));
}

void Lighting::find_boundary(const v3ll &p, int k)
{
	int k2;
	if (p.y < 0 || p.y >= World::H)
		return;
	if (opaque_shape[w->get_shape(p)])
		return;
	k2 = w->get_light(p);
	if (k2 == k) {
		s1.push_back(p);
	} else if (k2 > k) {
		s2.push_back(p);
	}
}

void Lighting::lit_up(const v3ll &p, int k, int f1, int f2)
{
	int k2;
	if (p.y < 0 || p.y >= World::H) {
		return;
	}
	if (opaque_shape[w->get_shape(p)]) {
		return;
	}
	k2 = w->get_light(p);
	if (k2 < k - f1) {
		w->set_light(p, k - f1);
		s2.push_back(p);
	} else if (k < k2 - f2) {
		s2.push_back(p);
	}
}

static inline void copy_value(World *w, const v3ll &p, int *k)
{
	int k2;
	if (opaque_shape[w->get_shape(p)])
		return;
	k2 = w->get_light(p);
	if (*k < k2)
		*k = k2;
}

void Lighting::update(const box3ll &bb, box3ll *rbb)
{
	int k;
	box3ll bb2;
	v3ll p;

	for (auto p : bb) {
		s1.push_back(p);
		s2.push_back(p);
	}

	while (!s1.empty()) {
		p = s1.back();
		s1.pop_back();
		k = w->get_light(p);
		if (k > 0) {
			w->set_light(p, 0);
			find_boundary(v3ll(p.x, p.y - 1, p.z), k);
			if (k > 1) {
				find_boundary(v3ll(p.x, p.y + 1, p.z), k - 1);
				find_boundary(v3ll(p.x - 1, p.y, p.z), k - 1);
				find_boundary(v3ll(p.x + 1, p.y, p.z), k - 1);
				find_boundary(v3ll(p.x, p.y, p.z - 1), k - 1);
				find_boundary(v3ll(p.x, p.y, p.z + 1), k - 1);
			}
		}
	}

	if (bb.y1 == World::H) {
		box2ll top(bb.x0, bb.z0, bb.x1, bb.z1);
		for (auto &q : top)
			w->set_light(v3ll(q.x, World::H - 1, q.y), 15);
	}

	if (rbb != NULL)
		*rbb = bb;
	bb2 = bb;
	while (!s2.empty()) {
		p = s2.back();
		s2.pop_back();
		if (p.x < bb2.x0)
			bb2.x0 = p.x;
		else if (p.x > bb2.x1)
			bb2.x1 = p.x;
		if (p.y < bb2.y0)
			bb2.y0 = p.y;
		else if (p.y > bb2.y1)
			bb2.y1 = p.y;
		if (p.z < bb2.z0)
			bb2.z0 = p.z;
		else if (p.z > bb2.z1)
			bb2.z1 = p.z;
		k = w->get_light(p);
		lit_up(v3ll(p.x, p.y - 1, p.z), k, 0, 1);
		lit_up(v3ll(p.x, p.y + 1, p.z), k, 1, 0);
		lit_up(v3ll(p.x - 1, p.y, p.z), k, 1, 1);
		lit_up(v3ll(p.x + 1, p.y, p.z), k, 1, 1);
		lit_up(v3ll(p.x, p.y, p.z - 1), k, 1, 1);
		lit_up(v3ll(p.x, p.y, p.z + 1), k, 1, 1);
	}

	strange_blocks(bb2);
}

void Lighting::strange_blocks(const box3ll &bb)
{
	for (auto p : bb) {
		int s = w->get_shape(p);
		if (s == SHAPE_SLAB_DN) {
			int k = w->get_light(p);
			copy_value(w, v3ll(p.x, p.y + 1, p.z), &k);
			copy_value(w, v3ll(p.x - 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x + 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x, p.y, p.z - 1), &k);
			copy_value(w, v3ll(p.x, p.y, p.z + 1), &k);
			w->set_light(p, k);
		} else if (s == SHAPE_SLAB_UP) {
			int k = w->get_light(p);
			copy_value(w, v3ll(p.x, p.y - 1, p.z), &k);
			copy_value(w, v3ll(p.x - 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x + 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x, p.y, p.z - 1), &k);
			copy_value(w, v3ll(p.x, p.y, p.z + 1), &k);
			w->set_light(p, k);
		} else if (s == SHAPE_SLAB_LF) {
			int k = w->get_light(p);
			copy_value(w, v3ll(p.x, p.y - 1, p.z), &k);
			copy_value(w, v3ll(p.x, p.y + 1, p.z), &k);
			copy_value(w, v3ll(p.x + 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x, p.y, p.z - 1), &k);
			copy_value(w, v3ll(p.x, p.y, p.z + 1), &k);
			w->set_light(p, k);
		} else if (s == SHAPE_SLAB_RT) {
			int k = w->get_light(p);
			copy_value(w, v3ll(p.x, p.y - 1, p.z), &k);
			copy_value(w, v3ll(p.x, p.y + 1, p.z), &k);
			copy_value(w, v3ll(p.x - 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x, p.y, p.z - 1), &k);
			copy_value(w, v3ll(p.x, p.y, p.z + 1), &k);
			w->set_light(p, k);
		} else if (s == SHAPE_SLAB_BK) {
			int k = w->get_light(p);
			copy_value(w, v3ll(p.x, p.y - 1, p.z), &k);
			copy_value(w, v3ll(p.x, p.y + 1, p.z), &k);
			copy_value(w, v3ll(p.x - 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x + 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x, p.y, p.z + 1), &k);
			w->set_light(p, k);
		} else if (s == SHAPE_SLAB_FT) {
			int k = w->get_light(p);
			copy_value(w, v3ll(p.x, p.y - 1, p.z), &k);
			copy_value(w, v3ll(p.x, p.y + 1, p.z), &k);
			copy_value(w, v3ll(p.x - 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x + 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x, p.y, p.z - 1), &k);
			w->set_light(p, k);
		} else if (s == SHAPE_STAIRS_DF) {
			int k = w->get_light(p);
			copy_value(w, v3ll(p.x, p.y + 1, p.z), &k);
			copy_value(w, v3ll(p.x - 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x + 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x, p.y, p.z - 1), &k);
			w->set_light(p, k);
		} else if (s == SHAPE_STAIRS_DB) {
			int k = w->get_light(p);
			copy_value(w, v3ll(p.x, p.y + 1, p.z), &k);
			copy_value(w, v3ll(p.x - 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x + 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x, p.y, p.z + 1), &k);
			w->set_light(p, k);
		} else if (s == SHAPE_STAIRS_DL) {
			int k = w->get_light(p);
			copy_value(w, v3ll(p.x, p.y + 1, p.z), &k);
			copy_value(w, v3ll(p.x + 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x, p.y, p.z - 1), &k);
			copy_value(w, v3ll(p.x, p.y, p.z + 1), &k);
			w->set_light(p, k);
		} else if (s == SHAPE_STAIRS_DR) {
			int k = w->get_light(p);
			copy_value(w, v3ll(p.x, p.y + 1, p.z), &k);
			copy_value(w, v3ll(p.x - 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x, p.y, p.z - 1), &k);
			copy_value(w, v3ll(p.x, p.y, p.z + 1), &k);
			w->set_light(p, k);
		} else if (s == SHAPE_STAIRS_UF) {
			int k = w->get_light(p);
			copy_value(w, v3ll(p.x, p.y - 1, p.z), &k);
			copy_value(w, v3ll(p.x - 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x + 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x, p.y, p.z - 1), &k);
			w->set_light(p, k);
		} else if (s == SHAPE_STAIRS_UB) {
			int k = w->get_light(p);
			copy_value(w, v3ll(p.x, p.y - 1, p.z), &k);
			copy_value(w, v3ll(p.x - 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x + 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x, p.y, p.z + 1), &k);
			w->set_light(p, k);
		} else if (s == SHAPE_STAIRS_UL) {
			int k = w->get_light(p);
			copy_value(w, v3ll(p.x, p.y - 1, p.z), &k);
			copy_value(w, v3ll(p.x + 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x, p.y, p.z - 1), &k);
			copy_value(w, v3ll(p.x, p.y, p.z + 1), &k);
			w->set_light(p, k);
		} else if (s == SHAPE_STAIRS_UR) {
			int k = w->get_light(p);
			copy_value(w, v3ll(p.x, p.y - 1, p.z), &k);
			copy_value(w, v3ll(p.x, p.y + 1, p.z), &k);
			copy_value(w, v3ll(p.x - 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x, p.y, p.z - 1), &k);
			copy_value(w, v3ll(p.x, p.y, p.z + 1), &k);
			w->set_light(p, k);
		}
	}
}

