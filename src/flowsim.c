
#include "flowsim.h"

#include <stdio.h>

static int object_id = 0;

static inline void test_fall(struct fs_layer *l, struct v3ll p)
{
	if (WORLD_AT(l->v->w, shape, p.x, p.y, p.z) != SHAPE_NONE)
		return;
	if (WORLD_AT(l->v->w, data, p.x, p.y, p.z) == l)
		return;
	if (WORLD_AT(l->v->w, shape, p.x, p.y - 1, p.z) != SHAPE_NONE)
		return;
	stack_push(l->falls, &p);
}

static void calculate_falls(struct fs_layer *l)
{
//	printf("%s %d\n", __func__, l->id);
	struct v3ll p;
	stack_clear(l->falls);
	stack_foreach(p, l->cells) {
		test_fall(l, v3ll(p.x - 1, p.y, p.z));
		test_fall(l, v3ll(p.x + 1, p.y, p.z));
		test_fall(l, v3ll(p.x, p.y, p.z - 1));
		test_fall(l, v3ll(p.x, p.y, p.z + 1));
	}
//	printf("#falls=%u\n", stack_size(l->falls));
}

static inline void test_cell(struct fs_layer *l, struct v3ll p)
{
	struct fs_layer *l2;
	if (WORLD_AT(l->v->w, shape, p.x, p.y, p.z) != SHAPE_NONE)
		return;
	l2 = WORLD_AT(l->v->w, data, p.x, p.y, p.z);
	if (l2 == l)
		return;
	if (l2 != NULL) {
		/* merge layers */
		return;
	}
	if (WORLD_AT(l->v->w, shape, p.x, p.y - 1, p.z) == SHAPE_NONE) {
		l2 = WORLD_AT(l->v->w, data, p.x, p.y - 1, p.z);
		if (l2->top <= l2->y + 1)
			return;
		/* merge volumes */
	}
	WORLD_AT(l->v->w, data, p.x, p.y, p.z) = l;
	stack_push(l->cells, &p);
}

static void expand_layer(struct fs_layer *l)
{
//	printf("%s %d\n", __func__, l->id);
	struct v3ll p;
	struct stack *tmp = l->cells;
	float vol = (l->top - l->y) * stack_size(l->cells);
	l->cells = stack(sizeof(struct v3ll));
	stack_foreach(p, tmp) {
		stack_push(l->cells, &p);
		test_cell(l, v3ll(p.x - 1, p.y, p.z));
		test_cell(l, v3ll(p.x + 1, p.y, p.z));
		test_cell(l, v3ll(p.x, p.y, p.z - 1));
		test_cell(l, v3ll(p.x, p.y, p.z + 1));
	}
	stack_destroy(tmp);
	l->top = l->y + vol / stack_size(l->cells);
//	printf("#cells=%u\n", stack_size(l->cells));
}

static inline void check_boundary(struct fs_layer *l, struct fs_layer *l2, struct stack *s, struct v3ll p)
{
	if (WORLD_AT(l->v->w, shape, p.x, p.y, p.z) != SHAPE_NONE)
		return;
	if (WORLD_AT(l->v->w, data, p.x, p.y, p.z) == l)
		return;
	if (WORLD_AT(l->v->w, data, p.x, p.y - 1, p.z) != l2)
		return;
	WORLD_AT(l->v->w, data, p.x, p.y, p.z) = l;
	stack_push(s, &p);
	stack_push(l->cells, &p);
}

static void layer_from_layer(struct fs_layer *l, struct fs_layer *l2, struct v3ll p)
{
	printf("%s %d\n", __func__, l->id);
	struct stack *s = stack(sizeof(struct v3ll));
	check_boundary(l, l2, s, p);
	while (stack_size(s)) {
		stack_pop(s, &p);
		check_boundary(l, l2, s, v3ll(p.x - 1, p.y, p.z));
		check_boundary(l, l2, s, v3ll(p.x + 1, p.y, p.z));
		check_boundary(l, l2, s, v3ll(p.x, p.y, p.z - 1));
		check_boundary(l, l2, s, v3ll(p.x, p.y, p.z + 1));
	}
}

struct fs_layer *fs_layer(struct fs_volume *v, int y)
{
	struct fs_layer *l = calloc(1, sizeof(*l));
	l->v = v;
	l->cells = stack(sizeof(struct v3ll));
	l->falls = stack(sizeof(struct v3ll));
	l->y = y;
	l->top = y;
	l->id = object_id++;
	printf("%s %d\n", __func__, l->id);
	return l;
}

void fs_layer_destroy(struct fs_layer *l)
{
	printf("%s %d\n", __func__, l->id);
	stack_destroy(l->cells);
	stack_destroy(l->falls);
	free(l);
}

