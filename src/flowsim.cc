/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "flowsim.h"

#include <stdio.h>

#include "context.h"

void merge_volumes(struct fs_volume *v1, struct fs_volume *v2)
{
	log_info("%llu %llu", v1->id, v2->id);
	list_merge(&v1->layers, &v2->layers);
	v1->v += v2->v;
	list_unlink(&v2->volumes);
	fs_volume_destroy(v2);
}

void merge_layers(struct fs_layer *l1, struct fs_layer *l2)
{
	log_info("%llu %llu", l1->id, l2->id);
	struct v3ll p;

	list_unlink(&l2->layers);
	array_foreach(p, l2->cells)
		array_push(l1->cells, &p);
	if (l1->v != l2->v)
		merge_volumes(l1->v, l2->v);
	fs_layer_destroy(l2);
}

static inline void check_boundary(struct fs_layer *l, struct fs_layer *l2,
    struct array *s, struct v3ll p)
{
	if (world_get_shape(l->v->w, p) != SHAPE_NONE)
		return;
	if (world_get_data(l->v->w, p) == l)
		return;
	if (world_get_data(l->v->w, v3ll(p.x, p.y - 1, p.z)) != l2)
		return;
	world_set_data(l->v->w, p, l);
	array_push(s, &p);
	array_push(l->cells, &p);
}

static void layer_from_layer(struct fs_layer *l, struct fs_layer *l2,
    struct v3ll p)
{
	log_info("%llu", l->id);
	struct array *s = array(sizeof(struct v3ll));
	check_boundary(l, l2, s, p);
	while (array_size(s)) {
		array_pop(s, &p);
		check_boundary(l, l2, s, v3ll(p.x - 1, p.y, p.z));
		check_boundary(l, l2, s, v3ll(p.x + 1, p.y, p.z));
		check_boundary(l, l2, s, v3ll(p.x, p.y, p.z - 1));
		check_boundary(l, l2, s, v3ll(p.x, p.y, p.z + 1));
	}
}

static inline void test_fall(struct fs_layer *l, struct v3ll p)
{
	if (world_get_shape(l->v->w, p) != SHAPE_NONE)
		return;
	if (world_get_data(l->v->w, p) == l)
		return;
	if (world_get_shape(l->v->w, v3ll(p.x, p.y - 1, p.z)) != SHAPE_NONE)
		return;
	array_push(l->falls, &p);
}

static void calculate_falls(struct fs_layer *l)
{
//      log_info("%d", l->id);
	struct v3ll p;
	array_clear(l->falls);
	array_foreach(p, l->cells) {
		test_fall(l, v3ll(p.x - 1, p.y, p.z));
		test_fall(l, v3ll(p.x + 1, p.y, p.z));
		test_fall(l, v3ll(p.x, p.y, p.z - 1));
		test_fall(l, v3ll(p.x, p.y, p.z + 1));
	}
//      printf("#falls=%u\n", array_size(l->falls));
}

static inline void test_cell(struct fs_layer *l, struct v3ll p)
{
	struct fs_layer *l2;
	if (world_get_shape(l->v->w, p) != SHAPE_NONE)
		return;
	l2 = world_get_data(l->v->w, p);
	if (l2 == l)
		return;
	if (l2 != NULL) {
		merge_layers(l, l2);
		return;
	}
	if (world_get_shape(l->v->w, v3ll(p.x, p.y - 1, p.z)) == SHAPE_NONE)
		return;
	world_set_data(l->v->w, p, l);
	array_push(l->cells, &p);
}

static void expand_layer(struct fs_layer *l)
{
//      log_info("%d", l->id);
	struct v3ll p;
	struct array *tmp = l->cells;
	l->cells = array(sizeof(struct v3ll));
	array_foreach(p, tmp) {
		array_push(l->cells, &p);
		test_cell(l, v3ll(p.x - 1, p.y, p.z));
		test_cell(l, v3ll(p.x + 1, p.y, p.z));
		test_cell(l, v3ll(p.x, p.y, p.z - 1));
		test_cell(l, v3ll(p.x, p.y, p.z + 1));
	}
	array_destroy(tmp);
//      printf("#cells=%u\n", array_size(l->cells));
}

struct fs_layer *fs_layer(struct fs_volume *v, int y)
{
	struct fs_layer *l = calloc(1, sizeof(*l));
	l->v = v;
	l->cells = array(sizeof(struct v3ll));
	l->falls = array(sizeof(struct v3ll));
	l->y = y;
	l->id = next_id();
	log_info("%llu", l->id);
	return l;
}

