
#ifndef VOXEL_DROP_H_
#define VOXEL_DROP_H_

#include "roaming_entity.h"

struct drop {
	struct roaming_entity roaming;
	struct item item;
	int ticks;
};

struct drop *drop(struct context *ctx, struct item item);

#endif

