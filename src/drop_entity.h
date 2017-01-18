/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_DROP_ENTITY_H_
#define SRC_DROP_ENTITY_H_

#include "roaming_entity.h"

class DropEntity : public RoamingEntity {
 public:
	DropEntity(Context *ctx, Item item);
	~DropEntity();

	void update();
	void render();
	void load(sz_Tag *root);
	sz_Tag *save();

	const char *get_name() { return "drop"; }

 private:
	int ticks;

	void callback(Body *b, const v3ll &p, int face);
};

#endif  // SRC_DROP_ENTITY_H_
