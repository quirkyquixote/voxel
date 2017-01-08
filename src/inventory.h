
#ifndef VOXEL_INVENTORY_H_
#define VOXEL_INVENTORY_H_

#include <stdint.h>
#include <stdlib.h>

struct slot {
	uint8_t obj;
	uint8_t mat;
	uint8_t num;
};

struct inventory {
	size_t size;
	struct slot *slots;
};

struct inventory *inventory(size_t size);
void inventory_destroy(struct inventory *i);
int inventory_resize(struct inventory *i, size_t size);
int inventory_add(struct inventory *i, int obj, int mat, int num);
int inventory_add_to_slot(struct inventory *i, int slot, int obj, int mat, int num);
int inventory_remove(struct inventory *i, int obj, int mat, int num);
int inventory_remove_from_slot(struct inventory  *i, int slot, int obj, int mat, int num);

#endif
