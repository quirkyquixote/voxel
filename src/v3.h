
#ifndef VOXEL_VEC_H_
#define VOXEL_VEC_H_

#include <math.h>

/*----------------------------------------------------------------------------
 * 3D v3for
 *----------------------------------------------------------------------------*/
struct v3f {
	float x, y, z;
};

/* Assign to any v3for to reset it to (0,0,0) */
extern const struct v3f v3fzero;

/* Create a new v3for */
static inline struct v3f v3f(float x, float y, float z)
{
	return (struct v3f) { x, y, z };
}

/* Return 1 if u,v are equal; 0 otherwise */
static inline int v3feql(struct v3f u, struct v3f v)
{
	return u.x == v.x && u.y == v.y && u.z == v.z;
}

/* Return 1 if u,v are not equal; 0 otherwise */
static inline int v3fneq(struct v3f u, struct v3f v)
{
	return u.x != v.x || u.y != v.y || u.z != v.z;
}

/* Return u + v */
static inline struct v3f v3fadd(struct v3f u, struct v3f v)
{
	return v3f(u.x + v.x, u.y + v.y, u.z + v.z);
}

/* Return u + (v * d) */
static inline struct v3f v3faddx(struct v3f u, struct v3f v, float d)
{
	return v3f(u.x + v.x * d, u.y + v.y * d, u.z + v.z * d);
}

/* Return u - v */
static inline struct v3f v3fsub(struct v3f u, struct v3f v)
{
	return v3f(u.x - v.x, u.y - v.y, u.z - v.z);
}

/* Return u - (v * d) */
static inline struct v3f v3fsubx(struct v3f u, struct v3f v, float d)
{
	return v3f(u.x - v.x * d, u.y - v.y * d, u.z - v.z * d);
}

/* Return u * f */
static inline struct v3f v3fmul(struct v3f u, float f)
{
	return v3f(u.x * f, u.y * f, u.z * f);
}

/* Return the dot product of u and v */
static inline float v3fdot(struct v3f u, struct v3f v)
{
	return u.x * v.x + u.y * v.y + u.z * v.z;
}

/* Return the length of u */
static inline float v3flength(struct v3f u)
{
	return hypot(hypot(u.x, u.y), u.z);
}

/* Return the distance between u and v */
static inline float v3fdist(struct v3f u, struct v3f v)
{
	return v3flength(v3fsub(u, v));
}

/* Return the normalized form of u */
static inline struct v3f v3fnormalize(struct v3f u)
{
	return v3fmul(u, 1. / v3flength(u));
}

/* Return the normalized form of u. Handle null v3for gracefully */
static inline struct v3f v3fnormalize_safe(struct v3f u)
{
	float d = v3flength(u);
	return d == 0 ? u : v3fmul(u, 1. / d);
}

/* Rotate u a radians in the x axis */
static inline struct v3f v3frotx(struct v3f u, float a)
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

/* Rotate u a radians in the y axis */
static inline struct v3f v3froty(struct v3f u, float a)
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

/* Rotate u a radians in the z axis */
static inline struct v3f v3frotz(struct v3f u, float a)
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

/* Rotate u a.x,a.y.a.z radians in the x,y,z axes */
static inline struct v3f v3frot(struct v3f u, struct v3f r)
{
	u = v3frotx(u, r.x);
	u = v3froty(u, r.y);
	u = v3frotz(u, r.z);
	return u;
}

#endif