struct fs_volume *fs_volume(struct world *w)
{
	struct fs_volume *v = calloc(1, sizeof(*v));
	v->w = w;
	v->top_layers = stack(sizeof(struct fs_layer *));
	v->id = object_id++;
	printf("%s %d\n", __func__, v->id);
	return v;
}

void fs_volume_destroy(struct fs_volume *v)
{
	printf("%s %d\n", __func__, v->id);
	stack_destroy(v->top_layers);
	free(v);
}

/* unlink this top layers search new ones over it */
static void push_layer(struct fs_volume *v, struct fs_layer *l)
{
	struct v3ll p;
	struct fs_layer *ll;
	printf("%s %d\n", __func__, v->id);
	stack_foreach(p, l->cells) {
		++p.y;
		if (WORLD_AT(v->w, shape, p.x, p.y, p.z) == SHAPE_NONE) {
			ll = WORLD_AT(v->w, data, p.x, p.y, p.z);
			if (ll != NULL) {
				if (ll->v != l->v) {
					/* merge ll->v and l->v */
				}
			} else {
				ll = fs_layer(v, p.y);
				layer_from_layer(ll, l, p);
				ll->is_top = 1;
				stack_push(v->top_layers, &ll);
			}
		}
	}
}

/* unlink and destroy this top layer and search for new ones below it */
static void pop_layer(struct fs_volume *v, struct fs_layer *l)
{
	struct v3ll p;
	struct fs_layer *ll;
	printf("%s %d\n", __func__, l->id);
	stack_foreach(p, l->cells) {
		WORLD_AT(v->w, data, p.x, p.y, p.z) = NULL;
		--p.y;
		if (WORLD_AT(v->w, shape, p.x, p.y, p.z) == SHAPE_NONE) {
			ll = WORLD_AT(v->w, data, p.x, p.y, p.z);
			if (ll != NULL && !ll->is_top) {
				ll->is_top = 1;
				stack_push(v->top_layers, &ll);
			}
		}
	}
	fs_layer_destroy(l);
}

void fs_volume_step(struct fs_volume *v)
{
	struct fs_layer *l;
	float area, vol, height;
	struct stack *tmp;
	area = 0;
	vol = v->roaming;
	v->roaming = 0;
	stack_foreach(l, v->top_layers) {
		expand_layer(l);
		calculate_falls(l);
		area += stack_size(l->cells);
		vol += stack_size(l->cells) * l->top;
	}
	height = vol / area;
	tmp = v->top_layers;
	v->top_layers = stack(sizeof(struct fs_layer *));
	stack_foreach(l, tmp) {
		l->top = height;
		if (l->top > l->y + 1) {
			v->roaming += l->top - l->y - 1;
			l->top = l->y + 1;
			push_layer(v, l);
		} else if (l->top <= l->y) {
			pop_layer(v, l);
		} else {
			stack_push(v->top_layers, &l);
		}
	}
	stack_destroy(tmp);
}

struct flowsim *flowsim(struct world *w)
{
	struct flowsim *f = calloc(1, sizeof(*f));
	f->w = w;
	list_init(&f->volumes);
	return f;
}

void flowsim_destroy(struct flowsim *f)
{
	free(f);
}

void flowsim_step(struct flowsim *f)
{
	struct fs_volume *v;
	struct fs_layer *l;
	struct v3ll p;
	list_foreach(v, &f->volumes, volumes) {
		stack_foreach(l, v->top_layers) {
			float a = stack_size(l->falls) / 16.;
			float b = stack_size(l->cells) * (l->top - l->y);
			if (a > b)
				a = b;
			b = a / stack_size(l->falls);
			stack_foreach(p, l->falls)
				flowsim_add(f, p, b);
			l->top -= a / stack_size(l->cells);
		}
	}

	list_foreach(v, &f->volumes, volumes)
		fs_volume_step(v);
}

int flowsim_add(struct flowsim *f, struct v3ll p, float k)
{
	struct fs_layer *l;
	struct fs_volume *v;
	if (k == 0)
		return 0;
	while (WORLD_AT(f->w, shape, p.x, p.y, p.z) == SHAPE_NONE) {
		l = WORLD_AT(f->w, data, p.x, p.y, p.z);
		if (l != NULL) {
			if (!l->is_top) {
				l->v->roaming += k;
				return 1;
			}
			l->top += k / stack_size(l->cells);
			return 1;
		}
		--p.y;
	}
	++p.y;
	v = fs_volume(f->w);
	list_append(&f->volumes, &v->volumes);
	l = fs_layer(v, p.y);
	stack_push(l->cells, &p);
	WORLD_AT(f->w, data, p.x, p.y, p.z) = l;
	calculate_falls(l);
	l->is_top = 1;
	stack_push(v->top_layers, &l);
	l->top += k / stack_size(l->cells);
	return 1;
}

