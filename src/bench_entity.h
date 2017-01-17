
#ifndef VOXEL_BENCH_ENTITY_H_
#define VOXEL_BENCH_ENTITY_H_

#include "block_entity.h"

struct bench_entity {
	struct block_entity block;
};

struct entity_traits bench_traits;

struct bench_entity *bench_entity(struct context *ctx);

#endif
