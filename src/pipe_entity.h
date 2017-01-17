
#ifndef VOXEL_PIPE_ENTITY_H_
#define VOXEL_PIPE_ENTITY_H_

#include "block_entity.h"

struct pipe_entity {
	struct block_entity block;
	struct array *items;
};

struct entity_traits pipe_traits;

struct pipe_entity *pipe_entity(struct context *ctx);

#endif
