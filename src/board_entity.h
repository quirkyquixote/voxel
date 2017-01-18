
#ifndef VOXEL_BOARD_ENTITY_H_
#define VOXEL_BOARD_ENTITY_H_

#include "block_entity.h"

class BoardEntity : public BlockEntity {
public:
	BoardEntity(Context *ctx);
	~BoardEntity();

	void update();
	void render();
	sz_Tag *save();
	void load(sz_Tag *val);
	bool use();

	const char *get_name() { return "board"; }
};

#endif
