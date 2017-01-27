/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_BOARD_ENTITY_H_
#define SRC_BOARD_ENTITY_H_

#include "block_entity.h"

#include <memory>

#include "context.h"

#define BOARD_EXISTS 0x1
#define BOARD_ON 0x2

class BoardEntity : public BlockEntity {
 public:
	explicit BoardEntity(Context *ctx);
	~BoardEntity();

	sz::Tag *save();
	void load(sz::Tag *val);
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
