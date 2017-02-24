/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_RECIPES_H_
#define SRC_RECIPES_H_

#include <cassert>

#include <vector>

#include "inventory.h"
#include "box2.h"

class CraftGrid {
 public:
	CraftGrid(int res) : res(res), dim(0, 0), box(0, 0, 0, 0) {}

	~CraftGrid() {}

	void add_inv(const v2ll &p, std::vector<Item> *inv);

	long long get_res() const
	{
		return res;
	}

	box2ll get_box() const
	{
		return box * res;
	}

	const Item &get_item(const v2ll &p) const
	{
		static const Item empty_item = { 0, 0, 0 };
		v2ll a(p / res);
		if (a.x < box.x0 || a.y < box.y0 || a.x >= box.x1 || a.y >= box.y1)
			return empty_item;
		a -= v2ll(box.x0, box.y0);
		v2ll b(p % res);
		return (*invv[a.x * dim.y + a.y])[b.x * res + b.y];
	}

	Item &get_item(const v2ll &p)
	{
		v2ll a(p / res);
		assert(a.x >= box.x0 || a.y >= box.y0 || a.x < box.x1 || a.y < box.y1);
		a -= v2ll(box.x0, box.y0);
		v2ll b(p % res);
		return (*invv[a.x * dim.y + a.y])[b.x * res + b.y];
	}

 private:
	long long res;
	v2ll dim;
	box2ll box;
	std::vector<std::vector<Item> *> invv;
};

struct Ingredient {
	v2ll p;
	Item item;
};

struct Recipe {
	Item result;
	Ingredient ingredients[9];
};

struct RecipeMatch {
	const Recipe *recipe;
	v2ll p;
	int times;
};

bool match_recipe(const CraftGrid &grid, const v2ll &p0, RecipeMatch *m);
int match_recipes(const CraftGrid &grid, const v2ll &p0, std::vector<RecipeMatch> *matches);
void exec_recipe(const Recipe &r, const v2ll &p, int times, CraftGrid *grid);

#endif  // SRC_RECIPES_H_

