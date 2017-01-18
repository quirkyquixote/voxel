/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_BENCH_ENTITY_H_
#define SRC_BENCH_ENTITY_H_

#include "block_entity.h"

class BenchEntity : public BlockEntity {
 public:
	explicit BenchEntity(Context *ctx);
	~BenchEntity();

	void update();
	void render();
	sz_Tag *save();
	void load(sz_Tag *val);
	bool use(PlayerEntity *player);

	const char *get_name() { return "bench"; }
};

#endif  // SRC_BENCH_ENTITY_H_
