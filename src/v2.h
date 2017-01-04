
#ifndef VOXEL_V2_H_
#define VOXEL_V2_H_

#include <math.h>

/*----------------------------------------------------------------------------
 * 3D v2f_or
 *----------------------------------------------------------------------------*/
struct v2f {
	float x, y;
};

/* Assign to any v2f_or to reset it to (0,0,0) */
extern const struct v2f v2f_zero;

/* Create a new v2f_or */
static inline struct v2f v2f(float x, float y)
{
	return (struct v2f) { x, y };
}

/* Return 1 if u,v are equal; 0 otherwise */
static inline int v2f_eql(struct v2f u, struct v2f v)
{
	return u.x == v.x && u.y == v.y;
}

/* Return 1 if u,v are not equal; 0 otherwise */
static inline int v2f_neq(struct v2f u, struct v2f v)
{
	return u.x != v.x || u.y != v.y;
}

/* Return u + v */
static inline struct v2f v2f_add(struct v2f u, struct v2f v)
{
	return v2f(u.x + v.x, u.y + v.y);
}

/* Return u + (v * d) */
static inline struct v2f v2f_addx(struct v2f u, struct v2f v, float d)
{
	return v2f(u.x + v.x * d, u.y + v.y * d);
}

/* Return u - v */
static inline struct v2f v2f_sub(struct v2f u, struct v2f v)
{
	return v2f(u.x - v.x, u.y - v.y);
}

/* Return u - (v * d) */
static inline struct v2f v2f_subx(struct v2f u, struct v2f v, float d)
{
	return v2f(u.x - v.x * d, u.y - v.y * d);
}

/* Return u * f */
static inline struct v2f v2f_mul(struct v2f u, float f)
{
	return v2f(u.x * f, u.y * f);
}

/* Return the dot product of u and v */
static inline float v2f_dot(struct v2f u, struct v2f v)
{
	return u.x * v.x + u.y * v.y;
}

/* Return the length of u */
static inline float v2f_length(struct v2f u)
{
	return hypot(u.x, u.y);
}

/* Return the distance between u and v */
static inline float v2f_dist(struct v2f u, struct v2f v)
{
	return v2f_length(v2f_sub(u, v));
}

/* Return the normalized form of u */
static inline struct v2f v2f_normalize(struct v2f u)
{
	return v2f_mul(u, 1. / v2f_length(u));
}

/* Return the normalized form of u. Handle null v2f_or gracefully */
static inline struct v2f v2f_normalize_safe(struct v2f u)
{
	float d = v2f_length(u);
	return d == 0 ? u : v2f_mul(u, 1. / d);
}

/* Rotate u a radians */
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
