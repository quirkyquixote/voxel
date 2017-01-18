/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_BLOCK_ENTITY_H_
#define SRC_BLOCK_ENTITY_H_

#include "entity.h"

class BlockEntity : public Entity {
 public:
	BlockEntity(Context *ctx, int capacity) : Entity(ctx, capacity) { }

	virtual ~BlockEntity() { }
};

#endif  // SRC_BLOCK_ENTITY_H_
