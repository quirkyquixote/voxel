
#ifndef VOXEL_VEC_H_
#define VOXEL_VEC_H_

#include <math.h>

/*----------------------------------------------------------------------------
 * 3D v3or
 *----------------------------------------------------------------------------*/
struct v3 {
	float x;
	float y;
	float z;
};

/* Assign to any v3or to reset it to (0,0,0) */
extern const struct v3 v3zero;

/* Create a new v3or */
static inline struct v3 v3(float x, float y, float z)
{
	return (struct v3) { x, y, z };
}

/* Return 1 if u,v are equal; 0 otherwise */
static inline int v3eql(struct v3 u, struct v3 v)
{
	return u.x == v.x && u.y == v.y && u.z == v.z;
}

/* Return 1 if u,v are not equal; 0 otherwise */
static inline int v3neq(struct v3 u, struct v3 v)
{
	return u.x != v.x || u.y != v.y || u.z != v.z;
}

/* Return u + v */
static inline struct v3 v3add(struct v3 u, struct v3 v)
{
	return v3(u.x + v.x, u.y + v.y, u.z + v.z);
}

/* Return u + (v * d) */
static inline struct v3 v3addx(struct v3 u, struct v3 v, float d)
{
	return v3(u.x + v.x * d, u.y + v.y * d, u.z + v.z * d);
}

/* Return u - v */
static inline struct v3 v3sub(struct v3 u, struct v3 v)
{
	return v3(u.x - v.x, u.y - v.y, u.z - v.z);
}

/* Return u - (v * d) */
static inline struct v3 v3subx(struct v3 u, struct v3 v, float d)
{
	return v3(u.x - v.x * d, u.y - v.y * d, u.z - v.z * d);
}

/* Return u * f */
static inline struct v3 v3mul(struct v3 u, float f)
{
	return v3(u.x * f, u.y * f, u.z * f);
}

/* Return the dot product of u and v */
static inline float v3dot(struct v3 u, struct v3 v)
{
	return u.x * v.x + u.y * v.y + u.z * v.z;
}

/* Return the length of u */
static inline float v3length(struct v3 u)
{
	return hypot(hypot(u.x, u.y), u.z);
}

/* Return the distance between u and v */
static inline float v3dist(struct v3 u, struct v3 v)
{
	return v3length(v3sub(u, v));
}

/* Return the normalized form of u */
static inline struct v3 v3normalize(struct v3 u)
{
	return v3mul(u, 1. / v3length(u));
}

/* Return the normalized form of u. Handle null v3or gracefully */
static inline struct v3 v3normalize_safe(struct v3 u)
{
	float d = v3length(u);
	return d == 0 ? u : v3mul(u, 1. / d);
}

/* Rotate u a radians in the x axis */
static inline struct v3 v3rotx(struct v3 u, float a)
{
	float c;
	float s;

	if (a) {
		c = cos(a);
		s = sin(a);
		u = v3(u.x, c * u.y - s * u.z, s * u.y + c * u.z);
	}

	return u;
}

/* Rotate u a radians in the y axis */
static inline struct v3 v3roty(struct v3 u, float a)
{
	float c;
	float s;

	if (a) {
		c = cos(a);
		s = sin(a);
		u = v3(c * u.x - s * u.z, u.y, s * u.x + c * u.z);
	}

	return u;
}

/* Rotate u a radians in the z axis */
static inline struct v3 v3rotz(struct v3 u, float a)
{
	float c;
	float s;

	if (a) {
		c = cos(a);
		s = sin(a);
		u = v3(c * u.x - s * u.y, s * u.x + c * u.y, u.z);
	}

	return u;
}

/* Rotate u a.x,a.y.a.z radians in the x,y,z axes */
static inline struct v3 v3rot(struct v3 u, struct v3 r)
{
	u = v3rotx(u, r.x);
	u = v3roty(u, r.y);
	u = v3rotz(u, r.z);
	return u;
}

#endif
