
#ifndef VOXEL_INVENTORY_H_
#define VOXEL_INVENTORY_H_

#include <stdint.h>
#include <stdlib.h>

#include "types.h"
#include "array.h"

static inline struct array *inventory(size_t size)
{
	struct array *i = array(sizeof(struct item));
	array_resize(i, size);
	return i;
}

static inline struct item inventory_get(struct array *a, int i)
{
	struct item s;
	array_get(a, i, &s);
	return s;
}

static inline void inventory_set(struct array *a, int i, struct item s)
{
	array_set(a, i, &s);
}

static inline int inventory_get_obj(struct array *a, int i)
{
	struct item s;
	array_get(a, i, &s);
	return s.obj;
}

static inline int inventory_get_mat(struct array *a, int i)
{
	struct item s;
	array_get(a, i, &s);
	return s.mat;
}

static inline int inventory_get_num(struct array *a, int i)
{
	struct item s;
	array_get(a, i, &s);
	return s.num;
}

static inline int inventory_set_obj(struct array *a, int i, int obj)
{
	struct item s;
	array_get(a, i, &s);
	s.obj = obj;
	array_set(a, i, &s);
}

static inline int inventory_set_mat(struct array *a, int i, int mat)
{
	struct item s;
	array_get(a, i, &s);
	s.mat = mat;
	array_set(a, i, &s);
}

static inline int inventory_set_num(struct array *a, int i, int num)
{
	struct item s;
	array_get(a, i, &s);
	s.num = num;
	array_set(a, i, &s);
}

int inventory_add(struct array *i, struct item s);
int inventory_add_to_item(struct array *i, int slot, struct item s);
int inventory_remove(struct array *i, struct item s);
int inventory_remove_from_item(struct array  *i, int slot, struct item s);

#endif
