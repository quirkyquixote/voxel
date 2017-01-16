

#ifndef VOXEL_SHARD_H_
#define VOXEL_SHARD_H_

#include <stdint.h>
#include <assert.h>

#include "sz.h"
#include "v3.h"
#include "box3.h"
#include "types.h"

#define SHARD_W 16
#define SHARD_H 16
#define SHARD_D 16
#define SHARD_AREA (SHARD_W * SHARD_D)
#define SHARD_VOLUME (SHARD_AREA * SHARD_H)

#define SHARDS_PER_CHUNK 16

#define CHUNK_W SHARD_W
#define CHUNK_H (SHARD_H * SHARDS_PER_CHUNK)
#define CHUNK_D SHARD_D
#define CHUNK_AREA (CHUNK_W * CHUNK_D)
#define CHUNK_VOLUME (CHUNK_AREA * CHUNK_H)

#define CHUNKS_PER_WORLD 16

#define WORLD_W (CHUNK_W * CHUNKS_PER_WORLD)
#define WORLD_H CHUNK_H
#define WORLD_D (CHUNK_D * CHUNKS_PER_WORLD)
#define WORLD_AREA (WORLD_W * WORLD_D)
#define WORLD_VOLUME (WORLD_AREA * WORLD_H)

#define SHARDS_PER_WORLD (CHUNKS_PER_WORLD * CHUNKS_PER_WORLD * SHARDS_PER_CHUNK)

enum {
	CHUNK_UNLOADED = 1 << 0,
	CHUNK_UNLIT = 1 << 1,
	CHUNK_UNRENDERED = 1 << 2,
};

struct shard {
	int id;
	int8_t y;
	uint8_t mat[SHARD_W][SHARD_H][SHARD_D];
	uint8_t shape[SHARD_W][SHARD_H][SHARD_D];
	uint8_t light[SHARD_W][SHARD_H][SHARD_D];
	void *data[SHARD_W][SHARD_H][SHARD_D];
};

struct chunk {
	int id;
	int flags;
	int priority;
	int64_t x, z;
	struct shard *shards[SHARDS_PER_CHUNK];
};

struct world {
	uint64_t x, z;
	struct chunk *chunks[CHUNKS_PER_WORLD][CHUNKS_PER_WORLD];
};

struct shard *shard(int id, int y);
void shard_destroy(struct shard *s);
int shard_load(struct shard *s, union sz_tag *root);
int shard_save(struct shard *s, union sz_tag **root);

static inline int shard_get_mat(struct shard *s, struct v3ll p)
{
	assert(p.x >= 0 && p.x < SHARD_W);
	assert(p.y >= 0 && p.y < SHARD_H);
	assert(p.z >= 0 && p.z < SHARD_D);
	return s->mat[p.x][p.y][p.z];
}

static inline int shard_get_shape(struct shard *s, struct v3ll p)
{
	assert(p.x >= 0 && p.x < SHARD_W);
	assert(p.y >= 0 && p.y < SHARD_H);
	assert(p.z >= 0 && p.z < SHARD_D);
	return s->shape[p.x][p.y][p.z];
}

static inline int shard_get_light(struct shard *s, struct v3ll p)
{
	assert(p.x >= 0 && p.x < SHARD_W);
	assert(p.y >= 0 && p.y < SHARD_H);
	assert(p.z >= 0 && p.z < SHARD_D);
	return s->light[p.x][p.y][p.z];
}

static inline void *shard_get_data(struct shard *s, struct v3ll p)
{
	assert(p.x >= 0 && p.x < SHARD_W);
	assert(p.y >= 0 && p.y < SHARD_H);
	assert(p.z >= 0 && p.z < SHARD_D);
	return s->data[p.x][p.y][p.z];
}

static inline int shard_set_mat(struct shard *s, struct v3ll p, int val)
{
	assert(p.x >= 0 && p.x < SHARD_W);
	assert(p.y >= 0 && p.y < SHARD_H);
	assert(p.z >= 0 && p.z < SHARD_D);
	s->mat[p.x][p.y][p.z] = val;
}

static inline int shard_set_shape(struct shard *s, struct v3ll p, int val)
{
	assert(p.x >= 0 && p.x < SHARD_W);
	assert(p.y >= 0 && p.y < SHARD_H);
	assert(p.z >= 0 && p.z < SHARD_D);
	s->shape[p.x][p.y][p.z] = val;
}

static inline int shard_set_light(struct shard *s, struct v3ll p, int val)
{
	assert(p.x >= 0 && p.x < SHARD_W);
	assert(p.y >= 0 && p.y < SHARD_H);
	assert(p.z >= 0 && p.z < SHARD_D);
	s->light[p.x][p.y][p.z] = val;
}

static inline void *shard_set_data(struct shard *s, struct v3ll p, void *val)
{
	assert(p.x >= 0 && p.x < SHARD_W);
	assert(p.y >= 0 && p.y < SHARD_H);
	assert(p.z >= 0 && p.z < SHARD_D);
	s->data[p.x][p.y][p.z] = val;
}

struct chunk *chunk(int id);
void chunk_destroy(struct chunk *c);
int chunk_load(struct chunk *c, union sz_tag *root);
int chunk_save(struct chunk *c, union sz_tag **root);

