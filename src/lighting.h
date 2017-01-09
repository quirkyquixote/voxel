
#ifndef VOXEL_LIGHTING_H_
#define VOXEL_LIGHTING_H_

#include "chunk.h"

static inline int max(int a, int b)
{
	return a > b ? a : b;
}

void update_lighting(struct world *w, struct aab3ll bb, struct aab3ll *rbb);

#endif
