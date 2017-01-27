/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_LIGHTING_H_
#define SRC_LIGHTING_H_

#include <vector>

#include "world.h"
#include "box2.h"

class Lighting : public NonCopyable {
 public:
	explicit Lighting(World *w);
	~Lighting();

	void init(const box2ll &bb);
	void update(const box3ll &bb, box3ll *rbb);
 private:
	World *w;
	std::vector<v3ll> s1;
	std::vector<v3ll> s2;

	void find_boundary(const v3ll &p, int k);
	void lit_up(const v3ll &p, int k, int f1, int f2);
	void strange_blocks(const box3ll &bb);
};

#endif  // SRC_LIGHTING_H_

