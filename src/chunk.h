
#ifndef VOXEL_SHARD_H_
#define VOXEL_SHARD_H_

#include <stdint.h>
#include <assert.h>

#include "sz.h"
#include "v2.h"
#include "v3.h"
#include "box3.h"
#include "types.h"
#include "entity.h"

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

class Context;

class Shard {
public:
	Shard(int id, int y);
	~Shard();

	void load(sz_Tag *root);
	sz_Tag *save();

	inline int get_id() const { return id; }
	inline int get_y() const { return y; }

	inline int get_mat(const v3ll &p) const
	{
		assert(p.x >= 0 && p.x < SHARD_W);
		assert(p.y >= 0 && p.y < SHARD_H);
		assert(p.z >= 0 && p.z < SHARD_D);
		return mat[p.x][p.y][p.z];
	}

	inline int get_shape(const v3ll &p) const
	{
		assert(p.x >= 0 && p.x < SHARD_W);
		assert(p.y >= 0 && p.y < SHARD_H);
		assert(p.z >= 0 && p.z < SHARD_D);
		return shape[p.x][p.y][p.z];
	}

	inline int get_light(const v3ll &p) const
	{
		assert(p.x >= 0 && p.x < SHARD_W);
		assert(p.y >= 0 && p.y < SHARD_H);
		assert(p.z >= 0 && p.z < SHARD_D);
		return light[p.x][p.y][p.z];
	}

	inline Entity * get_data(const v3ll &p) const
	{
		assert(p.x >= 0 && p.x < SHARD_W);
		assert(p.y >= 0 && p.y < SHARD_H);
		assert(p.z >= 0 && p.z < SHARD_D);
		return data[p.x][p.y][p.z];
	}

	inline void set_mat(const v3ll &p, int val)
	{
		assert(p.x >= 0 && p.x < SHARD_W);
		assert(p.y >= 0 && p.y < SHARD_H);
		assert(p.z >= 0 && p.z < SHARD_D);
		mat[p.x][p.y][p.z] = val;
	}

	inline void set_shape(const v3ll &p, int val)
	{
		assert(p.x >= 0 && p.x < SHARD_W);
		assert(p.y >= 0 && p.y < SHARD_H);
		assert(p.z >= 0 && p.z < SHARD_D);
		shape[p.x][p.y][p.z] = val;
	}

	inline void set_light(const v3ll &p, int val)
	{
		assert(p.x >= 0 && p.x < SHARD_W);
		assert(p.y >= 0 && p.y < SHARD_H);
		assert(p.z >= 0 && p.z < SHARD_D);
		light[p.x][p.y][p.z] = val;
	}

	inline void set_data(const v3ll &p, Entity *val)
	{
		assert(p.x >= 0 && p.x < SHARD_W);
		assert(p.y >= 0 && p.y < SHARD_H);
		assert(p.z >= 0 && p.z < SHARD_D);
		data[p.x][p.y][p.z] = val;
	}

private:
	int id;
	int8_t y;
	uint8_t mat[SHARD_W][SHARD_H][SHARD_D];
	uint8_t shape[SHARD_W][SHARD_H][SHARD_D];
	uint8_t light[SHARD_W][SHARD_H][SHARD_D];
	Entity *data[SHARD_W][SHARD_H][SHARD_D];
};

class Chunk {
public:
	Chunk(Context *ctx, int id);
	~Chunk();

	void load(sz_Tag *root);
	sz_Tag *save();

	inline int get_id() const { return id; }
	inline void set_flags(int mask) { flags |= mask; }
	inline void unset_flags(int mask) { flags &= ~mask; }
	inline int get_flags() { return flags; }
	inline void set_p(const v2ll &p) { x = p.x; z = p.y; }
	inline v2ll get_p() { return v2ll(x, z); }
	inline Shard *get_shard(int y) { return shards[y]; }

	inline int get_mat(const v3ll &p) const
	{
		assert(p.x >= 0 && p.x < CHUNK_W);
		assert(p.z >= 0 && p.z < CHUNK_D);
		int y = (p.y >> 4) & 0xf;
		return shards[y]->get_mat(v3ll(p.x, p.y & 0xf, p.z));
	}

	inline int get_shape(const v3ll &p) const
	{
		assert(p.x >= 0 && p.x < CHUNK_W);
		assert(p.z >= 0 && p.z < CHUNK_D);
		int y = (p.y >> 4) & 0xf;
		return shards[y]->get_shape(v3ll(p.x, p.y & 0xf, p.z));
	}

	inline int get_light(const v3ll &p) const
	{
		assert(p.x >= 0 && p.x < CHUNK_W);
		assert(p.z >= 0 && p.z < CHUNK_D);
		int y = (p.y >> 4) & 0xf;
		return shards[y]->get_light(v3ll(p.x, p.y & 0xf, p.z));
	}

