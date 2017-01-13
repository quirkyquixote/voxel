
#include "lighting.h"

#include "stack.h"
#include "box2.h"
#include "v2.h"
#include "types.h"

static const int opaque_shape[] = {
	0,
	1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1,
	0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

struct lighting {
	struct world *w;
	struct stack *s1;
	struct stack *s2;
};

static inline void find_boundary(struct lighting *l, struct v3ll p, int k)
{
	int k2;
	if (p.y < 0 || p.y >= WORLD_H)
		return;
	if (opaque_shape[world_get_shape(l->w, p)])
		return;
	k2 = world_get_light(l->w, p);
	if (k2 == k) {
		stack_push(l->s1, &p);
	} else if (k2 > k) {
		stack_push(l->s2, &p);
	}
}

static inline void lit_up(struct lighting *l, struct v3ll p, int k, int f1, int f2)
{
	int k2;
	if (p.y < 0 || p.y >= WORLD_H) {
		return;
	}
	if (opaque_shape[world_get_shape(l->w, p)]) {
		return;
	}
	k2 = world_get_light(l->w, p);
	if (k2 < k - f1) {
		world_set_light(l->w, p, k - f1);
		stack_push(l->s2, &p);
	} else if (k < k2 - f2) {
		stack_push(l->s2, &p);
	}
}

void copy_value(struct world *w, struct v3ll p, int *k)
{
	int k2;
	if (opaque_shape[world_get_shape(w, p)])
		return;
	k2 = world_get_light(w, p);
	if (*k < k2)
		*k = k2;
}

void update_lighting(struct world *w, struct box3ll bb, struct box3ll *rbb)
{
	struct lighting *l;
	struct v3ll p;
	int k;

	l = calloc(1, sizeof(*l));
	l->w = w;
	l->s1 = stack(sizeof(struct v3ll));
	l->s2 = stack(sizeof(struct v3ll));

	box3_foreach(p, bb) {
		stack_push(l->s1, &p);
		stack_push(l->s2, &p);
	}

	while (stack_size(l->s1)) {
		stack_pop(l->s1, &p);
		k = world_get_light(w, p);
		world_set_light(w, p, 0);
		find_boundary(l, v3ll(p.x, p.y - 1, p.z), k);
		find_boundary(l, v3ll(p.x, p.y + 1, p.z), k - 1);
		find_boundary(l, v3ll(p.x - 1, p.y, p.z), k - 1);
		find_boundary(l, v3ll(p.x + 1, p.y, p.z), k - 1);
		find_boundary(l, v3ll(p.x, p.y, p.z - 1), k - 1);
		find_boundary(l, v3ll(p.x, p.y, p.z + 1), k - 1);
	}

	if (bb.y1 == WORLD_H) {
		struct box2ll top = box2ll(bb.x0, bb.z0, bb.x1, bb.z1);
		struct v2ll q;
		box2_foreach(q, top)
			world_set_light(w, v3ll(q.x, WORLD_H - 1, q.y), 15);
	}

	*rbb = bb;
	while (stack_size(l->s2)) {
		stack_pop(l->s2, &p);
		if (p.x < rbb->x0)
			rbb->x0 = p.x;
		else if (p.x >= rbb->x1)
			rbb->x1 = p.x + 1;
		if (p.y < rbb->y0)
			rbb->y0 = p.y;
		else if (p.y >= rbb->y1)
			rbb->y1 = p.y + 1;
		if (p.z < rbb->z0)
			rbb->z0 = p.z;
		else if (p.z >= rbb->z1)
			rbb->z1 = p.z + 1;
		k = world_get_light(w, p);
		lit_up(l, v3ll(p.x, p.y - 1, p.z), k, 0, 1);
		lit_up(l, v3ll(p.x, p.y + 1, p.z), k, 1, 0);
		lit_up(l, v3ll(p.x - 1, p.y, p.z), k, 1, 1);
		lit_up(l, v3ll(p.x + 1, p.y, p.z), k, 1, 1);
		lit_up(l, v3ll(p.x, p.y, p.z - 1), k, 1, 1);
		lit_up(l, v3ll(p.x, p.y, p.z + 1), k, 1, 1);
	}
	stack_destroy(l->s1);
	stack_destroy(l->s2);
	free(l);

	box3_foreach(p, bb) {
		int s = world_get_shape(w, p);
		if (s == SHAPE_SLAB_DN) {
			k = world_get_light(w, p);
			copy_value(w, v3ll(p.x, p.y + 1, p.z), &k);
			copy_value(w, v3ll(p.x - 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x + 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x, p.y, p.z - 1), &k);
			copy_value(w, v3ll(p.x, p.y, p.z + 1), &k);
			world_set_light(w, p, k);
		} else if (s == SHAPE_SLAB_UP) {
			k = world_get_light(w, p);
			copy_value(w, v3ll(p.x, p.y - 1, p.z), &k);
			copy_value(w, v3ll(p.x - 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x + 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x, p.y, p.z - 1), &k);
			copy_value(w, v3ll(p.x, p.y, p.z + 1), &k);
			world_set_light(w, p, k);
		} else if (s == SHAPE_SLAB_LF) {
			k = world_get_light(w, p);
			copy_value(w, v3ll(p.x, p.y - 1, p.z), &k);
			copy_value(w, v3ll(p.x, p.y + 1, p.z), &k);
			copy_value(w, v3ll(p.x + 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x, p.y, p.z - 1), &k);
			copy_value(w, v3ll(p.x, p.y, p.z + 1), &k);
			world_set_light(w, p, k);
		} else if (s == SHAPE_SLAB_RT) {
			k = world_get_light(w, p);
			copy_value(w, v3ll(p.x, p.y - 1, p.z), &k);
			copy_value(w, v3ll(p.x, p.y + 1, p.z), &k);
			copy_value(w, v3ll(p.x - 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x, p.y, p.z - 1), &k);
			copy_value(w, v3ll(p.x, p.y, p.z + 1), &k);
			world_set_light(w, p, k);
		} else if (s == SHAPE_SLAB_BK) {
			k = world_get_light(w, p);
			copy_value(w, v3ll(p.x, p.y - 1, p.z), &k);
			copy_value(w, v3ll(p.x, p.y + 1, p.z), &k);
			copy_value(w, v3ll(p.x - 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x + 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x, p.y, p.z + 1), &k);
			world_set_light(w, p, k);
		} else if (s == SHAPE_SLAB_FT) {
			k = world_get_light(w, p);
			copy_value(w, v3ll(p.x, p.y - 1, p.z), &k);
			copy_value(w, v3ll(p.x, p.y + 1, p.z), &k);
			copy_value(w, v3ll(p.x - 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x + 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x, p.y, p.z - 1), &k);
			world_set_light(w, p, k);
		} else if (s == SHAPE_STAIRS_DF) {
			k = world_get_light(w, p);
			copy_value(w, v3ll(p.x, p.y + 1, p.z), &k);
			copy_value(w, v3ll(p.x - 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x + 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x, p.y, p.z - 1), &k);
			world_set_light(w, p, k);
		} else if (s == SHAPE_STAIRS_DB) {
			k = world_get_light(w, p);
			copy_value(w, v3ll(p.x, p.y + 1, p.z), &k);
			copy_value(w, v3ll(p.x - 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x + 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x, p.y, p.z + 1), &k);
			world_set_light(w, p, k);
		} else if (s == SHAPE_STAIRS_DL) {
			k = world_get_light(w, p);
			copy_value(w, v3ll(p.x, p.y + 1, p.z), &k);
			copy_value(w, v3ll(p.x + 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x, p.y, p.z - 1), &k);
			copy_value(w, v3ll(p.x, p.y, p.z + 1), &k);
			world_set_light(w, p, k);
		} else if (s == SHAPE_STAIRS_DR) {
			k = world_get_light(w, p);
			copy_value(w, v3ll(p.x, p.y + 1, p.z), &k);
			copy_value(w, v3ll(p.x - 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x, p.y, p.z - 1), &k);
			copy_value(w, v3ll(p.x, p.y, p.z + 1), &k);
			world_set_light(w, p, k);
		} else if (s == SHAPE_STAIRS_UF) {
			k = world_get_light(w, p);
			copy_value(w, v3ll(p.x, p.y - 1, p.z), &k);
			copy_value(w, v3ll(p.x - 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x + 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x, p.y, p.z - 1), &k);
			world_set_light(w, p, k);
		} else if (s == SHAPE_STAIRS_UB) {
			k = world_get_light(w, p);
			copy_value(w, v3ll(p.x, p.y - 1, p.z), &k);
			copy_value(w, v3ll(p.x - 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x + 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x, p.y, p.z + 1), &k);
			world_set_light(w, p, k);
		} else if (s == SHAPE_STAIRS_UL) {
			k = world_get_light(w, p);
			copy_value(w, v3ll(p.x, p.y - 1, p.z), &k);
			copy_value(w, v3ll(p.x + 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x, p.y, p.z - 1), &k);
			copy_value(w, v3ll(p.x, p.y, p.z + 1), &k);
			world_set_light(w, p, k);
		} else if (s == SHAPE_STAIRS_UR) {
			k = world_get_light(w, p);
			copy_value(w, v3ll(p.x, p.y - 1, p.z), &k);
			copy_value(w, v3ll(p.x, p.y + 1, p.z), &k);
			copy_value(w, v3ll(p.x - 1, p.y, p.z), &k);
			copy_value(w, v3ll(p.x, p.y, p.z - 1), &k);
			copy_value(w, v3ll(p.x, p.y, p.z + 1), &k);
			world_set_light(w, p, k);
		}
	}
}

