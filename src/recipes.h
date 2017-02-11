/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_RECIPES_H_
#define SRC_RECIPES_H_

#include <cassert>

#include <vector>

#include "inventory.h"
#include "box2.h"

class CraftGrid {
 public:
	CraftGrid() : dim(0, 0), box(0, 0, 0, 0) {}

	~CraftGrid() {}

	void add_inv(const v2ll &p, std::vector<Item> *inv)
	{
		if (invv.empty()) {
			invv.push_back(inv);
			box = box2ll(p.x, p.y, p.x, p.y);
			dim = v2ll(1, 1);
			return;
		}
		box2ll n_box;
		n_box.x0 = std::min(p.x, box.x0);
		n_box.x1 = std::max(p.x, box.x1);
		n_box.y0 = std::min(p.y, box.y0);
		n_box.y1 = std::max(p.y, box.y1);
		if (n_box != box) {
			v2ll n_dim;
			n_dim.x = n_box.x1 - n_box.x0 + 1;
			n_dim.y = n_box.y1 - n_box.y0 + 1;
			std::vector<std::vector<Item> *> n_invv(n_dim.x * n_dim.y, nullptr);
			for (auto p : box) {
				auto a = p - v2ll(box.x0, box.y0);
				auto b = p - v2ll(n_box.x0, n_box.y0);
				n_invv[b.x * n_dim.y + b.y] =
					invv[a.x * dim.y + a.y];
			}
			box = n_box;
			dim = n_dim;
			invv = n_invv;
		}
		invv[p.x * dim.y + p.y] = inv;
	}

	box2ll get_box() const
	{
		return box2ll(0, 0, dim.x * 3 + 1, dim.y * 3 + 1);
	}

	const Item &get_item(const v2ll &p) const
	{
		static const Item empty_item = { 0, 0, 0 };
		v2ll a(p / 3LL);
		if (a.x < 0 || a.y < 0 || a.x >= dim.x || a.y >= dim.y)
			return empty_item;
		v2ll b(p % 3LL);
		return (*invv[a.x * dim.y + a.y])[b.x * 3 + b.y];
	}

	Item &get_item(const v2ll &p)
	{
		v2ll a(p / 3LL);
		assert(a.x >= 0 || a.y >= 0 || a.x < dim.x || a.y < dim.y);
		v2ll b(p % 3LL);
		return (*invv[a.x * dim.y + a.y])[b.x * 3 + b.y];
	}

 private:
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

bool match_recipe(const CraftGrid &grid, RecipeMatch *m);
int match_recipes(const CraftGrid &grid, std::vector<RecipeMatch> *matches);
void exec_recipe(const Recipe &r, const v2ll &p, int times, CraftGrid *grid);

#endif  // SRC_RECIPES_H_

