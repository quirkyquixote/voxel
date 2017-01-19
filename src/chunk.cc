/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "chunk.h"

#include <string.h>
#include <stdio.h>

#include "sz.h"
#include "lighting.h"
#include "inventory.h"
#include "context.h"
#include "block_entity.h"

Chunk::Chunk(Context *ctx, int id)
	: ctx(ctx), id(id)
{
	for (int i = 0; i < SHARDS_PER_CHUNK; ++i)
		shards[i] = new Shard(id * SHARDS_PER_CHUNK + i, i);
	flags = CHUNK_UNLOADED;
}

Chunk::~Chunk()
{
	for (int i = 0; i < SHARDS_PER_CHUNK; ++i)
		delete shards[i];
}

void Chunk::load(sz_Tag *root)
{
	for (auto &it : root->get_dict()) {
		if (strcmp(it.first, "x") == 0) {
			x = it.second->get_i64();
		} else if (strcmp(it.first, "z") == 0) {
			z = it.second->get_i64();
		} else if (strcmp(it.first, "shards") == 0) {
			int i = 0;
			for (auto &it2 : it.second->get_list())
				shards[i++]->load(it2);
		} else if (strcmp(it.first, "entities") == 0) {
			for (auto &it2 : it.second->get_list()) {
				Entity *e = load_entity(ctx, it2);
				ctx->world->set_data(e->get_p(), e);
			}
		} else {
			log_error("bad tag: %s", it.first);
		}
	}
	flags = CHUNK_UNRENDERED;
}

sz_Tag *Chunk::save()
{
	sz_Tag *root, *tmp;
	box3ll bb(0, 0, 0, CHUNK_W, CHUNK_H, CHUNK_D);
	root = new sz_Dict();
	sz_dict_add(root, "x", new sz_i64(x));
	sz_dict_add(root, "z", new sz_i64(z));
	tmp = new sz_List();
	for (int i = 0; i < SHARDS_PER_CHUNK; ++i) {
		sz_list_add(tmp, shards[i]->save());
	}
	sz_dict_add(root, "shards", tmp);
	tmp = new sz_List();
	for (auto &p : bb) {
		Entity *e = get_data(p);
		if (e != NULL)
			sz_list_add(tmp, e->save());
	}
	sz_dict_add(root, "entities", tmp);
	return root;
}

