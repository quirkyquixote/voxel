/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_WORLD_H_
#define SRC_WORLD_H_

#include "chunk.h"

class World : public NonCopyable {
 public:
	static const long long CHUNK_NUM = 32;
	static const long long SHARD_NUM = CHUNK_NUM * CHUNK_NUM * Chunk::SHARD_NUM;
	static const long long W = Chunk::W * CHUNK_NUM;
	static const long long H = Chunk::H;
	static const long long D = Chunk::D * CHUNK_NUM;
	static const long long AREA = W * D;
	static const long long VOLUME = AREA * H;

	explicit World(Context *ctx);
	~World();

	void load(serializer::Tag *root);
	serializer::Tag *save();

	void set_block(const v3ll &p, int shape, int mat);
	void set_flags(const box3ll &bb, int flags);
	inline Chunk *get_chunk(const v2ll &p) { return chunks[p.x][p.y]; }
	inline Chunk *get_chunk_absolute(const v2ll &p) { return get_chunk(p & 0x1fLL); }
	inline Chunk *get_chunk_at_block(const v2ll &p) { return get_chunk_absolute(p >> 4LL); }
	inline void set_chunk(const v2ll &p, Chunk *c) { chunks[p.x][p.y] = c; }
	inline void set_chunk_absolute(const v2ll &p, Chunk *c) { set_chunk(p & 0x1fLL, c); }
	inline void set_chunk_at_block(const v2ll &p, Chunk *c) { set_chunk_absolute(p >> 4LL, c); }
	inline const v2ll &get_p() { return p; }
	inline void set_p(const v2ll &new_p) { p = new_p; }

	inline int get_mat(const v3ll &p) const
	{
		int x = p.x & 0x1ff;
		int z = p.z & 0x1ff;
		return chunks[x >> 4][z >> 4]->get_mat(v3ll(x & 0xf, p.y, z & 0xf));
	}

	inline int get_shape(const v3ll &p) const
	{
		int x = p.x & 0x1ff;
		int z = p.z & 0x1ff;
		return chunks[x >> 4][z >> 4]->get_shape(v3ll(x & 0xf, p.y, z & 0xf));
	}

	inline int get_light(const v3ll &p) const
	{
		int x = p.x & 0x1ff;
		int z = p.z & 0x1ff;
		return chunks[x >> 4][z >> 4]->get_light(v3ll(x & 0xf, p.y, z & 0xf));
	}

	inline Entity *get_data(const v3ll &p) const
	{
		int x = p.x & 0x1ff;
		int z = p.z & 0x1ff;
		return chunks[x >> 4][z >> 4]->get_data(v3ll(x & 0xf, p.y, z & 0xf));
	}

	inline void set_mat(const v3ll &p, int val)
	{
		int x = p.x & 0x1ff;
		int z = p.z & 0x1ff;
		chunks[x >> 4][z >> 4]->set_mat(v3ll(x & 0xf, p.y, z & 0xf), val);
	}

	inline void set_shape(const v3ll &p, int val)
	{
		int x = p.x & 0x1ff;
		int z = p.z & 0x1ff;
		chunks[x >> 4][z >> 4]->set_shape(v3ll(x & 0xf, p.y, z & 0xf), val);
	}

	inline void set_light(const v3ll &p, int val)
	{
		int x = p.x & 0x1ff;
		int z = p.z & 0x1ff;
		chunks[x >> 4][z >> 4]->set_light(v3ll(x & 0xf, p.y, z & 0xf), val);
	}

	inline void set_data(const v3ll &p, Entity *val)
	{
		int x = p.x & 0x1ff;
		int z = p.z & 0x1ff;
		chunks[x >> 4][z >> 4]->set_data(v3ll(x & 0xf, p.y, z & 0xf), val);
	}

 private:
	Context *ctx;
	v2ll p;
	Chunk *chunks[CHUNK_NUM][CHUNK_NUM];
};

#endif  // SRC_WORLD_H_


