
#ifndef VOXEL_V3F_H_
#define VOXEL_V3F_H_

#include <math.h>

struct v3f {
	float x, y, z;
};

static inline struct v3f v3f(float x, float y, float z)
{
	return (struct v3f) { x, y, z };
}

static inline int v3f_eql(struct v3f u, struct v3f v)
{
	return u.x == v.x && u.y == v.y && u.z == v.z;
}

static inline int v3f_neq(struct v3f u, struct v3f v)
{
	return u.x != v.x || u.y != v.y || u.z != v.z;
}

static inline struct v3f v3f_add(struct v3f u, struct v3f v)
{
	return v3f(u.x + v.x, u.y + v.y, u.z + v.z);
}

static inline struct v3f v3f_addx(struct v3f u, struct v3f v, float d)
{
	return v3f(u.x + v.x * d, u.y + v.y * d, u.z + v.z * d);
}

static inline struct v3f v3f_sub(struct v3f u, struct v3f v)
{
	return v3f(u.x - v.x, u.y - v.y, u.z - v.z);
}

static inline struct v3f v3f_subx(struct v3f u, struct v3f v, float d)
{
	return v3f(u.x - v.x * d, u.y - v.y * d, u.z - v.z * d);
}

static inline struct v3f v3f_mul(struct v3f u, float f)
{
	return v3f(u.x * f, u.y * f, u.z * f);
}

static inline float v3f_dot(struct v3f u, struct v3f v)
{
	return u.x * v.x + u.y * v.y + u.z * v.z;
}

static inline float v3f_length(struct v3f u)
{
	return hypot(hypot(u.x, u.y), u.z);
}

static inline float v3f_dist(struct v3f u, struct v3f v)
{
	return v3f_length(v3f_sub(u, v));
}

static inline struct v3f v3f_normalize(struct v3f u)
{
	return v3f_mul(u, 1. / v3f_length(u));
}

static inline struct v3f v3f_normalize_safe(struct v3f u)
{
	float d = v3f_length(u);
	return d == 0 ? u : v3f_mul(u, 1. / d);
}

static inline struct v3f v3f_rotx(struct v3f u, float a)
{
	float c;
	float s;

	if (a) {
		c = cos(a);
		s = sin(a);
		u = v3f(u.x, c * u.y - s * u.z, s * u.y + c * u.z);
	}

	return u;
}

static inline struct v3f v3f_roty(struct v3f u, float a)
{
	float c;
	float s;

	if (a) {
		c = cos(a);
		s = sin(a);
		u = v3f(c * u.x - s * u.z, u.y, s * u.x + c * u.z);
	}

	return u;
}

static inline struct v3f v3f_rotz(struct v3f u, float a)
{
	float c;
	float s;

	if (a) {
		c = cos(a);
		s = sin(a);
		u = v3f(c * u.x - s * u.y, s * u.x + c * u.y, u.z);
	}

	return u;
}

static inline struct v3f v3f_rot(struct v3f u, struct v3f r)
{
	u = v3f_rotx(u, r.x);
	u = v3f_roty(u, r.y);
	u = v3f_rotz(u, r.z);
	return u;
}

#endif