	inline Entity *get_data(const v3ll &p) const
	{
		assert(p.x >= 0 && p.x < CHUNK_W);
		assert(p.z >= 0 && p.z < CHUNK_D);
		int y = (p.y >> 4) & 0xf;
		return shards[y]->get_data(v3ll(p.x, p.y & 0xf, p.z));
	}

	inline void set_mat(const v3ll &p, int val)
	{
		assert(p.x >= 0 && p.x < CHUNK_W);
		assert(p.z >= 0 && p.z < CHUNK_D);
		int y = (p.y >> 4) & 0xf;
		shards[y]->set_mat(v3ll(p.x, p.y & 0xf, p.z), val);
	}

	inline void set_shape(const v3ll &p, int val)
	{
		assert(p.x >= 0 && p.x < CHUNK_W);
		assert(p.z >= 0 && p.z < CHUNK_D);
		int y = (p.y >> 4) & 0xf;
		shards[y]->set_shape(v3ll(p.x, p.y & 0xf, p.z), val);
	}

	inline void set_light(const v3ll &p, int val)
	{
		assert(p.x >= 0 && p.x < CHUNK_W);
		assert(p.z >= 0 && p.z < CHUNK_D);
		int y = (p.y >> 4) & 0xf;
		shards[y]->set_light(v3ll(p.x, p.y & 0xf, p.z), val);
	}

	inline void set_data(const v3ll &p, Entity *val)
	{
		assert(p.x >= 0 && p.x < CHUNK_W);
		assert(p.z >= 0 && p.z < CHUNK_D);
		int y = (p.y >> 4) & 0xf;
		shards[y]->set_data(v3ll(p.x, p.y & 0xf, p.z), val);
	}

private:
	int id;
	int flags;
	int priority;
	Context *ctx;
	int64_t x, z;
	Shard *shards[SHARDS_PER_CHUNK];
};

class World {
public:
	World(Context *ctx);
	~World();

	void load(sz_Tag *root);
	sz_Tag *save();

	void set_block(const v3ll &p, int shape, int mat);
	void set_flags(const box3ll &bb, int flags);
	inline Chunk *get_chunk(const v2ll &p) { return chunks[p.x][p.y]; }
	inline const v2ll &get_p() { return p; }

	inline int get_mat(const v3ll &p) const
	{
		int x = (p.x >> 4) & 0xf;
		int z = (p.z >> 4) & 0xf;
		return chunks[x][z]->get_mat(v3ll(p.x & 0xf, p.y, p.z & 0xf));
	}

	inline int get_shape(const v3ll &p) const
	{
		int x = (p.x >> 4) & 0xf;
		int z = (p.z >> 4) & 0xf;
		return chunks[x][z]->get_shape(v3ll(p.x & 0xf, p.y, p.z & 0xf));
	}

	inline int get_light(const v3ll &p) const
	{
		int x = (p.x >> 4) & 0xf;
		int z = (p.z >> 4) & 0xf;
		return chunks[x][z]->get_light(v3ll(p.x & 0xf, p.y, p.z & 0xf));
	}

	inline Entity *get_data(const v3ll &p) const
	{
		int x = (p.x >> 4) & 0xf;
		int z = (p.z >> 4) & 0xf;
		return chunks[x][z]->get_data(v3ll(p.x & 0xf, p.y, p.z & 0xf));
	}

	inline void set_mat(const v3ll &p, int val)
	{
		int x = (p.x >> 4) & 0xf;
		int z = (p.z >> 4) & 0xf;
		chunks[x][z]->set_mat(v3ll(p.x & 0xf, p.y, p.z & 0xf), val);
	}

	inline void set_shape(const v3ll &p, int val)
	{
		int x = (p.x >> 4) & 0xf;
		int z = (p.z >> 4) & 0xf;
		chunks[x][z]->set_shape(v3ll(p.x & 0xf, p.y, p.z & 0xf), val);
	}

	inline void set_light(const v3ll &p, int val)
	{
		int x = (p.x >> 4) & 0xf;
		int z = (p.z >> 4) & 0xf;
		chunks[x][z]->set_light(v3ll(p.x & 0xf, p.y, p.z & 0xf), val);
	}

	inline void set_data(const v3ll &p, Entity *val)
	{
		int x = (p.x >> 4) & 0xf;
		int z = (p.z >> 4) & 0xf;
		chunks[x][z]->set_data(v3ll(p.x & 0xf, p.y, p.z & 0xf), val);
	}

private:
	Context *ctx;
	v2ll p;
	Chunk *chunks[CHUNKS_PER_WORLD][CHUNKS_PER_WORLD];
};

#endif


