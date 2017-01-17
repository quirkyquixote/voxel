

#ifndef VOXEL_ENTITY_H_
#define VOXEL_ENTITY_H_

#include "list.h"
#include "physics.h"
#include "sz.h"

struct entity_traits {
	const char *name;
	void (* update_func)(void *);
	void (* render_func)(void *);
	void (* destroy_func)(void *);
	int (* save_func)(void *, union sz_tag *);
	int (* load_func)(void *, union sz_tag *);

};

struct entity {
	const struct entity_traits *traits;
	struct list entities;
	struct body *body;
	int die;
};

#endif
