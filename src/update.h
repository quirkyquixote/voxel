
#ifndef VOXEL_UPDATE_H_
#define VOXEL_UPDATE_H_

#include "context.h"
#include "inventory.h"

void update(void *data);

void update_inventory(struct context *ctx, struct array *inventory);

#endif
