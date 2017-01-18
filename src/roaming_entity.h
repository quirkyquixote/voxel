
#ifndef VOXEL_ROAMING_ENTITY_H_
#define VOXEL_ROAMING_ENTITY_H_

#include "entity.h"

#include "physics.h"

class RoamingEntity : public Entity {
public:
	RoamingEntity(Context *ctx, int capacity);
	virtual ~RoamingEntity();

	virtual sz_Tag *save();
	virtual void load(sz_Tag *root);

	inline Body *get_body() { return body; }
	inline bool get_die() { return die; }

protected:
	Body *body;
	bool die;
};

#endif
