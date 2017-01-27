/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "entity.h"

#include "crate_entity.h"
#include "bench_entity.h"
#include "pipe_entity.h"
#include "board_entity.h"
#include "context.h"

Entity *load_entity(Context *ctx, sz::Tag *root)
{
	sz::Tag *tag;
	Entity *e;

	tag = sz::dict_lookup(root, "name");
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
		throw sz::Exception();
	}
	e->load(root);
	return e;
}

sz::Tag *Entity::save()
{
	sz::Tag *root, *tmp;
	root = new sz::Dict();
	sz::dict_add(root, "name", new sz::Str(get_name()));
	tmp = new sz::List();
	sz::list_add(tmp, new sz::i64(p.x));
	sz::list_add(tmp, new sz::i64(p.y));
	sz::list_add(tmp, new sz::i64(p.z));
	sz::dict_add(root, "p", tmp);
	if (!items.empty()) {
		tmp = new sz::Raw(items.data(), sizeof(Item) * items.size());
		sz::dict_add(root, "items", tmp);
	}
	return root;
}

void Entity::load(sz::Tag *root)
{
	sz::Tag *tag;

	tag = sz::dict_lookup(root, "p");
	p.x = tag->get_list()[0]->get_i64();
	p.y = tag->get_list()[1]->get_i64();
	p.z = tag->get_list()[2]->get_i64();

	try {
		tag = sz::dict_lookup(root, "items");
		memcpy(items.data(), tag->get_raw().data(), sizeof(Item) * items.size());
	} catch (sz::Exception &ex) {
		/* do nothing */
	}
}
