/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "recipes.h"

#include "log.h"

#define PATTERN_1x1(o1, m1, num, o2, m2) \
	{ { o1, m1, num }, { \
		{ { 0, 0 }, { o2, m2, 1 } }, \
	} }

#define PATTERN_2x1(o1, m1, num, o2, m2) \
	{ { o1, m1, num }, { \
		{ { 0, 0 }, { o2, m2, 1 } }, \
		{ { 0, 1 }, { o2, m2, 1 } }, \
	} }, \
	{ { o1, m1, num }, { \
		{ { 0, 0 }, { o2, m2, 1 } }, \
		{ { 1, 0 }, { o2, m2, 1 } }, \
	} }

#define PATTERN_3x1(o1, m1, num, o2, m2) \
	{ { o1, m1, num }, { \
		{ { 0, 0 }, { o2, m2, 1 } }, \
		{ { 0, 1 }, { o2, m2, 1 } }, \
		{ { 0, 2 }, { o2, m2, 1 } }, \
	} }, \
	{ { o1, m1, num }, { \
		{ { 0, 0 }, { o2, m2, 1 } }, \
		{ { 1, 0 }, { o2, m2, 1 } }, \
		{ { 2, 0 }, { o2, m2, 1 } }, \
	} }

#define PATTERN_2x2(o1, m1, num, o2, m2) \
	{ { o1, m1, num }, { \
		{ { 0, 0 }, { o2, m2, 1 } }, \
		{ { 0, 1 }, { o2, m2, 1 } }, \
		{ { 1, 0 }, { o2, m2, 1 } }, \
		{ { 1, 1 }, { o2, m2, 1 } }, \
	} }

#define PATTERN_DIAMOND(o1, m1, num, o2, m2) \
	{ { o1, m1, num }, { \
		{ { 0, 1 }, { o2, m2, 1 } }, \
		{ { 1, 0 }, { o2, m2, 1 } }, \
		{ { 2, 1 }, { o2, m2, 1 } }, \
		{ { 1, 2 }, { o2, m2, 1 } }, \
	} }

#define PATTERN_CROSS(o1, m1, num, o2, m2) \
	{ { o1, m1, num }, { \
		{ { 0, 1 }, { o2, m2, 1 } }, \
		{ { 1, 0 }, { o2, m2, 1 } }, \
		{ { 1, 1 }, { o2, m2, 1 } }, \
		{ { 2, 1 }, { o2, m2, 1 } }, \
		{ { 1, 2 }, { o2, m2, 1 } }, \
	} }

#define PATTERN_STAIRS(o1, m1, num, o2, m2) \
	{ { o1, m1, num }, { \
		{ { 0, 0 }, { o2, m2, 1 } }, \
		{ { 0, 1 }, { o2, m2, 1 } }, \
		{ { 0, 2 }, { o2, m2, 1 } }, \
		{ { 1, 0 }, { o2, m2, 1 } }, \
		{ { 1, 1 }, { o2, m2, 1 } }, \
		{ { 2, 0 }, { o2, m2, 1 } }, \
	} }, \
	{ { o1, m1, num }, { \
		{ { 0, 0 }, { o2, m2, 1 } }, \
		{ { 0, 1 }, { o2, m2, 1 } }, \
		{ { 0, 2 }, { o2, m2, 1 } }, \
		{ { 1, 1 }, { o2, m2, 1 } }, \
		{ { 1, 2 }, { o2, m2, 1 } }, \
		{ { 2, 2 }, { o2, m2, 1 } }, \
	} }, \
	{ { o1, m1, num }, { \
		{ { 0, 2 }, { o2, m2, 1 } }, \
		{ { 1, 1 }, { o2, m2, 1 } }, \
		{ { 1, 2 }, { o2, m2, 1 } }, \
		{ { 2, 0 }, { o2, m2, 1 } }, \
		{ { 2, 1 }, { o2, m2, 1 } }, \
		{ { 2, 2 }, { o2, m2, 1 } }, \
	} }, \
	{ { o1, m1, num }, { \
		{ { 0, 0 }, { o2, m2, 1 } }, \
		{ { 1, 0 }, { o2, m2, 1 } }, \
		{ { 1, 1 }, { o2, m2, 1 } }, \
		{ { 2, 0 }, { o2, m2, 1 } }, \
		{ { 2, 1 }, { o2, m2, 1 } }, \
		{ { 2, 2 }, { o2, m2, 1 } }, \
	} }


#define SELF_RECIPE(shape, mat)\
	PATTERN_1x1(shape, mat, 1, shape, mat)

#define COMMON_RECIPES(mat)\
	SELF_RECIPE(OBJ_BLOCK, mat), \
	SELF_RECIPE(OBJ_PANE, mat), \
	SELF_RECIPE(OBJ_SLAB, mat), \
	SELF_RECIPE(OBJ_STAIRS, mat), \
	PATTERN_3x1(OBJ_SLAB, mat, 6, OBJ_BLOCK, mat), \
	PATTERN_3x1(OBJ_PANE, mat, 6, OBJ_SLAB, mat), \
	PATTERN_STAIRS(OBJ_STAIRS, mat, 6, OBJ_BLOCK, mat)

