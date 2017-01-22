/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_CHUNK_H_
#define SRC_CHUNK_H_

#include "shard.h"

#define SHARDS_PER_CHUNK 16

#define CHUNK_W SHARD_W
#define CHUNK_H (SHARD_H * SHARDS_PER_CHUNK)
#define CHUNK_D SHARD_D
#define CHUNK_AREA (CHUNK_W * CHUNK_D)
#define CHUNK_VOLUME (CHUNK_AREA * CHUNK_H)

enum {
	CHUNK_UNLOADED = 1 << 0,
	CHUNK_UNLIT = 1 << 1,
	CHUNK_UNRENDERED = 1 << 2,
};

class Context;

class Chunk : public NonCopyable {
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

#endif  // SRC_CHUNK_H_


