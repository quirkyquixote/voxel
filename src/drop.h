
#ifndef VOXEL_DROP_H_
#define VOXEL_DROP_H_

#include "context.h"
#include "entity.h"

struct drop {
	struct entity entity;
	struct context *ctx;
	uint8_t obj;
	uint8_t mat;
	uint8_t num;
	int ticks;
};

struct drop *drop(struct context *ctx, uint8_t obj, uint8_t mat, uint8_t num);

#endif

