
#include "recipes.h"

struct ingredient {
	int obj;
	int mat;
};

struct recipe {
	struct ingredient result;
	int amount;
	struct ingredient pattern[9];
};

#define PATTERN_1x1(o1,m1,num,o2,m2) \
	{ { o1, m1 }, num, { \
		{ o2, m2 }, { -1, -1 }, { -1, -1 }, \
		{ -1, -1 }, { -1, -1 }, { -1, -1 }, \
		{ -1, -1 }, { -1, -1 }, { -1, -1 }, \
	} }, \
	{ { o1, m1 }, num, { \
		{ -1, -1 }, { o2, m2 }, { -1, -1 }, \
		{ -1, -1 }, { -1, -1 }, { -1, -1 }, \
		{ -1, -1 }, { -1, -1 }, { -1, -1 }, \
	} }, \
	{ { o1, m1 }, num, { \
		{ -1, -1 }, { -1, -1 }, { -1, -1 }, \
		{ -1, -1 }, { o2, m2 }, { -1, -1 }, \
		{ -1, -1 }, { -1, -1 }, { -1, -1 }, \
	} }

#define PATTERN_2x1(o1,m1,num,o2,m2) \
	{ { o1, m1 }, num, { \
		{ o2, m2 }, { -1, -1 }, { -1, -1 }, \
		{ o2, m2 }, { -1, -1 }, { -1, -1 }, \
		{ -1, -1 }, { -1, -1 }, { -1, -1 }, \
	} }, \
	{ { o1, m1 }, num, { \
		{ -1, -1 }, { o2, m2 }, { -1, -1 }, \
		{ -1, -1 }, { o2, m2 }, { -1, -1 }, \
		{ -1, -1 }, { -1, -1 }, { -1, -1 }, \
	} }, \
	{ { o1, m1 }, num, { \
		{ -1, -1 }, { -1, -1 }, { o2, m2 }, \
		{ -1, -1 }, { -1, -1 }, { o2, m2 }, \
		{ -1, -1 }, { -1, -1 }, { -1, -1 }, \
	} }

#define PATTERN_3x1(o1,m1,num,o2,m2) \
	{ { o1, m1 }, num, { \
		{ o2, m2 }, { -1, -1 }, { -1, -1 }, \
		{ o2, m2 }, { -1, -1 }, { -1, -1 }, \
		{ o2, m2 }, { -1, -1 }, { -1, -1 }, \
	} }, \
	{ { o1, m1 }, num, { \
		{ -1, -1 }, { o2, m2 }, { -1, -1 }, \
		{ -1, -1 }, { o2, m2 }, { -1, -1 }, \
		{ -1, -1 }, { o2, m2 }, { -1, -1 }, \
	} }

#define PATTERN_2x2(o1,m1,num,o2,m2) \
	{ { o1, m1 }, num, { \
		{ o2, m2 }, { o2, m2 }, { -1, -1 }, \
		{ o2, m2 }, { o2, m2 }, { -1, -1 }, \
		{ -1, -1 }, { -1, -1 }, { -1, -1 }, \
	} }

#define PATTERN_DIAMOND(o1,m1,num,o2,m2) \
	{ { o1, m1 }, num, { \
		{ -1, -1 }, { o2, m2 }, { -1, -1 }, \
		{ o2, m2 }, { -1, -1 }, { o2, m2 }, \
		{ -1, -1 }, { o2, m2 }, { -1, -1 }, \
	} }

#define PATTERN_CROSS(o1,m1,num,o2,m2) \
	{ { o1, m1 }, num, { \
		{ -1, -1 }, { o2, m2 }, { -1, -1 }, \
		{ o2, m2 }, { o2, m2 }, { o2, m2 }, \
		{ -1, -1 }, { o2, m2 }, { -1, -1 }, \
	} }


#define COMMON_RECIPES(mat)\
	PATTERN_3x1(OBJ_SLAB, mat, 6, OBJ_BLOCK, mat), \
	PATTERN_3x1(OBJ_PANE, mat, 6, OBJ_SLAB, mat), \
	{ { OBJ_STAIRS, mat }, 6, {\
		{ OBJ_BLOCK, mat },\
		{ OBJ_BLOCK, mat },\
		{ OBJ_BLOCK, mat },\
		{ OBJ_BLOCK, mat },\
		{ OBJ_BLOCK, mat },\
		{ -1, -1 },\
		{ OBJ_BLOCK, mat },\
		{ -1, -1 },\
		{ -1, -1 },\
	} }

