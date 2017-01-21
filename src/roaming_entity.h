/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_ROAMING_ENTITY_H_
#define SRC_ROAMING_ENTITY_H_

#include <memory>

#include "entity.h"
#include "physics.h"
#include "list.h"

class RoamingEntity : public Entity, public Node {
 public:
	RoamingEntity(Context *ctx, int capacity);
	virtual ~RoamingEntity();

	virtual sz_Tag *save();
	virtual void load(sz_Tag *root);

	inline Body *get_body() { return body.get(); }

 protected:
	std::unique_ptr<Body> body;
};

#endif  // SRC_ROAMING_ENTITY_H_
