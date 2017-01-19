/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include <map>

#include "context.h"
#include "recipes.h"
#include "lighting.h"
#include "drop_entity.h"

void Context::spill_inventory(const v3ll &p)
{
	Entity *e;
	e = world->get_data(p);
	if (e == nullptr || e->get_items()->size() == 0)
		return;
	for (auto &s : *e->get_items()) {
		if (s.num) {
			DropEntity *d = new DropEntity(this, s);
			v3f q(p);
			q.x += (float)rand() / RAND_MAX;
			q.y += (float)rand() / RAND_MAX;
			q.z += (float)rand() / RAND_MAX;
			d->get_body()->set_p(q);
			v3f v(0, 0, 0);
			v.x += .1 * ((float)rand() / RAND_MAX - .5);
			v.y += .2 * ((float)rand() / RAND_MAX);
			v.z += .1 * ((float)rand() / RAND_MAX - .5);
			d->get_body()->set_v(v);
			entities.push_back(d);
		}
	}
}

void Context::drop_block(const v3ll &p)
{
	int s2, m2;
	Item s;
	s2 = world->get_shape(p);
	m2 = world->get_mat(p);
	s = block_traits[m2][s2].drop;
	if (s.num == 0)
		return;
	DropEntity *d = new DropEntity(this, s);
	v3f q(p);
	q.x += (float)rand() / RAND_MAX;
	q.y += (float)rand() / RAND_MAX;
	q.z += (float)rand() / RAND_MAX;
	d->get_body()->set_p(q);
	v3f v(0, 0, 0);
	v.x += .1 * ((float)rand() / RAND_MAX - .5);
	v.y += .2 * ((float)rand() / RAND_MAX);
	v.z += .1 * ((float)rand() / RAND_MAX - .5);
	d->get_body()->set_v(v);
	entities.push_back(d);
}

void Context::update_chunks()
{
	int x, z;
	Chunk *c;
	std::map<int, Chunk *> out_of_date;
	box2ll bb;
	v3ll m;

	v2ll p(player->get_body()->get_p().x, player->get_body()->get_p().z);
	bb.x0 = floor(p.x) - CHUNK_W * CHUNKS_PER_WORLD / 2;
	bb.y0 = floor(p.y) - CHUNK_D * CHUNKS_PER_WORLD / 2;
	bb.x1 = bb.x0 + CHUNK_W * CHUNKS_PER_WORLD;
	bb.y1 = bb.y0 + CHUNK_D * CHUNKS_PER_WORLD;
	m.x = CHUNK_W * CHUNKS_PER_WORLD * floor(bb.x1 / (CHUNK_W * CHUNKS_PER_WORLD));
	m.y = CHUNK_D * CHUNKS_PER_WORLD * floor(bb.y1 / (CHUNK_D * CHUNKS_PER_WORLD));

	for (x = 0; x < CHUNKS_PER_WORLD; ++x) {
		for (z = 0; z < CHUNKS_PER_WORLD; ++z) {
			c = world->get_chunk(v2ll(x, z));
			if (c->get_flags() != 0) {
				int d = dist(c->get_p(), p);
				out_of_date.emplace(d, c);
			}
		}
	}
	if (out_of_date.empty()) {
//		log_info("no chunks to update");
		return;
	}

	int i = 0;
	for (auto &iter : out_of_date) {
		if (i >= chunks_per_tick)
			break;
		c = iter.second;
//		log_info("Update chunk %d (%lld,%lld); priority:%d", c->get_id(), c->get_p().x, c->get_p().y, iter.first);
		if ((c->get_flags() & CHUNK_UNLOADED) != 0) {
//			log_info("load from file");
			/* load this chunk */
			c->unset_flags(CHUNK_UNLOADED);
		}
		if ((c->get_flags() & CHUNK_UNLIT) != 0) {
//			log_info("lit up");
			box3ll bb;
			bb.x0 = c->get_p().x;
			bb.y0 = 0;
			bb.z0 = c->get_p().y;
			bb.x1 = c->get_p().x + CHUNK_W;
			bb.y1 = CHUNK_H;
			bb.z1 = c->get_p().y + CHUNK_D;
			light->update(bb, NULL);
			c->unset_flags(CHUNK_UNLIT);
			c->set_flags(CHUNK_UNRENDERED);
		}
		if ((c->get_flags() & CHUNK_UNRENDERED) != 0) {
//			log_info("update vbos");
			for (int k = 0; k < SHARDS_PER_CHUNK; ++k)
				renderer->update_shard(c->get_shard(k)->get_id(), c->get_p().x,
						k * SHARD_H, c->get_p().y);
			c->unset_flags(CHUNK_UNRENDERED);
		}
		++i;
	}
//		log_info("update %d of %zd chunks (max: %d)", i, out_of_date.size(), chunks_per_tick);
}

void Context::update_entities()
{
	for (auto &e : entities)
		e->update();
	entities.remove_if([](RoamingEntity *e){ return e->get_die(); });
}

void Context::update()
{
	v3ll p;

	if (!ml->get_window()->has_mouse_focus())
		return;
	p.x = floor(renderer->get_cam()->get_p().x);
	p.y = floor(renderer->get_cam()->get_p().y);
	p.z = floor(renderer->get_cam()->get_p().z);
	space->run();
/*	if ((tick & 1) == 0)
		flowsim_step(flowsim);*/
	player->update();
	renderer->update_camera();
	update_chunks();
	update_entities();
	++tick;
}
