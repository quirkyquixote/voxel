/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "chunk.h"

#include <string.h>
#include <stdio.h>

#include "sz.h"
#include "context.h"

Chunk::Chunk(Context *ctx, const v2ll &p)
	: ctx(ctx), x(p.x), z(p.y)
{
	id = ((x >> 4) & 0x1f) + (((z >> 4) & 0x1f) << 5);
	for (int i = 0; i < SHARD_NUM; ++i)
		shards[i] = new Shard(id * SHARD_NUM + i, i);
	flags = UNLOADED;
}

Chunk::~Chunk()
{
	for (int i = 0; i < SHARD_NUM; ++i)
		delete shards[i];
}

void Chunk::load(sz_Tag *root)
{
	sz_Tag *tag;

	tag = sz_dict_lookup(root, "x");
	assert(x == tag->get_i64());
	tag = sz_dict_lookup(root, "z");
	assert(z == tag->get_i64());
	tag = sz_dict_lookup(root, "shards");
	{
		int i = 0;
		for (auto &it2 : tag->get_list())
			shards[i++]->load(it2);
	}
	tag = sz_dict_lookup(root, "entities");
	{
		for (auto &it2 : tag->get_list()) {
			Entity *e = load_entity(ctx, it2);
			auto p = e->get_p() - v3ll(x, 0, z);
			if (p.x < 0 || p.x >= W || p.y < 0 || p.y >= H || p.z < 0 || p.z >= D) {
				log_warning("chunk contains entity outide itself");
				delete e;
			} else {
				set_data(p, e);
			}
		}
	}
	flags = UNRENDERED;
}

sz_Tag *Chunk::save()
{
	sz_Tag *root, *tmp;
	root = new sz_Dict();
	sz_dict_add(root, "x", new sz_i64(x));
	sz_dict_add(root, "z", new sz_i64(z));
	tmp = new sz_List();
	for (int i = 0; i < SHARD_NUM; ++i) {
		sz_list_add(tmp, shards[i]->save());
	}
	sz_dict_add(root, "shards", tmp);
	tmp = new sz_List();
	for (auto &p : box3ll(0, 0, 0, W - 1, H - 1, D - 1)) {
		Entity *e = get_data(p);
		if (e != NULL)
			sz_list_add(tmp, e->save());
	}
	sz_dict_add(root, "entities", tmp);
	return root;
}

