/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_DROP_ENTITY_H_
#define SRC_DROP_ENTITY_H_

#include <memory>

#include "roaming_entity.h"
#include "callback.h"

class DropEntity : public RoamingEntity {
 public:
	DropEntity(Context *ctx, Item item);
	~DropEntity();

	void load(sz_Tag *root);
	sz_Tag *save();

	const char *get_name() { return "drop"; }

 private:
	std::unique_ptr<Callback> render_func;
	std::unique_ptr<Callback> update_func;
	int ticks;

	void callback(Body *b, const v3ll &p, int face);
	void update();
	void render();
};

#endif  // SRC_DROP_ENTITY_H_

