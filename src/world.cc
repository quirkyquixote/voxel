/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "world.h"

#include "context.h"
#include "lighting.h"

World::World(Context *ctx)
	:ctx(ctx)
{
	for (auto p : box2ll(0, 0, CHUNK_NUM - 1, CHUNK_NUM - 1))
		chunks[p.x][p.y] = nullptr;
}

World::~World()
{
	for (auto p : box2ll(0, 0, CHUNK_NUM - 1, CHUNK_NUM - 1))
		if (chunks[p.x][p.y])
			delete chunks[p.x][p.y];
}

void World::load(serializer::Tag *root)
{
	serializer::Tag *tag;

	tag = serializer::dict_lookup(root, "x");
	p.x = tag->get_i64();
	tag = serializer::dict_lookup(root, "z");
	p.y = tag->get_i64();
}

serializer::Tag *World::save()
{
	serializer::Tag *root;
	root = new serializer::Dict();
	serializer::dict_add(root, "x", new serializer::i64(p.x));
	serializer::dict_add(root, "z", new serializer::i64(p.y));
	return root;
}

void World::set_block(const v3ll &p, int shape, int mat)
{
	box3ll bb;
	set_shape(p, shape);
	set_mat(p, mat);
	Entity *e = get_data(p);
	if (e != NULL)
		delete e;
	if (block_traits[mat][shape].entity != NULL) {
		Entity *e = block_traits[mat][shape].entity(ctx);
		e->set_p(p);
		set_data(p, e);
	} else {
		set_data(p, NULL);
	}
	ctx->light->update(box3ll(p.x, p.y, p.z, p.x, p.y, p.z), &bb);
	set_flags(bb, Chunk::UNRENDERED);
}

void World::set_flags(const box3ll &bb, int flags)
{
	for (auto p : (box2ll(bb.x0, bb.z0, bb.x1, bb.z1) >> 4LL) & 0x1fLL)
		chunks[p.x][p.y]->set_flags(flags);
}
