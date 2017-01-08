
#include "recipes.h"

const struct recipe recipes[] = {
	{ OBJ_WORKBENCH, 1, {
		OBJ_BLOCK, OBJ_BLOCK, -1,
		OBJ_BLOCK, OBJ_BLOCK, -1,
		-1, -1, -1,
	} }, { OBJ_WORKBENCH, 1, {
		-1, OBJ_BLOCK, OBJ_BLOCK,
		-1, OBJ_BLOCK, OBJ_BLOCK,
		-1, -1, -1,
	} }, { OBJ_WORKBENCH, 1, {
		-1, -1, -1,
		OBJ_BLOCK, OBJ_BLOCK, -1,
		OBJ_BLOCK, OBJ_BLOCK, -1,
	} }, { OBJ_WORKBENCH, 1, {
		-1, -1, -1,
		-1, OBJ_BLOCK, OBJ_BLOCK,
		-1, OBJ_BLOCK, OBJ_BLOCK,
	} }, { OBJ_SLAB, 6, {
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
	} }, { OBJ_CRATE, 1, {
		-1, OBJ_PANE, -1,
		OBJ_PANE, OBJ_PANE, OBJ_PANE,
		-1, OBJ_PANE, -1,
	} }, { OBJ_PIPE, 1, {
		-1, OBJ_PANE, -1,
		OBJ_PANE, -1, OBJ_PANE,
		-1, OBJ_PANE, -1,
	} }, { 0, 0, {} }
};

int recipe_match(const struct recipe *r, struct inventory *inv)
{
	int i;
	for (i = 0; i < 9; ++i)	{
		if (r->pattern[i] < 0) {
			if (inv->slots[i].num > 0)
				return 0;
		} else {
			if (inv->slots[i].num == 0 || r->pattern[i] != inv->slots[i].obj)
				return 0;
		}
	}
	return 1;
}