#define STONE_RECIPES(mat) \
	COMMON_RECIPES(mat), \
	COMMON_RECIPES(mat##_COBBLE), \
	COMMON_RECIPES(mat##_LBRICK), \
	COMMON_RECIPES(mat##_SBRICK), \
	COMMON_RECIPES(mat##_TILE), \
	COMMON_RECIPES(mat##_COLUMN), \
	COMMON_RECIPES(mat##_BLOCK), \
	PATTERN_1x1(OBJ_BLOCK, mat##_BLOCK, 1, OBJ_BLOCK, mat), \
	PATTERN_1x1(OBJ_BLOCK, mat##_LBRICK, 1, OBJ_BLOCK, mat##_BLOCK), \
	PATTERN_1x1(OBJ_BLOCK, mat##_SBRICK, 1, OBJ_BLOCK, mat##_LBRICK), \
	PATTERN_1x1(OBJ_BLOCK, mat##_TILE, 1, OBJ_BLOCK, mat##_SBRICK), \
	SELF_RECIPE(OBJ_BLOCK, mat##_BENCH), \
	SELF_RECIPE(OBJ_BLOCK, mat##_CRATE), \
	SELF_RECIPE(OBJ_BLOCK, mat##_PIPE), \
	PATTERN_2x2(OBJ_BLOCK, mat##_BENCH, 1, OBJ_BLOCK, mat), \
	PATTERN_CROSS(OBJ_BLOCK, mat##_CRATE, 1, OBJ_PANE, mat), \
	PATTERN_DIAMOND(OBJ_BLOCK, mat##_PIPE, 1, OBJ_PANE, mat)

#define WOOD_RECIPES(mat)\
	SELF_RECIPE(OBJ_BLOCK, mat##_LOG), \
	PATTERN_1x1(OBJ_BLOCK, mat, 4, OBJ_BLOCK, mat##_LOG), \
	COMMON_RECIPES(mat), \
	SELF_RECIPE(OBJ_BLOCK, mat##_BENCH), \
	SELF_RECIPE(OBJ_BLOCK, mat##_CRATE), \
	SELF_RECIPE(OBJ_BLOCK, mat##_PIPE), \
	PATTERN_2x2(OBJ_BLOCK, mat##_BENCH, 1, OBJ_BLOCK, mat), \
	PATTERN_CROSS(OBJ_BLOCK, mat##_CRATE, 1, OBJ_PANE, mat), \
	PATTERN_DIAMOND(OBJ_BLOCK, mat##_PIPE, 1, OBJ_PANE, mat)

static const Recipe recipes[] = {
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

void CraftGrid::add_inv(const v2ll &p, std::vector<Item> *inv)
{
	if (invv.empty()) {
		invv.push_back(inv);
		box = box2ll(p.x, p.y, p.x + 1, p.y + 1);
		dim = v2ll(1, 1);
		return;
	}
	box2ll n_box;
	n_box.x0 = std::min(p.x, box.x0);
	n_box.x1 = std::max(p.x + 1, box.x1);
	n_box.y0 = std::min(p.y, box.y0);
	n_box.y1 = std::max(p.y + 1, box.y1);
	if (n_box != box) {
		v2ll n_dim;
		n_dim.x = n_box.x1 - n_box.x0;
		n_dim.y = n_box.y1 - n_box.y0;
		std::vector<std::vector<Item> *> n_invv(n_dim.x * n_dim.y, nullptr);
		for (auto p : box2ll(box.x0, box.y0, box.x1 - 1, box.y1 - 1)) {
			auto a = p - v2ll(box.x0, box.y0);
			auto b = p - v2ll(n_box.x0, n_box.y0);
			n_invv[b.x * n_dim.y + b.y] =
				invv[a.x * dim.y + a.y];
		}
		box = n_box;
		dim = n_dim;
		invv = n_invv;
	}
	auto a = p - v2ll(box.x0, box.y0);
	assert(a.x < dim.x && a.y < dim.y);
	assert(invv[a.x * dim.y + a.y] == nullptr);
	invv[a.x * dim.y + a.y] = inv;
}

bool match_recipe(const CraftGrid &grid, const v2ll &p0, RecipeMatch *m)
{
	for (auto p : grid.get_box()) {
		bool found = false;
		m->times = 64;
		for (auto i = m->recipe->ingredients; i->item.num != 0; ++i) {
			if (p0 == i->p + p)
				found = true;
			Item item = grid.get_item(i->p + p);
			if (item.num < i->item.num
					|| item.obj != i->item.obj
					|| item.mat != i->item.mat) {
				found = false;
				break;
			}
			if (m->times > item.num / i->item.num)
				m->times = item.num / i->item.num;
		}
		if (found) {
			m->p = p;
			return true;
		}
	}
	return false;
}

int match_recipes(const CraftGrid &grid, const v2ll &p0, std::vector<RecipeMatch> *matches)
{
	RecipeMatch m;
	for (m.recipe = recipes;
			m.recipe < recipes + sizeof(recipes) / sizeof(*recipes);
			++m.recipe) {
		if (match_recipe(grid, p0, &m))
			matches->push_back(m);
	}
	return matches->size();
}

void exec_recipe(const Recipe &r, const v2ll &p, int times, CraftGrid *grid)
{
	for (auto i = r.ingredients; i->item.num != 0; ++i)
		grid->get_item(i->p + p).num -= i->item.num * times;
}


