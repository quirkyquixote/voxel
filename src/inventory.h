
#ifndef VOXEL_INVENTORY_H_
#define VOXEL_INVENTORY_H_

#include <stdint.h>
#include <stdlib.h>

#include "types.h"
#include "array.h"

struct slot {
	uint8_t obj;
	uint8_t mat;
	uint8_t num;
};

static inline struct slot slot(int obj, int mat, int num)
{
	return (struct slot){ obj, mat, num };
}

static inline struct array *inventory(size_t size)
{
	struct array *i = array(sizeof(struct slot));
	array_resize(i, size);
	return i;
}

static inline struct slot inventory_get(struct array *a, int i)
{
	struct slot s;
	array_get(a, i, &s);
	return s;
}

static inline void inventory_set(struct array *a, int i, struct slot s)
{
	array_set(a, i, &s);
}

static inline int inventory_get_obj(struct array *a, int i)
{
	struct slot s;
	array_get(a, i, &s);
	return s.obj;
}

static inline int inventory_get_mat(struct array *a, int i)
{
	struct slot s;
	array_get(a, i, &s);
	return s.mat;
}

static inline int inventory_get_num(struct array *a, int i)
{
	struct slot s;
	array_get(a, i, &s);
	return s.num;
}

static inline int inventory_set_obj(struct array *a, int i, int obj)
{
	struct slot s;
	array_get(a, i, &s);
	s.obj = obj;
	array_set(a, i, &s);
}

static inline int inventory_set_mat(struct array *a, int i, int mat)
{
	struct slot s;
	array_get(a, i, &s);
	s.mat = mat;
	array_set(a, i, &s);
}

static inline int inventory_set_num(struct array *a, int i, int num)
{
	struct slot s;
	array_get(a, i, &s);
	s.num = num;
	array_set(a, i, &s);
}

int inventory_add(struct array *i, struct slot s);
int inventory_add_to_slot(struct array *i, int slot, struct slot s);
int inventory_remove(struct array *i, struct slot s);
int inventory_remove_from_slot(struct array  *i, int slot, struct slot s);

#endif
