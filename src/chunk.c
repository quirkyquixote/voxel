
#include "chunk.h"

#include <string.h>
#include <stdio.h>

#include "sz.h"
#include "lighting.h"
#include "inventory.h"
#include "context.h"
#include "block_entity.h"

struct shard *shard(int id, int y)
{
	struct shard *s = calloc(1, sizeof(*s));
	s->id = id;
	s->y = y;
	return s;
}

void shard_destroy(struct shard *s)
{
	free(s);
}

int shard_load(struct shard *s, union sz_tag *root)
{
	char *key;
	union sz_tag *val;
	sz_dict_foreach(key, val, root) {
		if (strcmp(key, "y") == 0) {
			s->y = val->i64.data;
		} else if (strcmp(key, "mat") == 0) {
			memcpy(s->mat, val->raw.data, val->raw.size);
		} else if (strcmp(key, "shape") == 0) {
			memcpy(s->shape, val->raw.data, val->raw.size);
		} else if (strcmp(key, "light") == 0) {
			memcpy(s->light, val->raw.data, val->raw.size);
		} else {
			log_error("bad tag: %s", key);
		}
	}
	return 0;
}

int shard_save(struct shard *s, union sz_tag **root)
{
	*root = sz_dict();
	sz_dict_add(*root, "y", sz_i64(s->y));
	sz_dict_add(*root, "mat", sz_raw(s->mat, SHARD_VOLUME));
	sz_dict_add(*root, "shape", sz_raw(s->shape, SHARD_VOLUME));
	sz_dict_add(*root, "light", sz_raw(s->light, SHARD_VOLUME));
	return 0;
}

struct chunk *chunk(struct context *ctx, int id)
{
	int i;
	struct chunk *c = calloc(1, sizeof(*c));
	c->id = id;
	c->ctx = ctx;
	for (i = 0; i < SHARDS_PER_CHUNK; ++i)
		c->shards[i] = shard(id * SHARDS_PER_CHUNK + i, i);
	c->flags = CHUNK_UNLOADED;
	return c;
}

void chunk_destroy(struct chunk *c)
{
	int i;
	for (i = 0; i < SHARDS_PER_CHUNK; ++i)
		shard_destroy(c->shards[i]);
	free(c);
}

int chunk_load(struct chunk *c, union sz_tag *root)
{
	char *key;
	union sz_tag *val;
	sz_dict_foreach(key, val, root) {
		if (strcmp(key, "x") == 0) {
			c->x = val->i64.data;
		} else if (strcmp(key, "z") == 0) {
			c->z = val->i64.data;
		} else if (strcmp(key, "shards") == 0) {
			int i;
			union sz_tag *iter;
			i = 0;
			sz_list_foreach(iter, val) {
				if (shard_load(c->shards[i], iter) != 0)
					return -1;
				++i;
			}
		} else if (strcmp(key, "entities") == 0) {
			int i;
			union sz_tag *iter;
			i = 0;
			sz_list_foreach(iter, val) {
				struct block_entity *e;
				e = entity_load(c->ctx, iter);
				if (e == NULL)
					return -1;
				chunk_set_data(c, e->p, e);
			}
		} else {
			log_error("bad tag: %s", key);
		}
	}
	c->flags = CHUNK_UNRENDERED;
	return 0;
}

int chunk_save(struct chunk *c, union sz_tag **root)
{
	int i;
	union sz_tag *shards, *entities;
	struct v3ll p;
	struct box3ll bb = { 0, 0, 0, CHUNK_W, CHUNK_H, CHUNK_D };
	*root = sz_dict();
	sz_dict_add(*root, "x", sz_i64(c->x));
	sz_dict_add(*root, "z", sz_i64(c->z));
	shards = sz_list();
	for (i = 0; i < SHARDS_PER_CHUNK; ++i) {
		union sz_tag *tag;
		shard_save(c->shards[i], &tag);
		sz_list_add(shards, tag);
	}
	sz_dict_add(*root, "shards", shards);
	entities = sz_list();
	box3_foreach(p, bb) {
		struct block_entity *e;
		union sz_tag *tag;
		e = chunk_get_data(c, p);
		if (e != NULL) {
			tag = entity_save(e);
			sz_list_add(entities, tag);
		}
	}
	sz_dict_add(*root, "entities", entities);
	return 0;
}

struct world *world(struct context *ctx)
{
	struct world *w;
	int x, z, id;
	w = calloc(1, sizeof(*w));
	id = 0;
	w->ctx = ctx;
	for (x = 0; x < CHUNKS_PER_WORLD; ++x)
		for (z = 0; z < CHUNKS_PER_WORLD; ++z)
			w->chunks[x][z] = chunk(ctx, id++);
	return w;
}

void world_destroy(struct world *w)
{
	int x, z;
	for (x = 0; x < CHUNKS_PER_WORLD; ++x)
		for (z = 0; z < CHUNKS_PER_WORLD; ++z)
			chunk_destroy(w->chunks[x][z]);
}

int world_load(struct world *w, union sz_tag *root)
{
	char *key;
	union sz_tag *val;
	sz_dict_foreach(key, val, root) {
		if (strcmp(key, "x") == 0) {
			w->x = val->i64.data;
		} else if (strcmp(key, "z") == 0) {
			w->z = val->i64.data;
		} else {
			log_error("bad tag: %s", key);
			return -1;
		}
	}
	return 0;
}

int world_save(struct world *w, union sz_tag **root)
{
	*root = sz_dict();
	sz_dict_add(*root, "x", sz_i64(w->x));
	sz_dict_add(*root, "z", sz_i64(w->z));
	return 0;
}

void world_set(struct world *w, struct v3ll p, int shape, int mat)
{
	struct box3ll bb;
	world_set_shape(w, p, shape);
	world_set_mat(w, p, mat);
	if (block_traits[mat][shape].entity != NULL)
		world_set_data(w, p, entity(w->ctx, block_traits[mat][shape].entity));
	update_lighting(w, box3ll(p.x, p.y, p.z, p.x + 1, p.y + 1, p.z + 1), &bb);
	world_set_flags(w, bb, CHUNK_UNRENDERED);
}

void world_set_flags(struct world *w, struct box3ll bb, int flags)
{
	int x, z;
	bb.x0 = (bb.x0 >> 4) & 0xf;
	bb.z0 = (bb.z0 >> 4) & 0xf;
	bb.x1 = (bb.x1 >> 4) & 0xf;
	bb.z1 = (bb.z1 >> 4) & 0xf;
	for (x = bb.x0; x <= bb.x1; ++x) {
		for (z = bb.z0; z <= bb.z1; ++z) {
			w->chunks[x][z]->flags |= flags;
		}
	}
}
