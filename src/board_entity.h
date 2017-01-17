
#ifndef VOXEL_BOARD_ENTITY_H_
#define VOXEL_BOARD_ENTITY_H_

#include "block_entity.h"

struct board_entity {
	struct block_entity block;
};

struct entity_traits board_traits;

struct board_entity *board_entity(struct context *ctx);

#endif
