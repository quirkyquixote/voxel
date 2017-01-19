/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "roaming_entity.h"

#include "context.h"

RoamingEntity::RoamingEntity(Context *ctx, int capacity)
	: Entity(ctx, capacity), die(false)
{
	body = ctx->space->create_body();
}

RoamingEntity::~RoamingEntity()
{
	ctx->space->destroy_body(body);
}

sz_Tag *RoamingEntity::save()
{
	sz_Tag *root, *tmp;
	root = Entity::save();
	tmp = new sz_List();
	sz_list_add(tmp, new sz_f64(body->get_p().x));
	sz_list_add(tmp, new sz_f64(body->get_p().y));
	sz_list_add(tmp, new sz_f64(body->get_p().z));
	sz_dict_add(root, "pos", tmp);
	tmp = new sz_List();
	sz_list_add(tmp, new sz_f64(body->get_r().x));
	sz_list_add(tmp, new sz_f64(body->get_r().y));
	sz_list_add(tmp, new sz_f64(body->get_r().z));
	sz_dict_add(root, "rot", tmp);
	tmp = new sz_List();
	sz_list_add(tmp, new sz_f64(body->get_v().x));
	sz_list_add(tmp, new sz_f64(body->get_v().y));
	sz_list_add(tmp, new sz_f64(body->get_v().z));
	sz_dict_add(root, "vel", tmp);
	return root;
}

void RoamingEntity::load(sz_Tag *root)
{
	Entity::load(root);

	sz_Tag *tag;
	tag = sz_dict_lookup(root, "pos");
	{
		v3f p;
		p.x = tag->get_list()[0]->get_f64();
		p.y = tag->get_list()[1]->get_f64();
		p.z = tag->get_list()[2]->get_f64();
		body->set_p(p);
	}
	tag = sz_dict_lookup(root, "rot");
	{
		v3f r;
		r.x = tag->get_list()[0]->get_f64();
		r.y = tag->get_list()[1]->get_f64();
		r.z = tag->get_list()[2]->get_f64();
		body->set_r(r);
	}
	tag = sz_dict_lookup(root, "vel");
	{
		v3f v;
		v.x = tag->get_list()[0]->get_f64();
		v.y = tag->get_list()[1]->get_f64();
		v.z = tag->get_list()[2]->get_f64();
		body->set_v(v);
	}
}
