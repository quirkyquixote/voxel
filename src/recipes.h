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

	void add_inv(const v2ll &p, std::vector<Item> *inv)
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

