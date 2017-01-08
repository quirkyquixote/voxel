
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

static inline void try_cell(struct world *w, struct v3ll *pv, size_t *pc, struct v3ll p, int l)
{
	int s;
	s = WORLD_AT(w, shape, p.x, p.y, p.z);
	if (opaque_shape[s]) {
		WORLD_AT(w, light, p.x, p.y, p.z) = 0;
		return;
	}
	if (WORLD_AT(w, light, p.x, p.y, p.z) >= l)
		return;
	WORLD_AT(w, light, p.x, p.y, p.z) = l;
	pv[(*pc)++] = p;
}

void update_lighting(struct world *w, struct aab3ll bb)
{
	struct v3ll *pv;
	size_t pc;
	struct v3ll p;
	int l;

	aab3_foreach(p, bb) {
		WORLD_AT(w, light, p.x, p.y, p.z) = 0;
	}

	pv = calloc((bb.x1 - bb.x0) * (bb.y1 - bb.y0) * (bb.z1 - bb.z0), sizeof(*pv));
	p.y = bb.y1 - 1;
	pc = 0;
	for (p.x = bb.x0; p.x < bb.x1; ++p.x) {
		for (p.z = bb.z0; p.z < bb.z1; ++p.z) {
			WORLD_AT(w, light, p.x, p.y, p.z) = 15;
			pv[pc++] = p;
		}
	}

	while (pc) {
		p = pv[--pc];
		l = WORLD_AT(w, light, p.x, p.y, p.z);
		if (l > 1) {
			if (p.x > bb.x0)
				try_cell(w, pv, &pc, v3ll(p.x - 1, p.y, p.z), l - 1);
			if (p.x < bb.x1 - 1)
				try_cell(w, pv, &pc, v3ll(p.x + 1, p.y, p.z), l - 1);
			if (p.z > bb.z0)
				try_cell(w, pv, &pc, v3ll(p.x, p.y, p.z - 1), l - 1);
			if (p.z < bb.z1 - 1)
				try_cell(w, pv, &pc, v3ll(p.x, p.y, p.z + 1), l - 1);
			if (p.y < bb.y1 - 1)
				try_cell(w, pv, &pc, v3ll(p.x, p.y + 1, p.z), l - 1);
		}
		if (p.y > bb.y0) {
			try_cell(w, pv, &pc, v3ll(p.x, p.y - 1, p.z), l);
		}
	}
	free(pv);

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

