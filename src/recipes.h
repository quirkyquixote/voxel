
#ifndef VOXEL_RECIPES_H_
#define VOXEL_RECIPES_H_

#include <stdint.h>

#include "context.h"
#include "inventory.h"

int recipe_match(std::vector<Item> *inv, Item *s);

#endif

