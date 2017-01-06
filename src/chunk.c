
#include "chunk.h"

#include <string.h>
#include <stdio.h>

#include "sz.h"

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
			fprintf(stderr, "%s: bad tag %s\n", __func__, key);
			return -1;
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

struct chunk *chunk(int id)
{
	int i;
	struct chunk *c = calloc(1, sizeof(*c));
	c->id = id;
	for (i = 0; i < SHARDS_PER_CHUNK; ++i)
		c->shards[i] = shard(id * SHARDS_PER_CHUNK + i, i);
	return c;
}

void chunk_destroy(struct chunk *c)
{
	int i;
	struct cell_entity *it, *next;
	for (i = 0; i < SHARDS_PER_CHUNK; ++i)
		shard_destroy(c->shards[i]);
	for (it = c->entities; it != NULL; it = next) {
		next = it->next;
		free(it);
	}
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
		} else {
			fprintf(stderr, "%s: bad tag %s\n", __func__, key);
			return -1;
		}
	}
	return 0;
}

int chunk_save(struct chunk *c, union sz_tag **root)
{
	int i;
	union sz_tag *shards;
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
	return 0;
}

struct world *world(void)
{
	struct world *w;
	int x, z, id;
	w = calloc(1, sizeof(*w));
	id = 0;
	for (x = 0; x < CHUNKS_PER_WORLD; ++x)
		for (z = 0; z < CHUNKS_PER_WORLD; ++z)
			w->chunks[x][z] = chunk(id++);
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
			fprintf(stderr, "%s: bad tag %s\n", __func__, key);
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

void world_set(struct world *w, struct v3ll p, int shape, int mat, void *data)
{
	int x;
	int z;
	WORLD_AT(w, shape, p.x, p.y, p.z) = shape;
	WORLD_AT(w, mat, p.x, p.y, p.z) = mat;
	WORLD_AT(w, data, p.x, p.y, p.z) = data;
	x = (p.x >> 4) & 0xf;
	z = (p.z >> 4) & 0xf;
	w->chunks[x][z]->up_to_date = 0;
	if ((p.x & 0xf) == 0)
		w->chunks[x - 1][z]->up_to_date = 0;
	if ((p.z & 0xf) == 0)
		w->chunks[x][z - 1]->up_to_date = 0;
	if ((p.x & 0xf) == 0xf)
		w->chunks[x + 1][z]->up_to_date = 0;
	if ((p.z & 0xf) == 0xf)
		w->chunks[x][z + 1]->up_to_date = 0;
}
