/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_PHYSICS_H_
#define SRC_PHYSICS_H_

#include <functional>
#include <vector>

#include "v2.h"
#include "v3.h"
#include "box3.h"
#include "ptr_list.h"

class World;
class Space;
class Body;
struct Query;

class Body : public PtrNode {
	friend class Space;

 public:
	typedef void(Callback)(Body *, const v3ll &, int);

	Body() = delete;
	explicit Body(Space *s);
	~Body();

	inline v3f get_p() const { return p; }
	inline v3f get_r() const { return r; }
	inline v3f get_v() const { return v; }
	inline v2f get_size() const { return s; }
	inline box3f get_box() const { return bb; }
	inline float get_step_size() const { return step_size; }

	inline void set_p(const v3f &new_p) { p = new_p; }
	inline void set_r(const v3f &new_r) { r = new_r; }
	inline void set_v(const v3f &new_v) { v = new_v; }
	inline void set_size(const v2f &new_s) { s = new_s; }
	inline void set_step_size(float s) { step_size = s; }

	inline void set_callback(const std::function<Body::Callback> &func)
	{
		cb_func = func;
	}

 private:
	v3f p;
	v3f r;
	v3f v;
	v2f s;
	box3f bb;
	float step_size;
	std::function<Callback> cb_func;
};

class Space : public NonCopyable {
 public:
	Space() = delete;
	explicit Space(World *w);
	~Space();

	void step(float dt);
	int query(const v3f &p, const v3f &v, Query *q);

	inline void set_gravity(float g) { gravity = g; }
	inline void set_impulse(float i) { impulse = i; }
	inline void set_terminal_speed(float t) { terminal_speed = t; }

	inline Body *create_body()
	{
		Body *b = new Body(this);
		bodies.push_back(b);
		return b;
	}

 private:
	float impulse;
	float gravity;
	float terminal_speed;
	World *world;
	PtrList<Body> bodies;
	std::vector<box3f> geom[256];

	void move_xpos(Body *b, float dt);
	void move_xneg(Body *b, float dt);
	void move_zpos(Body *b, float dt);
	void move_zneg(Body *b, float dt);
	void move_ypos(Body *b, float dt);
	void move_yneg(Body *b, float dt);

	void query_xpos(const v3f &p, const v3f &v, Query *q, float *best_t);
	void query_xneg(const v3f &p, const v3f &v, Query *q, float *best_t);
	void query_zpos(const v3f &p, const v3f &v, Query *q, float *best_t);
	void query_zneg(const v3f &p, const v3f &v, Query *q, float *best_t);
	void query_ypos(const v3f &p, const v3f &v, Query *q, float *best_t);
	void query_yneg(const v3f &p, const v3f &v, Query *q, float *best_t);
};

struct Query {
	int face;
	v3ll p;
	v3f q;
};

#endif  // SRC_PHYSICS_H_

