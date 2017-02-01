/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_RECIPES_H_
#define SRC_RECIPES_H_

#include <vector>

#include "inventory.h"
#include "box2.h"

class CraftGrid {
 public:
	CraftGrid(std::vector<Item> *inv) : inv(inv), box(0, 0, 2, 2) {}
	~CraftGrid() {}
	const box2ll &get_box() const { return box; }
	const Item &get_item(const v2ll &p) const { return (*inv)[p.x * 3 + p.y]; }
	Item &get_item(const v2ll &p) { return (*inv)[p.x * 3 + p.y]; }
 private:
	box2ll box;
	std::vector<Item> *inv;
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

bool match_recipe(const CraftGrid &grid, RecipeMatch *m);
int match_recipes(const CraftGrid &grid, std::vector<RecipeMatch> *matches);
void exec_recipe(const Recipe &r, const v2ll &p, int times, CraftGrid *grid);

#endif  // SRC_RECIPES_H_

