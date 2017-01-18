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
	sz_list_add(tmp, new sz_f64(body->get_p().y));
	sz_dict_add(root, "pos", tmp);
	tmp = new sz_List();
	sz_list_add(tmp, new sz_f64(body->get_r().x));
	sz_list_add(tmp, new sz_f64(body->get_r().y));
	sz_list_add(tmp, new sz_f64(body->get_r().y));
	sz_dict_add(root, "rot", tmp);
	tmp = new sz_List();
	sz_list_add(tmp, new sz_f64(body->get_v().x));
	sz_list_add(tmp, new sz_f64(body->get_v().y));
	sz_list_add(tmp, new sz_f64(body->get_v().y));
	sz_dict_add(root, "vel", tmp);
	return root;
}

void RoamingEntity::load(sz_Tag *root)
{
	Entity::load(root);
	for (auto &it : root->get_dict()) {
		if (strcmp(it.first, "pos") == 0) {
			v3f p;
			p.x = it.second->get_list()[0]->get_f64();
			p.y = it.second->get_list()[1]->get_f64();
			p.z = it.second->get_list()[2]->get_f64();
			body->set_p(p);
		} else if (strcmp(it.first, "rot") == 0) {
			v3f r;
			r.x = it.second->get_list()[0]->get_f64();
			r.y = it.second->get_list()[1]->get_f64();
			r.z = it.second->get_list()[2]->get_f64();
			body->set_r(r);
		} else if (strcmp(it.first, "vel") == 0) {
			v3f v;
			v.x = it.second->get_list()[0]->get_f64();
			v.y = it.second->get_list()[1]->get_f64();
			v.z = it.second->get_list()[2]->get_f64();
			body->set_v(v);
		}
	}
}
