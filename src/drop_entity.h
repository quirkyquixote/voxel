
#ifndef VOXEL_DROP_ENTITY_H_
#define VOXEL_DROP_ENTITY_H_

#include "roaming_entity.h"

struct drop_entity {
	struct roaming_entity roaming;
	struct item item;
	int ticks;
};

extern struct entity_traits drop_traits;

struct drop_entity *drop_entity(struct context *ctx, struct item item);

#endif

