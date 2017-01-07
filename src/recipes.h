
#ifndef VOXEL_RECIPES_H_
#define VOXEL_RECIPES_H_

#include <stdint.h>

#include "context.h"
#include "inventory.h"

struct recipe {
	uint8_t obj;
	uint8_t num;
	int pattern[9];
};

extern const struct recipe recipes[];

int recipe_match(const struct recipe *r, struct inventory *inv);

#endif

