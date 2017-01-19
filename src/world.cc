/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "world.h"

#include "context.h"
#include "lighting.h"

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
	sz_Tag *tag;

	tag = sz_dict_lookup(root, "x");
	p.x = tag->get_i64();
	tag = sz_dict_lookup(root, "z");
	p.y = tag->get_i64();
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
	ctx->light->update(box3ll(p.x, p.y, p.z, p.x + 1, p.y + 1, p.z + 1), &bb);
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
