
#include "flowsim.h"

#include <stdio.h>

static int object_id = 0;

static inline void check_boundary(struct fs_layer *l, struct stack *s, struct v3ll p)
{
	struct fs_layer *l2;
	if (WORLD_AT(l->v->w, shape, p.x, p.y, p.z) != SHAPE_NONE)
		return;
	if (WORLD_AT(l->v->w, data, p.x, p.y, p.z) == l)
		return;
	if (WORLD_AT(l->v->w, shape, p.x, p.y - 1, p.z) == SHAPE_NONE) {
		l2 = WORLD_AT(l->v->w, data, p.x, p.y - 1, p.z);
		if (l2 == NULL || l2->top - l2->y < 1) {
			stack_push(l->falls, &p);
			return;
		}
		l2->is_top = 0;
	}
	if (WORLD_AT(l->v->w, data, p.x, p.y, p.z) != NULL) {
		/* merge layers */
		return;
	}
	WORLD_AT(l->v->w, data, p.x, p.y, p.z) = l;
	stack_push(s, &p);
	stack_push(l->cells, &p);
}

static inline void calculate_boundary(struct fs_layer *l, struct v3ll p)
{
	struct stack *s = stack(sizeof(struct v3ll));
	check_boundary(l, s, p);
	while (stack_size(s)) {
		stack_pop(s, &p);
		check_boundary(l, s, v3ll(p.x - 1, p.y, p.z));
		check_boundary(l, s, v3ll(p.x + 1, p.y, p.z));
		check_boundary(l, s, v3ll(p.x, p.y, p.z - 1));
		check_boundary(l, s, v3ll(p.x, p.y, p.z + 1));
	}
}

struct fs_layer *fs_layer(struct fs_volume *v, struct v3ll p)
{
	struct fs_layer *l = calloc(1, sizeof(*l));
	l->v = v;
	l->cells = stack(sizeof(struct v3ll));
	l->falls = stack(sizeof(struct v3ll));
	calculate_boundary(l, p);
	l->y = p.y;
	l->top = p.y;
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
				ll = fs_layer(v, p);
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
	l = fs_layer(v, p);
	l->is_top = 1;
	stack_push(v->top_layers, &l);
	l->top += k / stack_size(l->cells);
	return 1;
}

