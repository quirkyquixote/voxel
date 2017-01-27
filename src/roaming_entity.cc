/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "roaming_entity.h"

#include "context.h"

RoamingEntity::RoamingEntity(Context *ctx, int capacity)
	: Entity(ctx, capacity)
{
	body.reset(ctx->space->create_body());
}

RoamingEntity::~RoamingEntity()
{
}

sz::Tag *RoamingEntity::save()
{
	sz::Tag *root, *tmp;
	root = Entity::save();
	tmp = new sz::List();
	sz::list_add(tmp, new sz::f64(body->get_p().x));
	sz::list_add(tmp, new sz::f64(body->get_p().y));
	sz::list_add(tmp, new sz::f64(body->get_p().z));
	sz::dict_add(root, "pos", tmp);
	tmp = new sz::List();
	sz::list_add(tmp, new sz::f64(body->get_r().x));
	sz::list_add(tmp, new sz::f64(body->get_r().y));
	sz::list_add(tmp, new sz::f64(body->get_r().z));
	sz::dict_add(root, "rot", tmp);
	tmp = new sz::List();
	sz::list_add(tmp, new sz::f64(body->get_v().x));
	sz::list_add(tmp, new sz::f64(body->get_v().y));
	sz::list_add(tmp, new sz::f64(body->get_v().z));
	sz::dict_add(root, "vel", tmp);
	return root;
}

void RoamingEntity::load(sz::Tag *root)
{
	Entity::load(root);

	sz::Tag *tag;
	tag = sz::dict_lookup(root, "pos");
	{
		v3f p;
		p.x = tag->get_list()[0]->get_f64();
		p.y = tag->get_list()[1]->get_f64();
		p.z = tag->get_list()[2]->get_f64();
		body->set_p(p);
	}
	tag = sz::dict_lookup(root, "rot");
	{
		v3f r;
		r.x = tag->get_list()[0]->get_f64();
		r.y = tag->get_list()[1]->get_f64();
		r.z = tag->get_list()[2]->get_f64();
		body->set_r(r);
	}
	tag = sz::dict_lookup(root, "vel");
	{
		v3f v;
		v.x = tag->get_list()[0]->get_f64();
		v.y = tag->get_list()[1]->get_f64();
		v.z = tag->get_list()[2]->get_f64();
		body->set_v(v);
	}
}
