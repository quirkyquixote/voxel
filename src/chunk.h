

#ifndef VX_SHARD_H_
#define VX_SHARD_H_

#include <stdint.h>

#include "sz.h"
#include "v3.h"
#include "aab3.h"
#include "inventory.h"

#define SHARD_W 16
#define SHARD_H 16
#define SHARD_D 16
#define SHARD_AREA (SHARD_W * SHARD_D)
#define SHARD_VOLUME (SHARD_AREA * SHARD_H)
#define SHARD_AT(s,t,x,y,z) ((s)->t[x][y][z])

#define SHARDS_PER_CHUNK 16

#define CHUNK_W SHARD_W
#define CHUNK_H (SHARD_H * SHARDS_PER_CHUNK)
#define CHUNK_D SHARD_D
#define CHUNK_AREA (CHUNK_W * CHUNK_D)
#define CHUNK_VOLUME (CHUNK_AREA * CHUNK_H)
#define CHUNK_AT(c,t,x,y,z) (SHARD_AT((c)->shards[((y) >> 4) & 0xf],t,x,(y) & 0xf,z))

#define CHUNKS_PER_WORLD 16

#define WORLD_W (CHUNK_W * CHUNKS_PER_WORLD)
#define WORLD_H CHUNK_H
#define WORLD_D (CHUNK_D * CHUNKS_PER_WORLD)
#define WORLD_AREA (WORLD_W * WORLD_D)
#define WORLD_VOLUME (WORLD_AREA * WORLD_H)
#define WORLD_AT(w,t,x,y,z) (CHUNK_AT(w->chunks[((x) >> 4) & 0xf][((z) >> 4) & 0xf], t, (x) & 0xf, y, (z) & 0xf))

#define SHARDS_PER_WORLD (CHUNKS_PER_WORLD * CHUNKS_PER_WORLD * SHARDS_PER_CHUNK)

enum {
	SHAPE_NONE,
	SHAPE_BLOCK_DN,
	SHAPE_BLOCK_UP,
	SHAPE_BLOCK_LF,
	SHAPE_BLOCK_RT,
	SHAPE_BLOCK_BK,
	SHAPE_BLOCK_FT,
	SHAPE_SLAB_DN,
	SHAPE_SLAB_UP,
	SHAPE_SLAB_LF,
	SHAPE_SLAB_RT,
	SHAPE_SLAB_BK,
	SHAPE_SLAB_FT,
	SHAPE_PANE_X,
	SHAPE_PANE_Y,
	SHAPE_PANE_Z,
	SHAPE_STAIRS_DF,
	SHAPE_STAIRS_DL,
	SHAPE_STAIRS_DB,
	SHAPE_STAIRS_DR,
	SHAPE_STAIRS_UF,
	SHAPE_STAIRS_UL,
	SHAPE_STAIRS_UB,
	SHAPE_STAIRS_UR,
	SHAPE_WORKBENCH,
	SHAPE_CRATE,
	SHAPE_FLUID1,
	SHAPE_FLUID2,
	SHAPE_FLUID3,
	SHAPE_FLUID4,
	SHAPE_FLUID5,
	SHAPE_FLUID6,
	SHAPE_FLUID7,
	SHAPE_FLUID8,
	SHAPE_FLUID9,
	SHAPE_FLUID10,
	SHAPE_FLUID11,
	SHAPE_FLUID12,
	SHAPE_FLUID13,
	SHAPE_FLUID14,
	SHAPE_FLUID15,
	SHAPE_FLUID16,
	SHAPE_COUNT,
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
	int up_to_date;
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

struct chunk *chunk(int id);
void chunk_destroy(struct chunk *c);
int chunk_load(struct chunk *c, union sz_tag *root);
int chunk_save(struct chunk *c, union sz_tag **root);

struct world *world(void);
void world_destroy(struct world *w);
int world_load(struct world *w, union sz_tag *root);
int world_save(struct world *w, union sz_tag **root);
void world_set(struct world *w, struct v3ll p, int shape, int mat, void *data);

#endif


