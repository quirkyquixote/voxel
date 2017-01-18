
#ifndef VOXEL_PIPE_ENTITY_H_
#define VOXEL_PIPE_ENTITY_H_

#include "block_entity.h"

class PipeEntity : public BlockEntity {
public:
	PipeEntity(Context *ctx);
	~PipeEntity();

	void update();
	void render();
	sz_Tag *save();
	void load(sz_Tag *val);
	bool use();

	const char *get_name() { return "pipe"; }
};

#endif
