
#ifndef VOXEL_V2I_H_
#define VOXEL_V2I_H_

#include <stdint.h>

struct v2i {
	int64_t x, y;
};

static inline struct v2i v2i(int64_t x, int64_t y)
{
	return (struct v2i) { x, y };
}

static inline int v2i_eql(struct v2i u, struct v2i v)
{
	return u.x == v.x && u.y == v.y;
}

static inline int v2i_neq(struct v2i u, struct v2i v)
{
	return u.x != v.x || u.y != v.y;
}

static inline struct v2i v2i_add(struct v2i u, struct v2i v)
{
	return v2i(u.x + v.x, u.y + v.y);
}

static inline struct v2i v2i_addx(struct v2i u, struct v2i v, int64_t d)
{
	return v2i(u.x + v.x * d, u.y + v.y * d);
}

static inline struct v2i v2i_sub(struct v2i u, struct v2i v)
{
	return v2i(u.x - v.x, u.y - v.y);
}

static inline struct v2i v2i_subx(struct v2i u, struct v2i v, int64_t d)
{
	return v2i(u.x - v.x * d, u.y - v.y * d);
}

static inline struct v2i v2i_mul(struct v2i u, int64_t f)
{
	return v2i(u.x * f, u.y * f);
}

static inline int64_t v2i_dot(struct v2i u, struct v2i v)
{
	return u.x * v.x + u.y * v.y;
}

static inline int64_t v2i_length(struct v2i u)
{
	return u.x + u.y;
}

static inline int64_t v2i_dist(struct v2i u, struct v2i v)
{
	return v2i_length(v2i_sub(u, v));
}

#endif
