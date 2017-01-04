
#ifndef VOXEL_AAB3I_H_
#define VOXEL_AAB3I_H_

#include <stdint.h>

struct aab3i {
	int64_t x0, y0, z0;
	int64_t x1, y1, z1;
};

static inline struct aab3i aab3i(int64_t x0, int64_t y0, int64_t z0, int64_t x1, int64_t y1, int64_t z1)
{
	return (struct aab3i){ x0, y0, z0, x1, y1, z1 };
}

static inline int aab3i_overlap(struct aab3i *b1, struct aab3i *b2)
{
	return b2->x0 <= b1->x1 && b2->x1 >= b1->x0 &&
		b2->y0 <= b1->y1 && b2->y1 >= b1->y0 &&
		b2->z0 <= b1->z1 && b2->z1 >= b1->z0;
}

#endif
