
#include "lighting.h"

#include <stdlib.h>

static const int opaque_shape[] = {
	0,
	1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1,
	0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static inline int max(int a, int b)
{
	return a > b ? a : b;
}

struct lighting {
	struct world *w;
	struct v3ll pv1[WORLD_VOLUME];
	size_t pc1;
	struct v3ll pv2[WORLD_VOLUME];
	size_t pc2;
};

static inline void find_boundary(struct lighting *l, struct v3ll p, int k)
{
	int k2;
	if (opaque_shape[WORLD_AT(l->w, shape, p.x, p.y, p.z)])
		return;
	k2 = WORLD_AT(l->w, light, p.x, p.y, p.z);
	if (k2 == k - 1) {
		l->pv1[l->pc1++] = p;
	} else if (k2 > k) {
		l->pv2[l->pc2++] = p;
	}
}

static inline void lit_up(struct lighting *l, struct v3ll p, int *k)
{
	int k2;
	if (opaque_shape[WORLD_AT(l->w, shape, p.x, p.y, p.z)])
		return;
	k2 = WORLD_AT(l->w, light, p.x, p.y, p.z);
	if (k2 < *k - 1) {
		WORLD_AT(l->w, light, p.x, p.y, p.z) = *k - 1;
		l->pv2[l->pc2++] = p;
	} else if (*k < k2 - 1) {
		*k = k2 - 1;
	}
}

void update_lighting(struct world *w, struct aab3ll bb)
{
	struct lighting *l;
	struct aab3ll top;
	struct v3ll p;
	int k;

	l = calloc(1, sizeof(*l));
	l->w = w;

	aab3_foreach(p, bb)
		l->pv1[l->pc1++] = p;

	while (l->pc1) {
		p = l->pv1[--l->pc1];
		k = WORLD_AT(w, light, p.x, p.y, p.z);
		while (p.y > 0) {
			find_boundary(l, v3ll(p.x - 1, p.y, p.z), k);
			find_boundary(l, v3ll(p.x + 1, p.y, p.z), k);
			find_boundary(l, v3ll(p.x, p.y, p.z - 1), k);
			find_boundary(l, v3ll(p.x, p.y, p.z + 1), k);
			if (WORLD_AT(w, light, p.x, p.y, p.z) != k) {
				l->pv2[l->pc2++] = p;
				break;
			}
			WORLD_AT(w, light, p.x, p.y, p.z) = 0;
			--p.y;
		}
	}

	if (bb.y1 == WORLD_H) {
		top = aab3ll(bb.x0, bb.y1 - 1, bb.z0, bb.x1, bb.y1, bb.z1);
		aab3_foreach(p, top) {
			WORLD_AT(w, light, p.x, p.y, p.z) = 15;
			l->pv2[l->pc2++] = p;
		}
	} else {
		top = aab3ll(bb.x0, bb.y1, bb.z0, bb.x1, bb.y1 + 1, bb.z1);
		aab3_foreach(p, top) {
			l->pv2[l->pc2++] = p;
		}
	}

	while (l->pc2) {
		p = l->pv2[--l->pc2];
		k = WORLD_AT(w, light, p.x, p.y, p.z);
		while (p.y > 0) {
			if (opaque_shape[WORLD_AT(w, shape, p.x, p.y, p.z)])
				k = 0;
			lit_up(l, v3ll(p.x - 1, p.y, p.z), &k);
			lit_up(l, v3ll(p.x + 1, p.y, p.z), &k);
			lit_up(l, v3ll(p.x, p.y, p.z - 1), &k);
			lit_up(l, v3ll(p.x, p.y, p.z + 1), &k);
			WORLD_AT(w, light, p.x, p.y, p.z) = k;
			--p.y;
		}
	}
	free(l);

	aab3_foreach(p, bb) {
		int s = WORLD_AT(w, shape, p.x, p.y, p.z);
		if (s == SHAPE_SLAB_DN)
			WORLD_AT(w, light, p.x, p.y, p.z) =
				max(WORLD_AT(w, light, p.x - 1, p.y, p.z),
						max(WORLD_AT(w, light, p.x + 1, p.y, p.z),
							max(WORLD_AT(w, light, p.x, p.y, p.z - 1),
								max(WORLD_AT(w, light, p.x, p.y, p.z + 1),
									WORLD_AT(w, light, p.x, p.y + 1, p.z)))));
		else if (s == SHAPE_SLAB_UP)
			WORLD_AT(w, light, p.x, p.y, p.z) =
				max(WORLD_AT(w, light, p.x - 1, p.y, p.z),
						max(WORLD_AT(w, light, p.x + 1, p.y, p.z),
							max(WORLD_AT(w, light, p.x, p.y, p.z - 1),
								max(WORLD_AT(w, light, p.x, p.y, p.z + 1),
									WORLD_AT(w, light, p.x, p.y - 1, p.z)))));
		else if (s == SHAPE_SLAB_LF)
			WORLD_AT(w, light, p.x, p.y, p.z) =
				max(WORLD_AT(w, light, p.x + 1, p.y, p.z),
						max(WORLD_AT(w, light, p.x, p.y, p.z - 1),
							max(WORLD_AT(w, light, p.x, p.y, p.z + 1),
								max(WORLD_AT(w, light, p.x, p.y - 1, p.z),
									WORLD_AT(w, light, p.x, p.y + 1, p.z)))));
		else if (s == SHAPE_SLAB_RT)
			WORLD_AT(w, light, p.x, p.y, p.z) =
				max(WORLD_AT(w, light, p.x - 1, p.y, p.z),
						max(WORLD_AT(w, light, p.x, p.y, p.z - 1),
							max(WORLD_AT(w, light, p.x, p.y, p.z + 1),
								max(WORLD_AT(w, light, p.x, p.y - 1, p.z),
									WORLD_AT(w, light, p.x, p.y + 1, p.z)))));
		else if (s == SHAPE_SLAB_BK)
			WORLD_AT(w, light, p.x, p.y, p.z) =
				max(WORLD_AT(w, light, p.x - 1, p.y, p.z),
						max(WORLD_AT(w, light, p.x + 1, p.y, p.z),
							max(WORLD_AT(w, light, p.x, p.y, p.z + 1),
								max(WORLD_AT(w, light, p.x, p.y - 1, p.z),
									WORLD_AT(w, light, p.x, p.y + 1, p.z)))));
		else if (s == SHAPE_SLAB_FT)
			WORLD_AT(w, light, p.x, p.y, p.z) =
				max(WORLD_AT(w, light, p.x - 1, p.y, p.z),
						max(WORLD_AT(w, light, p.x + 1, p.y, p.z),
							max(WORLD_AT(w, light, p.x, p.y, p.z - 1),
								max(WORLD_AT(w, light, p.x, p.y - 1, p.z),
									WORLD_AT(w, light, p.x, p.y + 1, p.z)))));
		else if (s == SHAPE_STAIRS_DF)
			WORLD_AT(w, light, p.x, p.y, p.z) =
				max(WORLD_AT(w, light, p.x - 1, p.y, p.z),
						max(WORLD_AT(w, light, p.x + 1, p.y, p.z),
							max(WORLD_AT(w, light, p.x, p.y, p.z - 1),
								WORLD_AT(w, light, p.x, p.y - 1, p.z))));
		else if (s == SHAPE_STAIRS_DB)
			WORLD_AT(w, light, p.x, p.y, p.z) =
				max(WORLD_AT(w, light, p.x - 1, p.y, p.z),
						max(WORLD_AT(w, light, p.x + 1, p.y, p.z),
							max(WORLD_AT(w, light, p.x, p.y, p.z + 1),
								WORLD_AT(w, light, p.x, p.y - 1, p.z))));
		else if (s == SHAPE_STAIRS_DL)
			WORLD_AT(w, light, p.x, p.y, p.z) =
				max(WORLD_AT(w, light, p.x + 1, p.y, p.z),
						max(WORLD_AT(w, light, p.x, p.y, p.z - 1),
							max(WORLD_AT(w, light, p.x, p.y, p.z + 1),
								WORLD_AT(w, light, p.x, p.y - 1, p.z))));
		else if (s == SHAPE_STAIRS_DR)
			WORLD_AT(w, light, p.x, p.y, p.z) =
				max(WORLD_AT(w, light, p.x - 1, p.y, p.z),
						max(WORLD_AT(w, light, p.x, p.y, p.z - 1),
							max(WORLD_AT(w, light, p.x, p.y, p.z + 1),
								WORLD_AT(w, light, p.x, p.y - 1, p.z))));
		else if (s == SHAPE_STAIRS_UF)
			WORLD_AT(w, light, p.x, p.y, p.z) =
				max(WORLD_AT(w, light, p.x - 1, p.y, p.z),
						max(WORLD_AT(w, light, p.x + 1, p.y, p.z),
							max(WORLD_AT(w, light, p.x, p.y, p.z - 1),
								WORLD_AT(w, light, p.x, p.y + 1, p.z))));
		else if (s == SHAPE_STAIRS_UB)
			WORLD_AT(w, light, p.x, p.y, p.z) =
				max(WORLD_AT(w, light, p.x - 1, p.y, p.z),
						max(WORLD_AT(w, light, p.x + 1, p.y, p.z),
							max(WORLD_AT(w, light, p.x, p.y, p.z + 1),
								WORLD_AT(w, light, p.x, p.y + 1, p.z))));
		else if (s == SHAPE_STAIRS_UL)
			WORLD_AT(w, light, p.x, p.y, p.z) =
				max(WORLD_AT(w, light, p.x + 1, p.y, p.z),
						max(WORLD_AT(w, light, p.x, p.y, p.z - 1),
							max(WORLD_AT(w, light, p.x, p.y, p.z + 1),
								WORLD_AT(w, light, p.x, p.y + 1, p.z))));
		else if (s == SHAPE_STAIRS_UR)
			WORLD_AT(w, light, p.x, p.y, p.z) =
				max(WORLD_AT(w, light, p.x - 1, p.y, p.z),
						max(WORLD_AT(w, light, p.x, p.y, p.z - 1),
							max(WORLD_AT(w, light, p.x, p.y, p.z + 1),
								WORLD_AT(w, light, p.x, p.y + 1, p.z))));
	}
}

