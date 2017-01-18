/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_BOARD_ENTITY_H_
#define SRC_BOARD_ENTITY_H_

#include "block_entity.h"

class BoardEntity : public BlockEntity {
 public:
	explicit BoardEntity(Context *ctx);
	~BoardEntity();

	void update();
	void render();
	sz_Tag *save();
	void load(sz_Tag *val);
	bool use();

	const char *get_name() { return "board"; }
};

#endif  // SRC_BOARD_ENTITY_H_
