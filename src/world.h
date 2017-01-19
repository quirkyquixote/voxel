/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_WORLD_H_
#define SRC_WORLD_H_

#include "chunk.h"

#define CHUNKS_PER_WORLD 16

#define WORLD_W (CHUNK_W * CHUNKS_PER_WORLD)
#define WORLD_H CHUNK_H
#define WORLD_D (CHUNK_D * CHUNKS_PER_WORLD)
#define WORLD_AREA (WORLD_W * WORLD_D)
#define WORLD_VOLUME (WORLD_AREA * WORLD_H)

#define SHARDS_PER_WORLD (CHUNKS_PER_WORLD * CHUNKS_PER_WORLD * SHARDS_PER_CHUNK)

class World {
 public:
	explicit World(Context *ctx);
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

#endif  // SRC_WORLD_H_


