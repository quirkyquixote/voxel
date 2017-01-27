/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_CHUNK_H_
#define SRC_CHUNK_H_

#include "shard.h"

class Context;

class Chunk : public NonCopyable {
 public:
	static const long long SHARD_NUM = 16;
	static const long long W = Shard::W;
	static const long long H = Shard::H * SHARD_NUM;
	static const long long D = Shard::D;
	static const long long AREA = W * D;
	static const long long VOLUME = AREA * H;

	enum {
		UNLOADED = 1 << 0,
		UNLIT = 1 << 1,
		UNRENDERED = 1 << 2,
	};

	Chunk(Context *ctx, const v2ll &p);
	~Chunk();

	void load(sz_Tag *root);
	sz_Tag *save();

	inline int get_id() const { return id; }
	inline void set_flags(int mask) { flags |= mask; }
	inline void unset_flags(int mask) { flags &= ~mask; }
	inline int get_flags() { return flags; }
	inline v2ll get_p() { return v2ll(x, z); }
	inline Shard *get_shard(int y) { return shards[y]; }

	inline int get_mat(const v3ll &p) const
	{
		assert(p.x >= 0 && p.x < W);
		assert(p.z >= 0 && p.z < D);
		int y = (p.y >> 4) & 0xf;
		return shards[y]->get_mat(v3ll(p.x, p.y & 0xf, p.z));
	}

	inline int get_shape(const v3ll &p) const
	{
		assert(p.x >= 0 && p.x < W);
		assert(p.z >= 0 && p.z < D);
		int y = (p.y >> 4) & 0xf;
		return shards[y]->get_shape(v3ll(p.x, p.y & 0xf, p.z));
	}

	inline int get_light(const v3ll &p) const
	{
		assert(p.x >= 0 && p.x < W);
		assert(p.z >= 0 && p.z < D);
		int y = (p.y >> 4) & 0xf;
		return shards[y]->get_light(v3ll(p.x, p.y & 0xf, p.z));
	}

	inline Entity *get_data(const v3ll &p) const
	{
		assert(p.x >= 0 && p.x < W);
		assert(p.z >= 0 && p.z < D);
		int y = (p.y >> 4) & 0xf;
		return shards[y]->get_data(v3ll(p.x, p.y & 0xf, p.z));
	}

	inline void set_mat(const v3ll &p, int val)
	{
		assert(p.x >= 0 && p.x < W);
		assert(p.z >= 0 && p.z < D);
		int y = (p.y >> 4) & 0xf;
		shards[y]->set_mat(v3ll(p.x, p.y & 0xf, p.z), val);
	}

	inline void set_shape(const v3ll &p, int val)
	{
		assert(p.x >= 0 && p.x < W);
		assert(p.z >= 0 && p.z < D);
		int y = (p.y >> 4) & 0xf;
		shards[y]->set_shape(v3ll(p.x, p.y & 0xf, p.z), val);
	}

	inline void set_light(const v3ll &p, int val)
	{
		assert(p.x >= 0 && p.x < W);
		assert(p.z >= 0 && p.z < D);
		int y = (p.y >> 4) & 0xf;
		shards[y]->set_light(v3ll(p.x, p.y & 0xf, p.z), val);
	}

	inline void set_data(const v3ll &p, Entity *val)
	{
		assert(p.x >= 0 && p.x < W);
		assert(p.z >= 0 && p.z < D);
		int y = (p.y >> 4) & 0xf;
		shards[y]->set_data(v3ll(p.x, p.y & 0xf, p.z), val);
	}

 private:
	int id;
	int flags;
	int priority;
	Context *ctx;
	int64_t x, z;
	Shard *shards[SHARD_NUM];
};

#endif  // SRC_CHUNK_H_


