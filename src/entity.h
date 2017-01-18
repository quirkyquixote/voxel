/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_ENTITY_H_
#define SRC_ENTITY_H_

#include "sz.h"
#include "inventory.h"
#include "v3.h"

class Context;

class Entity {
 public:
	Entity(Context *ctx, int capacity)
		: ctx(ctx), items(capacity, Item(0, 0, 0)) { }

	virtual ~Entity() { }

	virtual void update() { }
	virtual void render() { }
	virtual sz_Tag *save();
	virtual void load(sz_Tag *val);
	virtual bool use() { return false; }
	virtual Inventory *get_items() { return &items; }

	inline const v3ll &get_p() const { return p; }
	inline void set_p(const v3ll &new_p) { p = new_p; }

	virtual const char *get_name() = 0;

 protected:
	Context *ctx;
	v3ll p;
	Inventory items;
};

Entity *load_entity(Context *ctx, sz_Tag *val);

#endif  // SRC_ENTITY_H_
