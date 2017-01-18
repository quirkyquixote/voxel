
#ifndef VOXEL_CRATE_ENTITY_H_
#define VOXEL_CRATE_ENTITY_H_

#include "block_entity.h"

class CrateEntity : public BlockEntity {
public:
	CrateEntity(Context *ctx);
	~CrateEntity();

	void update();
	void render();
	sz_Tag *save();
	void load(sz_Tag *val);
	bool use();

	const char *get_name() { return "crate"; }
};

#endif
