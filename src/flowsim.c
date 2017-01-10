
#include "flowsim.h"

#include <stdio.h> 


static inline void check_boundary(struct fs_layer *l, struct stack *s, struct v3ll p)
{
	struct fs_layer *l2;
	if (WORLD_AT(l->v->w, shape, p.x, p.y, p.z) != SHAPE_NONE)
		return;
	if (WORLD_AT(l->v->w, data, p.x, p.y, p.z) == l)
		return;
	if (WORLD_AT(l->v->w, shape, p.x, p.y - 1, p.z) == SHAPE_NONE) {
		l2 = WORLD_AT(l->v->w, data, p.x, p.y - 1, p.z);
		if (l2 == NULL || l2->top - l2->bottom < 1) {
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
	l->is_top = 1;
	l->bottom = p.y;
	l->top = p.y;
	list_prepend(&v->top_layers, &l->top_layers);
	printf("%s %p\n", __func__, l);
	return l;
}

void fs_layer_destroy(struct fs_layer *l)
{
	printf("%s %p\n", __func__, l);
	stack_destroy(l->cells);
	stack_destroy(l->falls);
	free(l);
}

struct fs_volume *fs_volume(struct world *w)
{
	struct fs_volume *v = calloc(1, sizeof(*v));
	v->w = w;
	list_init(&v->top_layers);
	printf("%s %p\n", __func__, v);
	return v;
}

void fs_volume_destroy(struct fs_volume *v)
{
	printf("%s %p\n", __func__, v);
	free(v);
}

/* unlink this top layers search new ones over it */
static void push_layer(struct fs_volume *v, struct fs_layer *l)
{
	struct v3ll p;
	struct fs_layer *ll;
	list_unlink(&l->top_layers);
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
			}
		}
	}
}

/* unlink and destroy this top layer and search for new ones below it */
static void pop_layer(struct fs_volume *v, struct fs_layer *l)
{
	struct v3ll p;
	struct fs_layer *ll;
	list_unlink(&l->top_layers);
	stack_foreach(p, l->cells) {
		WORLD_AT(v->w, data, p.x, p.y, p.z) = NULL;
		--p.y;
		if (WORLD_AT(v->w, shape, p.x, p.y, p.z) == SHAPE_NONE) {
			ll = WORLD_AT(v->w, data, p.x, p.y, p.z);
			if (ll != NULL && !ll->is_top) {
				ll->is_top = 1;
				list_prepend(&v->top_layers, &l->top_layers);
			}
		}
	}
	fs_layer_destroy(l);
}

void fs_volume_step(struct fs_volume *v)
{
	struct fs_layer *l, *next;
	float area, vol, height;
	area = 0;
	vol = v->roaming;
	v->roaming = 0;
	list_foreach(l, &v->top_layers, top_layers) {
		area += stack_size(l->cells);
		vol += stack_size(l->cells) * l->top;
	}
	height = vol / area;
	list_foreach_safe(l, next, &v->top_layers, top_layers) {
		l->top = height;
		if (l->top > l->bottom + 1) {
			printf("%g > %g + 1: push layer\n", l->top, l->bottom);
			v->roaming += l->top - l->bottom - 1;
			push_layer(v, l);
		} else if (l->top <= l->bottom) {
			printf("%g <= %g: pop layer\n", l->top, l->bottom);
			pop_layer(v, l);
		}
	}
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
		list_foreach(l, &v->top_layers, top_layers) {
			float a = stack_size(l->falls) * .1;
			float b = stack_size(l->cells) * (l->top - l->bottom);
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
	if (WORLD_AT(f->w, shape, p.x, p.y, p.z) != SHAPE_NONE)
		return 0;
	do {
		l = WORLD_AT(f->w, data, p.x, p.y, p.z);
		if (l != NULL) {
			if (!l->is_top) {
				return 0;
			}
			l->top += k / stack_size(l->cells);
			return 1;
		}
		--p.y;
	} while (WORLD_AT(f->w, shape, p.x, p.y, p.z) == SHAPE_NONE);
	++p.y;
	v = fs_volume(f->w);
	list_append(&f->volumes, &v->volumes);
	l = fs_layer(v, p);
	l->top += k / stack_size(l->cells);
	return 1;
}

