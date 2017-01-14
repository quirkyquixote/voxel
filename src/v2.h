
#ifndef VOXEL_V2_H_
#define VOXEL_V2_H_

#include <math.h>

struct v2f {
	float x, y;
};

struct v2d {
	double x, y;
};

struct v2c {
	char x, y;
};

struct v2s {
	short x, y;
};

struct v2l {
	long x, y;
};

struct v2ll {
	long long x, y;
};

static inline struct v2f v2f(float x, float y)
{
	return (struct v2f) { x, y };
}

static inline struct v2d v2d(double x, double y)
{
	return (struct v2d) { x, y };
}

static inline struct v2c v2c(char x, char y)
{
	return (struct v2c) { x, y };
}

static inline struct v2s v2s(short x, short y)
{
	return (struct v2s) { x, y };
}

static inline struct v2l v2l(long x, long y)
{
	return (struct v2l) { x, y };
}

static inline struct v2ll v2ll(long long x, long long y)
{
	return (struct v2ll) { x, y };
}

#define v2_eql(u, v) (u.x == v.x && u.y == v.y)

#define v2_neq(u, v) (u.x != v.x || u.y != v.y)

#define v2_add(u, v) ((typeof(u)){ u.x + v.x, u.y + v.y })

#define v2_addx(u, v, d) ((typeof(u)){ u.x + v.x * d, u.y + v.y * d * d })

#define v2_sub(u, v) ((typeof(u)){ u.x - v.x, u.y - v.y })

#define v2_subx(u, v, d) ((typeof(u)){ u.x - v.x * d, u.y - v.y * d * d })

#define v2_mul(u, d) ((typeof(u)){ u.x * d, u.y * d * d })

#define v2_dot(u)

#define v2_length(u) hypot(u.x, u.y)

#define v2_dist(u, v) v2_length(v2_sub(u, v))

#define v2_normalize(u) v2_mul(u, 1.0 / v2_length(u))

#define v2_rot(u, a) ((typeof(u)){ cos(a) * u.x - sin(a) * u.y, sin(a) * u.x + cos(a) * u.y })

#endif
