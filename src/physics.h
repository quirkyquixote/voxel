



#ifndef VOXEL_PHYSICS_H_
#define VOXEL_PHYSICS_H_

#include "list.h"
#include "v2.h"
#include "v3.h"
#include "aab3.h"
#include "chunk.h"

enum {
	FACE_FT,
	FACE_LF,
	FACE_BK,
	FACE_RT,
	FACE_UP,
	FACE_DN,
};

enum {
	CELL_LDB = 1 << 0,
	CELL_LDF = 1 << 1,
	CELL_LUB = 1 << 2,
	CELL_LUF = 1 << 3,
	CELL_RDB = 1 << 4,
	CELL_RDF = 1 << 5,
	CELL_RUB = 1 << 6,
	CELL_RUF = 1 << 7,
};

struct body {
	struct list list;
	struct v3f p;
	struct v3f r;
	struct v3f v;
	struct v2f s;
	struct aab3f bb;
};

struct space {
	int iterations;
	float impulse;
	struct v3f gravity;
	float terminal_speed;
	struct world *world;
	struct list body;
};

struct query {
	int face;
	int cell;
	struct v3ll p;
};

struct body *body(struct space *s);
void body_destroy(struct body *b);

static inline struct v3f body_get_position(struct body *b)
{
	return b->p;
}

static inline struct v3f body_get_rotation(struct body *b)
{
	return b->r;
}

static inline struct v3f body_get_velocity(struct body *b)
{
	return b->v;
}

static inline struct v2f body_get_size(struct body *b)
{
	return b->s;
}

static inline void body_set_position(struct body *b, struct v3f p)
{
	b->p = p;
}

static inline void body_set_rotation(struct body *b, struct v3f r)
{
	b->r = r;
}

static inline void body_set_velocity(struct body *b, struct v3f v)
{
	b->v = v;
}

static inline void body_set_size(struct body *b, struct v2f s)
{
	b->s = s;
}

struct space *space(struct world *w);
void space_destroy(struct space *s);
void space_run(struct space *s);
int space_query(struct space *s, struct v3f p, struct v3f v, struct query *q);

static inline void space_set_gravity(struct space *s, struct v3f g)
{
	s->gravity = g;
}

static inline void space_set_iterations(struct space *s, int i)
{
	s->iterations = i;
}

static inline void space_set_impulse(struct space *s, float i)
{
	s->impulse = i;
}

static inline void space_set_terminal_speed(struct space *s, float t)
{
	s->terminal_speed = t;
}


#endif

