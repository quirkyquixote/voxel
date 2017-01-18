
#ifndef VOXEL_UPDATE_H_
#define VOXEL_UPDATE_H_

#include "context.h"
#include "inventory.h"

void update(Context *ctx);

void use_inventory(Context *ctx, std::vector<Item> *inv);
void use_workbench(Context *ctx, std::vector<Item> *inv);

#endif
