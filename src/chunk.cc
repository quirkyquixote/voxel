/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "chunk.h"

#include <string.h>
#include <stdio.h>

#include "sz.h"
#include "lighting.h"
#include "inventory.h"
#include "context.h"
#include "block_entity.h"

Shard::Shard(int id, int y)
	: id(id), y(y)
{
}

Shard::~Shard()
{
}

void Shard::load(sz_Tag *root)
{
	memset(data, 0, sizeof(data));
	for (auto &it : root->get_dict()) {
		if (strcmp(it.first, "y") == 0) {
			y = it.second->get_i64();
		} else if (strcmp(it.first, "mat") == 0) {
			memcpy(mat, it.second->get_raw().data(), sizeof(mat));
		} else if (strcmp(it.first, "shape") == 0) {
			memcpy(shape, it.second->get_raw().data(), sizeof(shape));
		} else if (strcmp(it.first, "light") == 0) {
			memcpy(light, it.second->get_raw().data(), sizeof(light));
		} else {
			log_error("bad tag: %s", it.first);
		}
	}
}

sz_Tag *Shard::save()
{
	sz_Tag *root = new sz_Dict();
	sz_dict_add(root, "y", new sz_i64(y));
	sz_dict_add(root, "mat", new sz_Raw(mat, SHARD_VOLUME));
	sz_dict_add(root, "shape", new sz_Raw(shape, SHARD_VOLUME));
	sz_dict_add(root, "light", new sz_Raw(light, SHARD_VOLUME));
	return root;
}

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

World::World(Context *ctx)
	:ctx(ctx)
{
	int x, z, id;
	id = 0;
	for (x = 0; x < CHUNKS_PER_WORLD; ++x)
		for (z = 0; z < CHUNKS_PER_WORLD; ++z)
			chunks[x][z] = new Chunk(ctx, id++);
}

World::~World()
{
	int x, z;
	for (x = 0; x < CHUNKS_PER_WORLD; ++x)
		for (z = 0; z < CHUNKS_PER_WORLD; ++z)
			delete chunks[x][z];
}

void World::load(sz_Tag *root)
{
	for (auto &it : root->get_dict()) {
		if (strcmp(it.first, "x") == 0) {
			p.x = it.second->get_i64();
		} else if (strcmp(it.first, "z") == 0) {
			p.y = it.second->get_i64();
		} else {
			log_error("bad tag: %s", it.first);
		}
	}
}

sz_Tag *World::save()
{
	sz_Tag *root;
	root = new sz_Dict();
	sz_dict_add(root, "x", new sz_i64(p.x));
	sz_dict_add(root, "z", new sz_i64(p.y));
	return root;
}

void World::set_block(const v3ll &p, int shape, int mat)
{
	box3ll bb;
	set_shape(p, shape);
	set_mat(p, mat);
	if (block_traits[mat][shape].entity != NULL) {
		Entity *e = block_traits[mat][shape].entity(ctx);
		e->set_p(p);
		set_data(p, e);
	} else {
		set_data(p, NULL);
	}
	update_lighting(this, box3ll(p.x, p.y, p.z, p.x + 1, p.y + 1, p.z + 1), &bb);
	set_flags(bb, CHUNK_UNRENDERED);
}

void World::set_flags(const box3ll &bb, int flags)
{
	box2ll cb((bb.x0 >> 4) & 0xf, (bb.z0 >> 4) & 0xf,
			(bb.x1 >> 4) & 0xf, (bb.z1 >> 4) & 0xf);
	for (int x = cb.x0; x <= cb.x1; ++x) {
		for (int z = cb.y0; z <= cb.y1; ++z) {
			chunks[x][z]->set_flags(flags);
		}
	}
}
