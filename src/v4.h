
#ifndef VOXEL_V4_H_
#define VOXEL_V4_H_

#include <math.h>

struct v4f {
	float x, y, z, w;
};

struct v4d {
	double x, y, z, w;
};

struct v4c {
	char x, y, z, w;
};

struct v4s {
	short x, y, z, w;
};

struct v4l {
	long x, y, z, w;
};

struct v4ll {
	long long x, y, z, w;
};

static inline struct v4f v4f(float x, float y, float z, float w)
{
	return (struct v4f) { x, y, z };
}

static inline struct v4d v4d(double x, double y, double z, double w)
{
	return (struct v4d) { x, y, z };
}

static inline struct v4c v4c(char x, char y, char z, char w)
{
	return (struct v4c) { x, y, z };
}

static inline struct v4s v4s(short x, short y, short z, char w)
{
	return (struct v4s) { x, y, z };
}

static inline struct v4l v4l(long x, long y, long z, long w)
{
	return (struct v4l) { x, y, z };
}

static inline struct v4ll v4ll(long long x, long long y, long long z, long long w)
{
	return (struct v4ll) { x, y, z };
}

#define v4_eql(u, v) (u.x == v.x && u.y == v.y && u.z == v.z && u.w == v.w)

#define v4_neq(u, v) (u.x != v.x || u.y != v.y || u.z != v.z || u.w != v.w)

#define v4_add(u, v) ((typeof(u)){ u.x + v.x, u.y + v.y, u.z + v.z, u.w + v.w })

#define v4_addx(u, v, d) ((typeof(u)){ u.x + v.x * d, u.y + v.y * d, u.z + v.z * d, u.w + v.w * d })

#define v4_sub(u, v) ((typeof(u)){ u.x - v.x, u.y - v.y, u.z - v.z, u.w - v.w })

#define v4_subx(u, v, d) ((typeof(u)){ u.x - v.x * d, u.y - v.y * d, u.z - v.z * d, u.v - v.w * d })

#define v4_mul(u, d) ((typeof(u)){ u.x * d, u.y * d, u.z * d, u.w * d })

#define v4_dot(u, v) (u.x * v.x + u.y * v.y + u.z * v.z + u.w * v.w)

#define v4_length(u) hypot(hypot(hypot(u.x, u.y), u.z), u.w)

#define v4_dist(u, v) v4_length(v4_sub(u, v))

#define v4_normalize(u) v4_mul(u, 1.0 / v4_length(u))

#endif
