
#ifndef VOXEL_V3_H_
#define VOXEL_V3_H_

#include <math.h>

struct v3f {
	float x, y, z;
};

struct v3d {
	double x, y, z;
};

struct v3c {
	char x, y, z;
};

struct v3s {
	short x, y, z;
};

struct v3l {
	long x, y, z;
};

struct v3ll {
	long long x, y, z;
};

static inline struct v3f v3f(float x, float y, float z)
{
	return (struct v3f) { x, y, z };
}

static inline struct v3d v3d(double x, double y, double z)
{
	return (struct v3d) { x, y, z };
}

static inline struct v3c v3c(char x, char y, char z)
{
	return (struct v3c) { x, y, z };
}

static inline struct v3s v3s(short x, short y, short z)
{
	return (struct v3s) { x, y, z };
}

static inline struct v3l v3l(long x, long y, long z)
{
	return (struct v3l) { x, y, z };
}

static inline struct v3ll v3ll(long long x, long long y, long long z)
{
	return (struct v3ll) { x, y, z };
}

#define v3_eql(u, v) (u.x == v.x && u.y == v.y && u.z == v.z)

#define v3_neq(u, v) (u.x != v.x || u.y != v.y || u.z != v.z)

#define v3_add(u, v) ((typeof(u)){ u.x + v.x, u.y + v.y, u.z + v.z })

#define v3_addx(u, v, d) ((typeof(u)){ u.x + v.x * d, u.y + v.y * d, u.z + v.z * d })

#define v3_sub(u, v) ((typeof(u)){ u.x - v.x, u.y - v.y, u.z - v.z })

#define v3_subx(u, v, d) ((typeof(u)){ u.x - v.x * d, u.y - v.y * d, u.z - v.z * d })

#define v3_mul(u, d) ((typeof(u)){ u.x * d, u.y * d, u.z * d })

#define v3_dot(u, v) (u.x * v.x + u.y * v.y + u.z * v.z)

#define v3_length(u) (hypot(hypot(u.x, u.y), u.z))

#define v3_dist(u, v) v3_length(v3_sub(u, v))

#define v3_normalize(u) v3_mul(u, 1.0 / v3_length(u))

#define v3_rotx(u, a) ((typeof(u)){ u.x, cos(a) * u.y - sin(a) * u.z, sin(a) * u.y + cos(a) * u.z })

#define v3_roty(u, a) ((typeof(u)){ cos(a) * u.x - sin(a) * u.z, u.y, sin(a) * u.x + cos(a) * u.z })

#define v3_rotz(u, a) ((typeof(u)){ cos(a) * u.x - sin(a) * u.y, sin(a) * u.x + cos(a) * u.y, u.z })

#define v3_rot(u, a) v3_rotz(v3_roty(v3_rotx(u, r.x), r.y), r.z)

#endif
