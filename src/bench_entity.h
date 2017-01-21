/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_BENCH_ENTITY_H_
#define SRC_BENCH_ENTITY_H_

#include <memory>

#include "block_entity.h"
#include "callback.h"

class BenchEntity : public BlockEntity {
 public:
	explicit BenchEntity(Context *ctx);
	~BenchEntity();

	sz_Tag *save();
	void load(sz_Tag *val);
	bool use(PlayerEntity *player);

	const char *get_name() { return "bench"; }

private:
	std::unique_ptr<Callback> render_callback;
};

#endif  // SRC_BENCH_ENTITY_H_
