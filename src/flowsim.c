
#include "flowsim.h"

#include <stdio.h>

struct cell {
	struct v3ll p;
	int v0, v1;
	int has_floor;
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
	stack_destroy(f->heads);
	stack_destroy(f->heads2);
	stack_destroy(f->s);
	stack_destroy(f->gs);
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
	int s1 = WORLD_AT(f->w, shape, f->w->x + p.x, p.y - 1, f->w->z + p.z);
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
	struct cell c = { p, *v, *v, vol_from_shape[s1] == -1 };
	stack_push(f->gs, &c);
}

int compare_cells(const void *p1, const void *p2)
{
	const struct cell *c1, *c2;
	c1 = p1;
	c2 = p2;
	return (c1->v1 + 16 * c1->has_floor + 32 * c1->p.y)
		- (c2->v1 + 16 * c1->has_floor + 32 * c2->p.y);
}

int step_group(struct flowsim *f)
{
	int changed = 0;
	struct cell *c0 = (void *)f->gs->data;
	struct cell *c1 = c0 + f->gs->size - 1;
	qsort(c0, f->gs->size, sizeof(struct cell), compare_cells);
	for (; c0 < c1; ++c0, --c1) {
		while (c0->v1 == 16) {
			++c0;
			if (c0 >= c1)
				return changed;
		}
		while (c1->v1 == 0) {
			--c1;
			if (c0 >= c1)
				return changed;
		}
		if (c0->p.y == c1->p.y && c1->v1 <= c0->v1 + 1) {
			printf("end reached\n");
			return changed;
		}
		printf("move: %lld,%lld,%lld << %lld,%lld,%lld\n",
				 c0->p.x, c0->p.y, c0->p.z,
				 c1->p.x, c1->p.y, c1->p.z);
		++c0->v1;
		--c1->v1;
		changed = 1;
	}
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
		printf("STEP GROUP\n");
		int changed = 0;
		for (int k = 0; k < 4; ++k)
			changed |= step_group(f);
		struct cell *c = (void *)f->gs->data;
		struct cell *c1 = c + f->gs->size - 1;
		while (c <= c1) {
			if (c->v0 != c->v1) {
				world_set(f->w, v3_add(c->p, v3ll(f->w->x, 0, f->w->z)),
						shape_from_vol[c->v1], 255, NULL);
			}
			++c;
		}
		if (changed) {
			c = (void *)f->gs->data;
			printf("NEW HEAD: %lld,%lld,%lld\n", c->p.x, c->p.y, c->p.z);
			stack_push(f->heads2, &c->p);
		}
		f->gs->size = 0;
	}
	void *tmp = f->heads;
	f->heads = f->heads2;
	f->heads2 = tmp;
}

