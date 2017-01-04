
#ifndef VOXEL_V2_H_
#define VOXEL_V2_H_

#include <math.h>

/*----------------------------------------------------------------------------
 * 3D v2for
 *----------------------------------------------------------------------------*/
struct v2f {
	float x, y;
};

/* Assign to any v2for to reset it to (0,0,0) */
extern const struct v2f v2fzero;

/* Create a new v2for */
static inline struct v2f v2f(float x, float y)
{
	return (struct v2f) { x, y };
}

/* Return 1 if u,v are equal; 0 otherwise */
static inline int v2feql(struct v2f u, struct v2f v)
{
	return u.x == v.x && u.y == v.y;
}

/* Return 1 if u,v are not equal; 0 otherwise */
static inline int v2fneq(struct v2f u, struct v2f v)
{
	return u.x != v.x || u.y != v.y;
}

/* Return u + v */
static inline struct v2f v2fadd(struct v2f u, struct v2f v)
{
	return v2f(u.x + v.x, u.y + v.y);
}

/* Return u + (v * d) */
static inline struct v2f v2faddx(struct v2f u, struct v2f v, float d)
{
	return v2f(u.x + v.x * d, u.y + v.y * d);
}

/* Return u - v */
static inline struct v2f v2fsub(struct v2f u, struct v2f v)
{
	return v2f(u.x - v.x, u.y - v.y);
}

/* Return u - (v * d) */
static inline struct v2f v2fsubx(struct v2f u, struct v2f v, float d)
{
	return v2f(u.x - v.x * d, u.y - v.y * d);
}

/* Return u * f */
static inline struct v2f v2fmul(struct v2f u, float f)
{
	return v2f(u.x * f, u.y * f);
}

/* Return the dot product of u and v */
static inline float v2fdot(struct v2f u, struct v2f v)
{
	return u.x * v.x + u.y * v.y;
}

/* Return the length of u */
static inline float v2flength(struct v2f u)
{
	return hypot(u.x, u.y);
}

/* Return the distance between u and v */
static inline float v2fdist(struct v2f u, struct v2f v)
{
	return v2flength(v2fsub(u, v));
}

/* Return the normalized form of u */
static inline struct v2f v2fnormalize(struct v2f u)
{
	return v2fmul(u, 1. / v2flength(u));
}

/* Return the normalized form of u. Handle null v2for gracefully */
static inline struct v2f v2fnormalize_safe(struct v2f u)
{
	float d = v2flength(u);
	return d == 0 ? u : v2fmul(u, 1. / d);
}

/* Rotate u a radians */
static inline struct v2f v2frot(struct v2f u, float a)
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
