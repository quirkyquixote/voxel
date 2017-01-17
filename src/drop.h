
#ifndef VOXEL_DROP_H_
#define VOXEL_DROP_H_

#include "context.h"
#include "entity.h"

struct drop {
	struct entity entity;
	struct context *ctx;
	struct item item;
	int ticks;
};

struct drop *drop(struct context *ctx, struct item item);

#endif