#define STONE_RECIPES(mat) \
	COMMON_RECIPES(mat),\
	COMMON_RECIPES(mat##_COBBLE),\
	COMMON_RECIPES(mat##_LBRICK),\
	COMMON_RECIPES(mat##_SBRICK),\
	COMMON_RECIPES(mat##_TILE),\
	COMMON_RECIPES(mat##_COLUMN),\
	COMMON_RECIPES(mat##_BLOCK), \
	PATTERN_1x1(OBJ_BLOCK, mat##_BLOCK, 1, OBJ_BLOCK, mat), \
	PATTERN_1x1(OBJ_BLOCK, mat##_LBRICK, 1, OBJ_BLOCK, mat##_BLOCK), \
	PATTERN_1x1(OBJ_BLOCK, mat##_SBRICK, 1, OBJ_BLOCK, mat##_LBRICK), \
	PATTERN_1x1(OBJ_BLOCK, mat##_TILE, 1, OBJ_BLOCK, mat##_SBRICK), \
	PATTERN_2x2(OBJ_BLOCK, mat##_BENCH, 1, OBJ_BLOCK, mat), \
	PATTERN_CROSS(OBJ_BLOCK, mat##_CRATE, 1, OBJ_PANE, mat), \
	PATTERN_DIAMOND(OBJ_BLOCK, mat##_PIPE, 1, OBJ_PANE, mat)

#define WOOD_RECIPES(mat)\
	PATTERN_1x1(OBJ_BLOCK, mat, 4, OBJ_BLOCK, mat##_LOG), \
	COMMON_RECIPES(mat), \
	PATTERN_2x2(OBJ_BLOCK, mat##_BENCH, 1, OBJ_BLOCK, mat), \
	PATTERN_CROSS(OBJ_BLOCK, mat##_CRATE, 1, OBJ_PANE, mat), \
	PATTERN_DIAMOND(OBJ_BLOCK, mat##_PIPE, 1, OBJ_PANE, mat)

static const struct recipe recipes[] = {
	STONE_RECIPES(MAT_LIMESTONE),
	STONE_RECIPES(MAT_SANDSTONE),
	STONE_RECIPES(MAT_MARBLE),
	STONE_RECIPES(MAT_GRANITE),
	STONE_RECIPES(MAT_BASALT),
	STONE_RECIPES(MAT_SLATE),
	STONE_RECIPES(MAT_CLAY),
	WOOD_RECIPES(MAT_WOOD1),
	WOOD_RECIPES(MAT_WOOD2),
	WOOD_RECIPES(MAT_WOOD3),
	WOOD_RECIPES(MAT_WOOD4),
	WOOD_RECIPES(MAT_WOOD5),
	WOOD_RECIPES(MAT_WOOD6),
	WOOD_RECIPES(MAT_WOOD7),
};

int rotations[4][9] = {
	{ 0, 1, 2, 3, 4, 5, 6, 7, 8 },
	{ 6, 3, 0, 7, 4, 1, 8, 5, 2 },
	{ 8, 7, 6, 5, 4, 3, 2, 1, 0 },
	{ 2, 5, 8, 1, 4, 7, 0, 3, 6 },
};

int rotation_match(const struct recipe *r, struct array *inv, int *rot, struct slot *rval)
{
	int k;
	struct slot s;
	struct ingredient i;
	for (k = 0; k < 9; ++k)	{
		s = inventory_get(inv, k);
		i = r->pattern[rot[k]];
		if (i.obj < 0) {
			if (s.num > 0)
				return 0;
		} else if (s.num == 0 || i.obj != s.obj || i.mat != s.mat) {
			return 0;
		}
	}
	for (k = 0; k < 9; ++k)	{
		s = inventory_get(inv, k);
		i = r->pattern[rot[k]];
		if (i.obj >= 0)
			inventory_set_num(inv, k, s.num - 1);
	}
	rval->obj = r->result.obj;
	rval->mat = r->result.mat;
	rval->num = r->amount;
	return 1;
}

int recipe_match(struct array *inv, struct slot *rval)
{
	int i, j;
	for (i = 0; i < sizeof(recipes) / sizeof(*recipes); ++i) {
		for (j = 0; j < 4; ++j) {
			if (rotation_match(&recipes[i], inv, rotations[j], rval))
				return 1;
		}
	}
	return 0;
}

