
#ifndef VOXEL_AAB2I_H_
#define VOXEL_AAB2I_H_

#include <stdint.h>

struct aab2i {
	int64_t x0, y0;
	int64_t x1, y1;
};

static inline struct aab2i aab2i(int64_t x0, int64_t y0, int64_t x1, int64_t y1)
{
	return (struct aab2i){ x0, y0, x1, y1 };
}

static inline int aab2i_overlap(struct aab2i *b1, struct aab2i *b2)
{
	return b2->x0 <= b1->x1 && b2->x1 >= b1->x0 &&
		b2->y0 <= b1->y1 && b2->y1 >= b1->y0;
}

#endif
