/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "entity.h"

#include "crate_entity.h"
#include "bench_entity.h"
#include "pipe_entity.h"
#include "board_entity.h"
#include "context.h"

Entity *load_entity(Context *ctx, sz_Tag *root)
{
	sz_Tag *tag;
	Entity *e;

	tag = sz_dict_lookup(root, "name");
	if (strcmp(tag->get_str(), "crate") == 0) {
		e = new CrateEntity(ctx);
	} else if (strcmp(tag->get_str(), "bench") == 0) {
		e = new BenchEntity(ctx);
	} else if (strcmp(tag->get_str(), "board") == 0) {
		e = new BoardEntity(ctx);
	} else if (strcmp(tag->get_str(), "pipe") == 0) {
		e = new PipeEntity(ctx);
	} else {
		log_error("bad entity type: %s", tag->get_str());
		throw sz_Exception();
	}
	e->load(root);
	return e;
}

sz_Tag *Entity::save()
{
	sz_Tag *root, *tmp;
	root = new sz_Dict();
	sz_dict_add(root, "name", new sz_Str(get_name()));
	tmp = new sz_List();
	sz_list_add(tmp, new sz_i64(p.x));
	sz_list_add(tmp, new sz_i64(p.y));
	sz_list_add(tmp, new sz_i64(p.z));
	sz_dict_add(root, "p", tmp);
	if (!items.empty()) {
		tmp = new sz_Raw(items.data(), sizeof(Item) * items.size());
		sz_dict_add(root, "items", tmp);
	}
	return root;
}

void Entity::load(sz_Tag *root)
{
	sz_Tag *tag;

	tag = sz_dict_lookup(root, "p");
	p.x = tag->get_list()[0]->get_i64();
	p.y = tag->get_list()[1]->get_i64();
	p.z = tag->get_list()[2]->get_i64();

	try {
		tag = sz_dict_lookup(root, "items");
		memcpy(items.data(), tag->get_raw().data(), sizeof(Item) * items.size());
	} catch (sz_Exception &ex) {
		/* do nothing */
	}
}
