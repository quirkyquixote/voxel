
#include "entity.h"

#include "crate_entity.h"
#include "bench_entity.h"
#include "pipe_entity.h"
#include "board_entity.h"
#include "context.h"

Entity *load_entity(Context *ctx, sz_Tag *root)
{
	auto &kv = root->get_dict()[0];
	Entity *e;
	if (strcmp(kv.first, "name") != 0) {
		log_error("first entity tag must be name; got %s", kv.first);
		throw sz_Exception();
	}
	if (strcmp(kv.second->get_str(), "crate") == 0) {
		e = new CrateEntity(ctx);
	} else if (strcmp(kv.second->get_str(), "bench") == 0) {
		e = new BenchEntity(ctx);
	} else if (strcmp(kv.second->get_str(), "board") == 0) {
		e = new BoardEntity(ctx);
	} else if (strcmp(kv.second->get_str(), "pipe") == 0) {
		e = new PipeEntity(ctx);
	} else {
		log_error("bad entity type: %s", kv.second->get_str());
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
	for (auto &it : root->get_dict()) {
		if (strcmp(it.first, "p") == 0) {
			p.x = it.second->get_list()[0]->get_i64();
			p.y = it.second->get_list()[1]->get_i64();
			p.z = it.second->get_list()[2]->get_i64();
		} else if (strcmp(it.first, "items") == 0) {
			memcpy(items.data(), it.second->get_raw().data(), sizeof(Item) * items.size());
		}
	}
}
