

#ifndef VOXEL_ENTITY_H_
#define VOXEL_ENTITY_H_

#include "list.h"
#include "physics.h"

struct entity_traits {
	void (* update_func)(void *);
	void (* render_func)(void *);
	void (* destroy_func)(void *);
};

struct entity {
	const struct entity_traits *traits;
	struct list entities;
	struct body *body;
	int die;
};

#endif
