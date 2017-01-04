
#ifndef VOXEL_V2F_H_
#define VOXEL_V2F_H_

#include <math.h>

struct v2f {
	float x, y;
};

static inline struct v2f v2f(float x, float y)
{
	return (struct v2f) { x, y };
}

static inline int v2f_eql(struct v2f u, struct v2f v)
{
	return u.x == v.x && u.y == v.y;
}

static inline int v2f_neq(struct v2f u, struct v2f v)
{
	return u.x != v.x || u.y != v.y;
}

static inline struct v2f v2f_add(struct v2f u, struct v2f v)
{
	return v2f(u.x + v.x, u.y + v.y);
}

static inline struct v2f v2f_addx(struct v2f u, struct v2f v, float d)
{
	return v2f(u.x + v.x * d, u.y + v.y * d);
}

static inline struct v2f v2f_sub(struct v2f u, struct v2f v)
{
	return v2f(u.x - v.x, u.y - v.y);
}

static inline struct v2f v2f_subx(struct v2f u, struct v2f v, float d)
{
	return v2f(u.x - v.x * d, u.y - v.y * d);
}

static inline struct v2f v2f_mul(struct v2f u, float f)
{
	return v2f(u.x * f, u.y * f);
}

static inline float v2f_dot(struct v2f u, struct v2f v)
{
	return u.x * v.x + u.y * v.y;
}

static inline float v2f_length(struct v2f u)
{
	return hypot(u.x, u.y);
}

static inline float v2f_dist(struct v2f u, struct v2f v)
{
	return v2f_length(v2f_sub(u, v));
}

static inline struct v2f v2f_normalize(struct v2f u)
{
	return v2f_mul(u, 1. / v2f_length(u));
}

static inline struct v2f v2f_normalize_safe(struct v2f u)
{
	float d = v2f_length(u);
	return d == 0 ? u : v2f_mul(u, 1. / d);
}

static inline struct v2f v2f_rot(struct v2f u, float a)
{
	float c;
	float s;

	if (a) {
		c = cos(a);
		s = sin(a);
		u = v2f(c * u.x - s * u.y, s * u.x + c * u.y);
	}

	return u;
}

#endif
