
#ifndef VOXEL_CRATE_ENTITY_H_
#define VOXEL_CRATE_ENTITY_H_

#include "block_entity.h"

struct crate_entity {
	struct block_entity block;
	struct array *items;
};

struct entity_traits crate_traits;

struct crate_entity *crate_entity(struct context *ctx);

#endif
