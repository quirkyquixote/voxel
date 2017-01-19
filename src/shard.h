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

#define SHARD_W 16
#define SHARD_H 16
#define SHARD_D 16
#define SHARD_AREA (SHARD_W * SHARD_D)
#define SHARD_VOLUME (SHARD_AREA * SHARD_H)

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

#endif  // SRC_SHARD_H_


