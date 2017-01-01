

#ifndef VX_SHARD_H_
#define VX_SHARD_H_

#include <stdint.h>

#include "sz.h"

#define SHARD_W 16
#define SHARD_H 16
#define SHARD_D 16
#define SHARD_AREA (SHARD_W * SHARD_D)
#define SHARD_VOLUME (SHARD_AREA * SHARD_H)
#define SHARD_AT(s,t,x,y,z) (s->t[x][y][z])

#define SHARDS_PER_CHUNK 16

#define CHUNK_W SHARD_W
#define CHUNK_H (SHARD_H * SHARDS_PER_CHUNK)
#define CHUNK_D SHARD_D
#define CHUNK_AREA (CHUNK_W * CHUNK_D)
#define CHUNK_VOLUME (CHUNK_AREA * CHUNK_H)
#define CHUNK_AT(c,t,x,y,z) (SHARD_AT(c->shards[y / SHARD_H],t,x,y % SHARD_H,z))

#define CHUNKS_PER_WORLD 16

#define WORLD_W (CHUNK_W * CHUNKS_PER_WORLD)
#define WORLD_H CHUNK_H
#define WORLD_D (CHUNK_D * CHUNKS_PER_WORLD)
#define WORLD_AREA (WORLD_W * WORLD_D)
#define WORLD_VOLUME (WORLD_AREA * WORLD_H)
#define WORLD_AT(w,t,x,y,z) (CHUNK_AT(w->chunks[x / CHUNKS_PER_WORLD][z / CHUNKS_PER_WORLD], t, x % CHUNKS_PER_WORLD, y, z % CHUNKS_PER_WORLD))

struct shard {
	int8_t y;
	int8_t mat[SHARD_W][SHARD_H][SHARD_D];
	int8_t shape[SHARD_W][SHARD_H][SHARD_D];
	int8_t light1[SHARD_W][SHARD_H][SHARD_D];
	int8_t light2[SHARD_W][SHARD_H][SHARD_D];
};

struct cell_entity {
	struct cell_entity *next;
};

struct chunk {
	int64_t x, z;
	struct shard *shards[SHARDS_PER_CHUNK];
	struct cell_entity *entities;
};

struct world {
	struct chunk *chunks[CHUNKS_PER_WORLD][CHUNKS_PER_WORLD];
};

struct shard *shard_new(void);
void shard_destroy(struct shard *s);
int shard_load(struct shard *s, union sz_tag *root);
int shard_save(struct shard *s, union sz_tag **root);

struct chunk *chunk_new(void);
void chunk_destroy(struct chunk *c);
int chunk_load(struct chunk *c, union sz_tag *root);
int chunk_save(struct chunk *c, union sz_tag **root);

struct world *world_new(void);
void world_destroy(struct world *w);
int world_load(struct world *w);
int world_save(struct world *w);

#endif


