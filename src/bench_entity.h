
#ifndef VOXEL_BENCH_ENTITY_H_
#define VOXEL_BENCH_ENTITY_H_

#include "block_entity.h"

class BenchEntity : public BlockEntity {
public:
	BenchEntity(Context *ctx);
	~BenchEntity();

	void update();
	void render();
	sz_Tag *save();
	void load(sz_Tag *val);
	bool use();

	const char *get_name() { return "bench"; }
};

#endif
