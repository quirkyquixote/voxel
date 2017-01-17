
#ifndef VOXEL_ROAMING_ENTITY_H_
#define VOXEL_ROAMING_ENTITY_H_

#include "entity.h"

struct roaming_entity {
	struct entity entity;
	struct list entities;
	struct body *body;
	int die;
};

static inline int roaming_entity_save(void *raw, union sz_tag *root)
{
	struct roaming_entity *e = raw;
	union sz_tag *tmp;
	tmp = sz_list();
	sz_list_add(tmp, sz_i64(e->body->p.x));
	sz_list_add(tmp, sz_i64(e->body->p.y));
	sz_list_add(tmp, sz_i64(e->body->p.z));
	sz_dict_add(root, "p", tmp);
	tmp = sz_list();
	sz_list_add(tmp, sz_i64(e->body->r.x));
	sz_list_add(tmp, sz_i64(e->body->r.y));
	sz_list_add(tmp, sz_i64(e->body->r.z));
	sz_dict_add(root, "r", tmp);
	tmp = sz_list();
	sz_list_add(tmp, sz_i64(e->body->v.x));
	sz_list_add(tmp, sz_i64(e->body->v.y));
	sz_list_add(tmp, sz_i64(e->body->v.z));
	sz_dict_add(root, "v", tmp);
	return 0;
}

static inline int roaming_entity_load(void *raw, char *key, union sz_tag *val)
{
	struct roaming_entity *e = raw;
	if (strcmp(key, "p")) {
		typeof(e->body->p.x) *p = &e->body->p.x;
		int i = 0;
		union sz_tag *iter;
		sz_list_foreach(iter, val)
			p[i++] = val->i64.data;
		return 0;
	}
	if (strcmp(key, "r")) {
		typeof(e->body->r.x) *r = &e->body->r.x;
		int i = 0;
		union sz_tag *iter;
		sz_list_foreach(iter, val)
			r[i++] = val->i64.data;
		return 0;
	}
	if (strcmp(key, "v")) {
		typeof(e->body->v.x) *v = &e->body->v.x;
		int i = 0;
		union sz_tag *iter;
		sz_list_foreach(iter, val)
			v[i++] = val->i64.data;
		return 0;
	}
	return -1;
}

#endif