void fs_layer_destroy(struct fs_layer *l)
{
	log_info("%llu", l->id);
	struct v3ll p;
	array_foreach(p, l->cells)
		world_set_data(l->v->w, p,  NULL);
	array_destroy(l->cells);
	array_destroy(l->falls);
	free(l);
}

struct fs_volume *fs_volume(struct world *w)
{
	struct fs_volume *v = calloc(1, sizeof(*v));
	v->w = w;
	list_init(&v->layers);
	v->id = next_id();
	log_info("%llu", v->id);
	return v;
}

void fs_volume_destroy(struct fs_volume *v)
{
	struct fs_layer *l;
	log_info("%llu", v->id);
	list_foreach(l, &v->layers, layers)
		fs_layer_destroy(l);
	free(v);
}

/* unlink this top layers search new ones over it */
static void push_layer(struct fs_volume *v, struct fs_layer *l)
{
	struct v3ll p;
	struct fs_layer *l2;
	log_info("%llu", v->id);
	array_foreach(p, l->cells) {
		++p.y;
		if (world_get_shape(v->w, p) == SHAPE_NONE) {
			l2 = world_get_data(v->w, p);
			if (l2 != NULL) {
				if (l->v != l2->v)
					merge_volumes(l->v, l2->v);
			} else {
				l2 = fs_layer(v, p.y);
				layer_from_layer(l2, l, p);
				l2->is_top = 1;
				list_prepend(&v->layers, &l2->layers);
				log_info("%d.is_top=1", l2->id);
			}
		}
	}
	l->is_top = 0;
}

/* unlink and destroy this top layer and search for new ones below it */
static void pop_layer(struct fs_volume *v, struct fs_layer *l)
{
	struct v3ll p;
	struct fs_layer *l2;
	log_info("%llu", l->id);
	array_foreach(p, l->cells) {
		--p.y;
		if (world_get_shape(v->w, p) == SHAPE_NONE) {
			l2 = world_get_data(v->w, p);
			if (l2 != NULL && !l2->is_top) {
				l2->is_top = 1;
				list_prepend(&v->layers, &l2->layers);
				log_info("%d.is_top=1", l2->id);
			}
		}
	}
	list_unlink(&l->layers);
	fs_layer_destroy(l);
}

void fs_volume_step(struct fs_volume *v)
{
	struct fs_layer *l;
	float top, bottom;
	top = 0;
	bottom = 0;
	list_foreach(l, &v->layers, layers) {
		assert(l->id < max_id());
		if (l->is_top) {
			top += array_size(l->cells);
		} else {
			bottom += array_size(l->cells);
		}
	}
	v->top = (v->v - bottom) / top;
	list_foreach_safe(l, &v->layers, layers) {
		assert(l->id < max_id());
		if (!l->is_top || v->top > (1. / 64.))
			expand_layer(l);
		calculate_falls(l);
	}
	if (v->top > 1) {
		list_foreach_safe(l, &v->layers, layers) {
		assert(l->id < max_id());
			if (l->is_top)
				push_layer(v, l);
		}
		--v->top;
	} else if (v->top < 0) {
		list_foreach_safe(l, &v->layers, layers) {
		assert(l->id < max_id());
			if (l->is_top)
				pop_layer(v, l);
		}
		++v->top;
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
		list_foreach(l, &v->layers, layers) {
			float a = array_size(l->falls) / 16.;
			float b = array_size(l->cells) * l->v->top;
			if (a > b)
				a = b;
			b = a / array_size(l->falls);
			array_foreach(p, l->falls)
				flowsim_add(f, p, b);
			l->v->v -= a;
		}
	}
	list_foreach_safe(v, &f->volumes, volumes) {
		if (v->v <= 0) {
			list_unlink(&v->volumes);
			fs_volume_destroy(v);
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
	while (world_get_shape(f->w, p) == SHAPE_NONE) {
		l = world_get_data(f->w, p);
		if (l != NULL) {
			l->v->v += k;
			return 1;
		}
		--p.y;
	}
	++p.y;
	v = fs_volume(f->w);
	list_append(&f->volumes, &v->volumes);
	l = fs_layer(v, p.y);
	array_push(l->cells, &p);
	world_set_data(f->w, p, l);
	calculate_falls(l);
	l->is_top = 1;
	list_prepend(&v->layers, &l->layers);
	v->v += k;
	return 1;
}
