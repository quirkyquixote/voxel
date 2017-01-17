
#ifndef VOXEL_BLOCK_ENTITY_H_
#define VOXEL_BLOCK_ENTITY_H_

#include "entity.h"

struct block_entity {
	struct entity entity;
	struct v3ll p;
};

static inline int block_entity_save(void *raw, union sz_tag *root)
{
	struct block_entity *e = raw;
	union sz_tag *tmp;
	tmp = sz_list();
	sz_list_add(tmp, sz_i64(e->p.x));
	sz_list_add(tmp, sz_i64(e->p.y));
	sz_list_add(tmp, sz_i64(e->p.z));
	sz_dict_add(root, "p", tmp);
	return 0;
}

static inline int block_entity_load(void *raw, char *key, union sz_tag *val)
{
	struct block_entity *e = raw;
	if (strcmp(key, "p") == 0) {
		typeof(e->p.x) *p = &e->p.x;
		union sz_tag *iter;
		sz_list_foreach(iter, val) {
			*p = iter->i64.data;
			++p;
		}
		return 0;
	}
	return -1;
}

#endif
