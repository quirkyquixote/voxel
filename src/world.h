/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_WORLD_H_
#define SRC_WORLD_H_

#include "chunk.h"

template<class T> T div_floor(T x, T y)
{
    const T quot = x / y;
    const T rem  = x % y;
    if (rem && (x < 0) != (y < 0))
        return quot - 1;
    return quot;
}

template<class T> T mod_floor(T x, T y)
{
    const T quot = x / y;
    const T rem  = x % y;
    if (rem && (x < 0) != (y < 0))
        return rem + y;
    return rem;
}

class World : public NonCopyable {
 public:
	static const size_t CHUNK_NUM = 16;
	static const size_t SHARD_NUM = CHUNK_NUM * CHUNK_NUM * Chunk::SHARD_NUM;
	static const size_t W = Chunk::W * CHUNK_NUM;
	static const size_t H = Chunk::H;
	static const size_t D = Chunk::D * CHUNK_NUM;
	static const size_t AREA = W * D;
	static const size_t VOLUME = AREA * H;

	explicit World(Context *ctx);
	~World();

	void load(sz_Tag *root);
	sz_Tag *save();

	void set_block(const v3ll &p, int shape, int mat);
	void set_flags(const box3ll &bb, int flags);
	inline Chunk *get_chunk(const v2ll &p) { return chunks[p.x][p.y]; }
	inline void set_chunk(const v2ll &p, Chunk *c) { chunks[p.x][p.y] = c; }
	inline const v2ll &get_p() { return p; }
	inline void set_p(const v2ll &new_p) { p = new_p; }

	inline int get_mat(const v3ll &p) const
	{
		int x = p.x & 0xff;
		int z = p.z & 0xff;
		return chunks[x >> 4][z >> 4]->get_mat(v3ll(x & 0xf, p.y, z & 0xf));
	}

	inline int get_shape(const v3ll &p) const
	{
		int x = div_floor(p.x, 16LL) & 0xf;
		int z = div_floor(p.z, 16LL) & 0xf;
		return chunks[x][z]->get_shape(v3ll(p.x & 0xf, p.y, p.z & 0xf));
	}

	inline int get_light(const v3ll &p) const
	{
		int x = div_floor(p.x, 16LL) & 0xf;
		int z = div_floor(p.z, 16LL) & 0xf;
		return chunks[x][z]->get_light(v3ll(p.x & 0xf, p.y, p.z & 0xf));
	}

	inline Entity *get_data(const v3ll &p) const
	{
		int x = div_floor(p.x, 16LL) & 0xf;
		int z = div_floor(p.z, 16LL) & 0xf;
		return chunks[x][z]->get_data(v3ll(p.x & 0xf, p.y, p.z & 0xf));
	}

	inline void set_mat(const v3ll &p, int val)
	{
		int x = div_floor(p.x, 16LL) & 0xf;
		int z = div_floor(p.z, 16LL) & 0xf;
		chunks[x][z]->set_mat(v3ll(p.x & 0xf, p.y, p.z & 0xf), val);
	}

	inline void set_shape(const v3ll &p, int val)
	{
		int x = div_floor(p.x, 16LL) & 0xf;
		int z = div_floor(p.z, 16LL) & 0xf;
		chunks[x][z]->set_shape(v3ll(p.x & 0xf, p.y, p.z & 0xf), val);
	}

	inline void set_light(const v3ll &p, int val)
	{
		int x = div_floor(p.x, 16LL) & 0xf;
		int z = div_floor(p.z, 16LL) & 0xf;
		chunks[x][z]->set_light(v3ll(p.x & 0xf, p.y, p.z & 0xf), val);
	}

	inline void set_data(const v3ll &p, Entity *val)
	{
		int x = div_floor(p.x, 16LL) & 0xf;
		int z = div_floor(p.z, 16LL) & 0xf;
		chunks[x][z]->set_data(v3ll(p.x & 0xf, p.y, p.z & 0xf), val);
	}

 private:
	Context *ctx;
	v2ll p;
	Chunk *chunks[CHUNK_NUM][CHUNK_NUM];
};

#endif  // SRC_WORLD_H_


