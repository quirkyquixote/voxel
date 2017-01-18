
#ifndef VOXEL_INVENTORY_H_
#define VOXEL_INVENTORY_H_

#include <stdint.h>
#include <stdlib.h>

#include <vector>

#include "types.h"

typedef std::vector<Item> Inventory;

int inventory_add(Inventory *i, Item s);
int inventory_add_to_item(Inventory *i, int slot, Item s);
int inventory_remove(Inventory *i, Item s);
int inventory_remove_from_item(Inventory *i, int slot, Item s);

#endif
