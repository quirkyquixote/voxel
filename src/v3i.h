
#ifndef VOXEL_V3I_H_
#define VOXEL_V3I_H_

#include <math.h>
#include <stdint.h>

struct v3i {
	int64_t x, y, z;
};

static inline struct v3i v3i(int64_t x, int64_t y, int64_t z)
{
	return (struct v3i) { x, y, z };
}

static inline int v3i_eql(struct v3i u, struct v3i v)
{
	return u.x == v.x && u.y == v.y && u.z == v.z;
}

static inline int v3i_neq(struct v3i u, struct v3i v)
{
	return u.x != v.x || u.y != v.y || u.z != v.z;
}

static inline struct v3i v3i_add(struct v3i u, struct v3i v)
{
	return v3i(u.x + v.x, u.y + v.y, u.z + v.z);
}

static inline struct v3i v3i_addx(struct v3i u, struct v3i v, int64_t d)
{
	return v3i(u.x + v.x * d, u.y + v.y * d, u.z + v.z * d);
}

static inline struct v3i v3i_sub(struct v3i u, struct v3i v)
{
	return v3i(u.x - v.x, u.y - v.y, u.z - v.z);
}

static inline struct v3i v3i_subx(struct v3i u, struct v3i v, int64_t d)
{
	return v3i(u.x - v.x * d, u.y - v.y * d, u.z - v.z * d);
}

static inline struct v3i v3i_mul(struct v3i u, int64_t f)
{
	return v3i(u.x * f, u.y * f, u.z * f);
}

static inline int64_t v3i_length(struct v3i u)
{
	return u.x + u.y + u.z;
}

static inline int64_t v3i_dist(struct v3i u, struct v3i v)
{
	return v3i_length(v3i_sub(u, v));
}

#endif
