/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_SHARD_H_
#define SRC_SHARD_H_

#include <stdint.h>
#include <assert.h>

#include "sz.h"
#include "v2.h"
#include "v3.h"
#include "box3.h"
#include "types.h"
#include "entity.h"
#include "noncopyable.h"

class Shard : public NonCopyable {
 public:
	static const long long W = 16;
	static const long long H = 16;
	static const long long D = 16;
	static const long long AREA = W * D;
	static const long long VOLUME = AREA * H;

	Shard(int id, int y);
	~Shard();

	void load(sz_Tag *root);
	sz_Tag *save();

	inline int get_id() const { return id; }
	inline int get_y() const { return y; }

	inline int get_mat(const v3ll &p) const
	{
		assert(p.x >= 0 && p.x < W);
		assert(p.y >= 0 && p.y < H);
		assert(p.z >= 0 && p.z < D);
		return mat[p.x][p.y][p.z];
	}

	inline int get_shape(const v3ll &p) const
	{
		assert(p.x >= 0 && p.x < W);
		assert(p.y >= 0 && p.y < H);
		assert(p.z >= 0 && p.z < D);
		return shape[p.x][p.y][p.z];
	}

	inline int get_light(const v3ll &p) const
	{
		assert(p.x >= 0 && p.x < W);
		assert(p.y >= 0 && p.y < H);
		assert(p.z >= 0 && p.z < D);
		return light[p.x][p.y][p.z];
	}

	inline Entity * get_data(const v3ll &p) const
	{
		assert(p.x >= 0 && p.x < W);
		assert(p.y >= 0 && p.y < H);
		assert(p.z >= 0 && p.z < D);
		return data[p.x][p.y][p.z];
	}

	inline void set_mat(const v3ll &p, int val)
	{
		assert(p.x >= 0 && p.x < W);
		assert(p.y >= 0 && p.y < H);
		assert(p.z >= 0 && p.z < D);
		mat[p.x][p.y][p.z] = val;
	}

	inline void set_shape(const v3ll &p, int val)
	{
		assert(p.x >= 0 && p.x < W);
		assert(p.y >= 0 && p.y < H);
		assert(p.z >= 0 && p.z < D);
		shape[p.x][p.y][p.z] = val;
	}

	inline void set_light(const v3ll &p, int val)
	{
		assert(p.x >= 0 && p.x < W);
		assert(p.y >= 0 && p.y < H);
		assert(p.z >= 0 && p.z < D);
		light[p.x][p.y][p.z] = val;
	}

	inline void set_data(const v3ll &p, Entity *val)
	{
		assert(p.x >= 0 && p.x < W);
		assert(p.y >= 0 && p.y < H);
		assert(p.z >= 0 && p.z < D);
		data[p.x][p.y][p.z] = val;
	}

 private:
	int id;
	int8_t y;
	uint8_t mat[W][H][D];
	uint8_t shape[W][H][D];
	uint8_t light[W][H][D];
	Entity *data[W][H][D];
};

#endif  // SRC_SHARD_H_


