



#ifndef VOXEL_PHYSICS_H_
#define VOXEL_PHYSICS_H_

#include <list>
#include <functional>

#include "v2.h"
#include "v3.h"
#include "box3.h"
#include "chunk.h"

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

class Space;
class Body;
struct Query;

class Body {
	friend class Space;

public:
	Body(Space *s);
	~Body();

	inline v3f get_p() const { return p; }
	inline v3f get_r() const { return r; }
	inline v3f get_v() const { return v; }
	inline v2f get_size() const { return s; }
	inline box3f get_box() const { return bb; }
	inline int get_step_size() const { return step_size; }

	inline void set_p(const v3f &new_p) { p = new_p; }
	inline void set_r(const v3f &new_r) { r = new_r; }
	inline void set_v(const v3f &new_v) { v = new_v; }
	inline void set_size(const v2f &new_s) { s = new_s; }
	inline void set_step_size(int s) { step_size = s; }

	inline void set_callback(const std::function<void(Body *, const v3ll &, int)> &func)
	{
		cb_func = func;
	}

private:
	v3f p;
	v3f r;
	v3f v;
	v2f s;
	box3f bb;
	int step_size;
	std::function<void(Body *, const v3ll, int)> cb_func;
};

class Space {
public:
	Space(World *w);
	~Space();

	void run();
	int query(const v3f &p, const v3f &v, Query *q);

	inline void set_gravity(float g) { gravity = g; }
	inline void set_iterations(int i) { iterations = i; }
	inline void set_impulse(float i) { impulse = i; }
	inline void set_terminal_speed(float t) { terminal_speed = t; }

	inline Body *create_body()
	{
		Body *b = new Body(this);
		bodies.push_back(b);
		return b;
	}

	inline void destroy_body(Body *b)
	{
		bodies.remove(b);
		delete b;
	}

private:
	int iterations;
	float impulse;
	float gravity;
	float terminal_speed;
	World *world;
	std::list<Body *> bodies;

	int cell_at(const int *masks, int64_t x, int64_t y, int64_t z);
	void move_xpos(Body *b, float dt);
	void move_xneg(Body *b, float dt);
	void move_zpos(Body *b, float dt);
	void move_zneg(Body *b, float dt);
	void move_ypos(Body *b, float dt);
	void move_yneg(Body *b, float dt);
	void step(float dt);

	int query_xpos(const v3f &p, const v3f &v, Query *q, float *best_t);
	int query_xneg(const v3f &p, const v3f &v, Query *q, float *best_t);
	int query_zpos(const v3f &p, const v3f &v, Query *q, float *best_t);
	int query_zneg(const v3f &p, const v3f &v, Query *q, float *best_t);
	int query_ypos(const v3f &p, const v3f &v, Query *q, float *best_t);
	int query_yneg(const v3f &p, const v3f &v, Query *q, float *best_t);
};

struct Query {
	int face;
	v3ll p;
	v3f q;
};

#endif

