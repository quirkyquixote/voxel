

#ifndef VOXEL_ENTITY_H_
#define VOXEL_ENTITY_H_

#include "context.h"

struct entity_traits {
	const char *name;
	void *(* create_func)(struct context *ctx);
	void (* update_func)(void *);
	void (* render_func)(void *);
	void (* destroy_func)(void *);
	int (* save_func)(void *, union sz_tag *);
	int (* load_func)(void *, char *, union sz_tag *);
	int (* use_func)(void *);
	struct list list;
};

struct entity {
	const struct entity_traits *traits;
	struct context *ctx;
	struct array *items;
};

static inline void *entity(struct context *ctx, struct entity_traits *traits)
{
	return traits->create_func(ctx);
}

static inline void entity_update(struct entity *e)
{
	e->traits->update_func(e);
}

static inline void entity_render(struct entity *e)
{
	e->traits->render_func(e);
}

static inline void entity_destroy(struct entity *e)
{
	e->traits->destroy_func(e);
}

static inline void *entity_load(struct context *ctx, union sz_tag *root)
{
	struct entity_traits *traits;
	struct entity *e;
	char *key;
	union sz_tag *val;
	e = NULL;
	sz_dict_foreach(key, val, root) {
		if (strcmp(key, "name") == 0) {
			list_foreach(traits, &ctx->entity_traits, list) {
				if (strcmp(val->str.data, traits->name) == 0) {
					e = traits->create_func(ctx);
					break;
				}
			}
			if (e == NULL) {
				log_error("bad entity name: \"%s\"", val->str.data);
			}
		} else if (e == NULL) {
			log_error("found \"%s\" before \"name\"", key);
		} else if (strcmp(key, "items") == 0) {
			memcpy(e->items->data, val->raw.data, val->raw.size);
		} else if (traits->load_func(e, key, val) != 0) {
			log_error("bad key: \"%s\"", key);
			traits->destroy_func(e);
			return NULL;
		}
	}
	return e;
}

static inline union sz_tag *entity_save(void *raw)
{
	struct entity *e = raw;
	union sz_tag *root;
	root = sz_dict();
	sz_dict_add(root, "name", sz_str(e->traits->name));
	e->traits->save_func(e, root);
	if (e->items != NULL)
		sz_dict_add(root, "items",
				sz_raw(e->items->data,
					e->items->elem_size * e->items->size));
	return root;
}


#endif
