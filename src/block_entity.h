
#ifndef VOXEL_BLOCK_ENTITY_H_
#define VOXEL_BLOCK_ENTITY_H_

#include "entity.h"

class BlockEntity : public Entity {
public:
	BlockEntity(Context *ctx, int capacity) : Entity(ctx, capacity) { }

	virtual ~BlockEntity() { }
};

#endif
