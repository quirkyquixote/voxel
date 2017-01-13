
#include "recipes.h"

const struct recipe recipes[] = {
	{ OBJ_SLAB, 6, {
		OBJ_BLOCK, OBJ_BLOCK, OBJ_BLOCK,
		-1, -1, -1,
		-1, -1, -1,
	} }, { OBJ_SLAB, 6, {
		-1, -1, -1,
		OBJ_BLOCK, OBJ_BLOCK, OBJ_BLOCK,
		-1, -1, -1,
	} }, { OBJ_SLAB, 6, {
		-1, -1, -1,
		-1, -1, -1,
		OBJ_BLOCK, OBJ_BLOCK, OBJ_BLOCK,
	} }, { OBJ_SLAB, 6, {
		OBJ_BLOCK, -1, -1,
		OBJ_BLOCK, -1, -1,
		OBJ_BLOCK, -1, -1,
	} }, { OBJ_SLAB, 6, {
		-1, OBJ_BLOCK, -1,
		-1, OBJ_BLOCK, -1,
		-1, OBJ_BLOCK, -1,
	} }, { OBJ_SLAB, 6, {
		-1, -1, OBJ_BLOCK,
		-1, -1, OBJ_BLOCK,
		-1, -1, OBJ_BLOCK,
	} }, { OBJ_STAIRS, 6, {
		OBJ_BLOCK, -1, -1,
		OBJ_BLOCK, OBJ_BLOCK, -1,
		OBJ_BLOCK, OBJ_BLOCK, OBJ_BLOCK,
	} }, { OBJ_STAIRS, 6, {
		-1, -1, OBJ_BLOCK
		-1, OBJ_BLOCK, OBJ_BLOCK,
		OBJ_BLOCK, OBJ_BLOCK, OBJ_BLOCK,
	} }, { OBJ_STAIRS, 6, {
		OBJ_BLOCK, OBJ_BLOCK, OBJ_BLOCK,
		-1, OBJ_BLOCK, OBJ_BLOCK,
		-1, -1, OBJ_BLOCK
	} }, { OBJ_STAIRS, 6, {
		OBJ_BLOCK, OBJ_BLOCK, OBJ_BLOCK,
		OBJ_BLOCK, OBJ_BLOCK, -1,
		OBJ_BLOCK, -1, -1,
	} }, { 0, 0, {} }
};

int recipe_match(const struct recipe *r, struct array *inv)
{
	int i;
	struct slot s;
	for (i = 0; i < 9; ++i)	{
		s = inventory_get(inv, i);
		if (r->pattern[i] < 0) {
			if (s.num > 0)
				return 0;
		} else {
			if (s.num == 0 || r->pattern[i] != s.obj)
				return 0;
		}
	}
	return 1;
}
