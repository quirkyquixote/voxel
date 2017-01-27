/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_ENTITY_H_
#define SRC_ENTITY_H_

#include "sz.h"
#include "inventory.h"
#include "v3.h"
#include "physics.h"

class Context;
class PlayerEntity;

class Entity : public NonCopyable {
 public:
	Entity() = delete;
	Entity(Context *ctx, int capacity)
		: ctx(ctx), items(capacity, Item(0, 0, 0)) { }

	virtual ~Entity() { }

	virtual sz::Tag *save();
	virtual void load(sz::Tag *val);
	virtual bool use(PlayerEntity *player) { return false; }
	virtual Inventory *get_items() { return &items; }

	inline const v3ll &get_p() const { return p; }
	inline void set_p(const v3ll &new_p) { p = new_p; }

	virtual const char *get_name() = 0;

 protected:
	Context *ctx;
	v3ll p;
	Inventory items;
};

Entity *load_entity(Context *ctx, sz::Tag *val);

#endif  // SRC_ENTITY_H_
