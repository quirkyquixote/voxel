/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_CRATE_ENTITY_H_
#define SRC_CRATE_ENTITY_H_

#include <memory>

#include "block_entity.h"
#include "callback.h"

class CrateEntity : public BlockEntity {
 public:
	explicit CrateEntity(Context *ctx);
	~CrateEntity();

	sz_Tag *save();
	void load(sz_Tag *val);
	bool use(PlayerEntity *player);

	const char *get_name() { return "crate"; }

 private:
	std::unique_ptr<Callback> render_func;

	void render();
};

#endif  // SRC_CRATE_ENTITY_H_
