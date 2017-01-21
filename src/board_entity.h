/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_BOARD_ENTITY_H_
#define SRC_BOARD_ENTITY_H_

#include "block_entity.h"

#include <memory>

#include "context.h"

class BoardEntity : public BlockEntity {
 public:
	explicit BoardEntity(Context *ctx);
	~BoardEntity();

	sz_Tag *save();
	void load(sz_Tag *val);
	bool use(PlayerEntity *player);

	const char *get_name() { return "board"; }

 private:
	std::unique_ptr<Callback> update_func;
	std::unique_ptr<Callback> render_func;
	unsigned char tmp[64];

	void update();
	void test(int i, int min, int max);
};

#endif  // SRC_BOARD_ENTITY_H_