static inline int chunk_get_mat(struct chunk *c, struct v3ll p)
{
	assert(p.x >= 0 && p.x < CHUNK_W);
	assert(p.z >= 0 && p.z < CHUNK_D);
	int y = (p.y >> 4) & 0xf;
	p.y &= 0xf;
	return shard_get_mat(c->shards[y], p);
}

static inline int chunk_get_shape(struct chunk *c, struct v3ll p)
{
	assert(p.x >= 0 && p.x < CHUNK_W);
	assert(p.z >= 0 && p.z < CHUNK_D);
	int y = (p.y >> 4) & 0xf;
	p.y &= 0xf;
	return shard_get_shape(c->shards[y], p);
}

static inline int chunk_get_light(struct chunk *c, struct v3ll p)
{
	assert(p.x >= 0 && p.x < CHUNK_W);
	assert(p.z >= 0 && p.z < CHUNK_D);
	int y = (p.y >> 4) & 0xf;
	p.y &= 0xf;
	return shard_get_light(c->shards[y], p);
}

static inline void *chunk_get_data(struct chunk *c, struct v3ll p)
{
	assert(p.x >= 0 && p.x < CHUNK_W);
	assert(p.z >= 0 && p.z < CHUNK_D);
	int y = (p.y >> 4) & 0xf;
	p.y &= 0xf;
	return shard_get_data(c->shards[y], p);
}

static inline void chunk_set_mat(struct chunk *c, struct v3ll p, int val)
{
	assert(p.x >= 0 && p.x < CHUNK_W);
	assert(p.z >= 0 && p.z < CHUNK_D);
	int y = (p.y >> 4) & 0xf;
	p.y &= 0xf;
	shard_set_mat(c->shards[y], p, val);
}

static inline void chunk_set_shape(struct chunk *c, struct v3ll p, int val)
{
	assert(p.x >= 0 && p.x < CHUNK_W);
	assert(p.z >= 0 && p.z < CHUNK_D);
	int y = (p.y >> 4) & 0xf;
	p.y &= 0xf;
	shard_set_shape(c->shards[y], p, val);
}

static inline void chunk_set_light(struct chunk *c, struct v3ll p, int val)
{
	assert(p.x >= 0 && p.x < CHUNK_W);
	assert(p.z >= 0 && p.z < CHUNK_D);
	int y = (p.y >> 4) & 0xf;
	p.y &= 0xf;
	shard_set_light(c->shards[y], p, val);
}

static inline void chunk_set_data(struct chunk *c, struct v3ll p, void *val)
{
	assert(p.x >= 0 && p.x < CHUNK_W);
	assert(p.z >= 0 && p.z < CHUNK_D);
	int y = (p.y >> 4) & 0xf;
	p.y &= 0xf;
	shard_set_data(c->shards[y], p, val);
}

struct world *world(void);
void world_destroy(struct world *w);
int world_load(struct world *w, union sz_tag *root);
int world_save(struct world *w, union sz_tag **root);
void world_set(struct world *w, struct v3ll p, int shape, int mat);
void world_set_flags(struct world *w, struct box3ll bb, int flags);

static inline int world_get_mat(struct world *w, struct v3ll p)
{
	int x = (p.x >> 4) & 0xf;
	int z = (p.z >> 4) & 0xf;
	p.x &= 0xf;
	p.z &= 0xf;
	return chunk_get_mat(w->chunks[x][z], p);
}

static inline int world_get_shape(struct world *w, struct v3ll p)
{
	int x = (p.x >> 4) & 0xf;
	int z = (p.z >> 4) & 0xf;
	p.x &= 0xf;
	p.z &= 0xf;
	return chunk_get_shape(w->chunks[x][z], p);
}

static inline int world_get_light(struct world *w, struct v3ll p)
{
	int x = (p.x >> 4) & 0xf;
	int z = (p.z >> 4) & 0xf;
	p.x &= 0xf;
	p.z &= 0xf;
	return chunk_get_light(w->chunks[x][z], p);
}

static inline void *world_get_data(struct world *w, struct v3ll p)
{
	int x = (p.x >> 4) & 0xf;
	int z = (p.z >> 4) & 0xf;
	p.x &= 0xf;
	p.z &= 0xf;
	return chunk_get_data(w->chunks[x][z], p);
}

static inline void world_set_mat(struct world *w, struct v3ll p, int val)
{
	int x = (p.x >> 4) & 0xf;
	int z = (p.z >> 4) & 0xf;
	p.x &= 0xf;
	p.z &= 0xf;
	chunk_set_mat(w->chunks[x][z], p, val);
}

static inline void world_set_shape(struct world *w, struct v3ll p, int val)
{
	int x = (p.x >> 4) & 0xf;
	int z = (p.z >> 4) & 0xf;
	p.x &= 0xf;
	p.z &= 0xf;
	chunk_set_shape(w->chunks[x][z], p, val);
}

static inline void world_set_light(struct world *w, struct v3ll p, int val)
{
	int x = (p.x >> 4) & 0xf;
	int z = (p.z >> 4) & 0xf;
	p.x &= 0xf;
	p.z &= 0xf;
	chunk_set_light(w->chunks[x][z], p, val);
}

static inline void world_set_data(struct world *w, struct v3ll p, void *val)
{
	int x = (p.x >> 4) & 0xf;
	int z = (p.z >> 4) & 0xf;
	p.x &= 0xf;
	p.z &= 0xf;
	chunk_set_data(w->chunks[x][z], p, val);
}

#endif


