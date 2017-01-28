/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "entity.h"

#include "crate_entity.h"
#include "bench_entity.h"
#include "pipe_entity.h"
#include "board_entity.h"
#include "context.h"

Entity *load_entity(Context *ctx, serializer::Tag *root)
{
	serializer::Tag *tag;
	Entity *e;

	tag = serializer::dict_lookup(root, "name");
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
		throw serializer::Exception();
	}
	e->load(root);
	return e;
}

serializer::Tag *Entity::save()
{
	serializer::Tag *root, *tmp;
	root = new serializer::Dict();
	serializer::dict_add(root, "name", new serializer::Str(get_name()));
	tmp = new serializer::List();
	serializer::list_add(tmp, new serializer::i64(p.x));
	serializer::list_add(tmp, new serializer::i64(p.y));
	serializer::list_add(tmp, new serializer::i64(p.z));
	serializer::dict_add(root, "p", tmp);
	if (!items.empty()) {
		tmp = new serializer::Raw(items.data(), sizeof(Item) * items.size());
		serializer::dict_add(root, "items", tmp);
	}
	return root;
}

void Entity::load(serializer::Tag *root)
{
	serializer::Tag *tag;

	tag = serializer::dict_lookup(root, "p");
	p.x = tag->get_list()[0]->get_i64();
	p.y = tag->get_list()[1]->get_i64();
	p.z = tag->get_list()[2]->get_i64();

	try {
		tag = serializer::dict_lookup(root, "items");
		memcpy(items.data(), tag->get_raw().data(), sizeof(Item) * items.size());
	} catch (serializer::Exception &ex) {
		/* do nothing */
	}
}
