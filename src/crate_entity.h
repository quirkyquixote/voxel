/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_CRATE_ENTITY_H_
#define SRC_CRATE_ENTITY_H_

#include "block_entity.h"

class CrateEntity : public BlockEntity {
 public:
	explicit CrateEntity(Context *ctx);
	~CrateEntity();

	void update();
	void render();
	sz_Tag *save();
	void load(sz_Tag *val);
	bool use(PlayerEntity *player);

	const char *get_name() { return "crate"; }
};

#endif  // SRC_CRATE_ENTITY_H_
