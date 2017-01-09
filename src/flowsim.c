
#include "flowsim.h"

#include <stdio.h>

struct cell {
	struct v3ll p;
	int v;
};

static const int vol_from_shape[] = {
	0,
	-1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1,
	-1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1,
	1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
	-1, -1, -1
};

static const int shape_from_vol[] = {
	SHAPE_NONE,
	SHAPE_FLUID1,
	SHAPE_FLUID2,
	SHAPE_FLUID3,
	SHAPE_FLUID4,
	SHAPE_FLUID5,
	SHAPE_FLUID6,
	SHAPE_FLUID7,
	SHAPE_FLUID8,
	SHAPE_FLUID9,
	SHAPE_FLUID10,
	SHAPE_FLUID11,
	SHAPE_FLUID12,
	SHAPE_FLUID13,
	SHAPE_FLUID14,
	SHAPE_FLUID15,
	SHAPE_FLUID16,
};

struct flowsim *flowsim(struct world *w)
{
	struct flowsim *f = calloc(1, sizeof(*f));
	f->w = w;
	f->heads = stack(sizeof(struct v3ll));
	f->heads2 = stack(sizeof(struct v3ll));
	f->s = stack(sizeof(struct v3ll));
	f->gs = stack(sizeof(struct cell));
	return f;
}

void flowsim_destroy(struct flowsim *f)
{
	free(f);
}


void find_boundary(struct flowsim *f, struct v3ll p, int g)
{
	int *m = &f->gmask[p.x][p.y][p.z];
	if (*m == g)
		return;
	if (*m != 0)
		return; /* this should not happen! */
	int s = WORLD_AT(f->w, shape, f->w->x + p.x, p.y, f->w->z + p.z);
	int *v = &f->vol[p.x][p.y][p.z];
	*v = vol_from_shape[s];
	printf("%lld,%lld,%lld", p.x, p.y, p.z);
	if (*v < 0) {
		printf(" (solid)\n");
		return;
	}
	*m = g;
	if (stack_size(f->gs) == 0) {
		printf(" (root)\n");
		stack_push(f->s, &p);
	} else if (*v > 0) {
		printf(" (inner)\n");
		stack_push(f->s, &p);
	} else {
		printf(" (boundary)\n");
	}
	struct cell c = { p, *v };
	stack_push(f->gs, &c);
}

static inline void move_flowsim(struct flowsim *f, struct cell c1, struct cell c2)
{
	world_set(f->w, v3_add(c1.p, v3ll(f->w->x, 0, f->w->z)),
			shape_from_vol[++c1.v], 255, NULL);
	world_set(f->w, v3_add(c2.p, v3ll(f->w->x, 0, f->w->z)),
			shape_from_vol[--c2.v], 255, NULL);
}

int cells_by_volume(const void *p1, const void *p2)
{
	const struct cell *c1, *c2;
	c1 = p1;
	c2 = p2;
	return (c1->v + 16 * c1->p.y) - (c2->v + 16 * c2->p.y);
}

int flowsim_step(struct flowsim *f)
{
	struct v3ll p, q;
	int i, g, s;

	if (stack_size(f->heads) == 0)
		return 0;

	g = 0;
	memset(f->gmask, 0, sizeof(f->gmask));
	while (stack_size(f->heads)) {
		stack_pop(f->heads, &p);
		if (f->gmask[p.x][p.y][p.z] != 0)
			continue;
		printf("BUILD GROUP\n");
		++g;
		find_boundary(f, p, g);
		while (stack_size(f->s)) {
			stack_pop(f->s, &p);
			find_boundary(f, v3ll(p.x - 1, p.y, p.z), g);
			find_boundary(f, v3ll(p.x + 1, p.y, p.z), g);
			find_boundary(f, v3ll(p.x, p.y - 1, p.z), g);
			find_boundary(f, v3ll(p.x, p.y + 1, p.z), g);
			find_boundary(f, v3ll(p.x, p.y, p.z - 1), g);
			find_boundary(f, v3ll(p.x, p.y, p.z + 1), g);
		}
		if (f->gs->size == 0) {
			printf("EMPTY!\n");
			continue;
		}
		int group_changed = 0;
		printf("STEP GROUP\n");
		struct cell *mid = (void *)f->gs->data;
		qsort(mid, f->gs->size, sizeof(struct cell), cells_by_volume);
		int j0 = 0;
		int j1 = f->gs->size - 1;
		for (;;) {
			while (mid[j0].v == 16) {
				++j0;
				if (mid[j0].p.y == mid[j1].p.y && abs(mid[j0].v - mid[j1].v) <= 1)
					goto cleanup;
			}
			while (mid[j1].v == 0) {
				--j1;
				if (mid[j0].p.y == mid[j1].p.y && abs(mid[j0].v - mid[j1].v) <= 1)
					goto cleanup;
			}
			if (group_changed == 0) {
				printf("NEW HEAD: %lld,%lld,%lld\n",
						mid[j0].p.x, mid[j0].p.y, mid[j0].p.z);
				stack_push(f->heads2, &mid[j0].p);
				group_changed = 1;
			}
			printf("%d << %d\n", j0, j1);
			move_flowsim(f, mid[j0], mid[j1]);
			++j0;
			--j1;
			if (j0 > j1 || (mid[j0].p.y == mid[j1].p.y && abs(mid[j0].v - mid[j1].v) <= 1))
				goto cleanup;
		}
cleanup:
		f->gs->size = 0;
	}
	void *tmp = f->heads;
	f->heads = f->heads2;
	f->heads2 = tmp;
}
