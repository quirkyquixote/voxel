
#ifndef VOXEL_UPDATE_H_
#define VOXEL_UPDATE_H_

#include "context.h"
#include "inventory.h"

void update(void *data);

void use_inventory(struct context *ctx, struct array *inv);
void use_workbench(struct context *ctx, struct array *inv);

#endif
