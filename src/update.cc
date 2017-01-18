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

void Context::use_inventory(std::vector<Item> *items)
{
	v3ll p = cur.p;
	v3f q = cur.q;
	int side = sqrt(items->size());
	int i = side * floor(q.x * side) + floor(q.z * side);
	Item &s1 = inv[tool];
	Item &s2 = (*items)[i];
	if (act == 1) {
		if (move.y0) {
			if (s2.num == 0) {
				log_info("nothing to take");
				return;
			}
			int acc = inventory_add(&inv, s2);
			s2.num -= acc;
			if (acc == 0)
				log_info("no space to take");
			else
				log_info("take %s %s %d", mat_names[s1.mat],
						obj_names[s1.obj], acc);
		} else {
			if (s2.num)
				log_info("take %s %s %d", mat_names[s2.mat],
						obj_names[s2.obj], s2.num);
			if (s1.num)
				log_info("left %s %s %d", mat_names[s1.mat],
						obj_names[s1.obj], s1.num);
			std::swap(s1, s2);
		}
	} else if (use == 1) {
		if (move.y0) {
			if (s2.num == 0) {
				log_info("nothing to take");
				return;
			}
			int acc = inventory_add(&inv, Item(s2.obj, s2.mat, 1));
			s2.num -= acc;
			if (acc == 0)
				log_info("no space to take");
			else
				log_info("take %s %s 1", mat_names[s1.mat],
						obj_names[s1.obj]);
		} else {
			if (s1.num == 0) {
				log_info("nothing to leave");
				return;
			}
			if (s2.num >= 64) {
				log_info("no space to leave");
				return;
			}
			if (s2.num == 0) {
				s2 = Item(s1.obj, s1.mat, 0);
			} else if (s1.obj != s2.obj || s1.mat != s2.mat) {
				log_info("not the same object");
				return;
			}
			++s2.num;
			--s1.num;
			log_info("left %s %s 1", mat_names[s1.mat],
					obj_names[s1.obj]);
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

void Context::use_workbench(std::vector<Item> *items)
{
	if (act == 1) {
		Item s;
		if (recipe_match(items, &s)) {
			int i = 0;
			do {
				inventory_add(&inv, s);
				++i;
			} while (recipe_match(items, &s));
			log_info("take %s %s %d", mat_names[s.mat],
					obj_names[s.obj], s.num * i);
		} else {
			log_info("not a recipe");
		}
	} else if (use == 1) {
		Item s;
		if (recipe_match(items, &s)) {
			inventory_add(&inv, s);
			log_info("take %s %s %d", mat_names[s.mat],
					obj_names[s.obj], s.num);
		} else {
			log_info("not a recipe");
		}
	}
}

void Context::use_tool()
{
	v3ll p = cur.p;
	v3f q = cur.q;
	int f = cur.face;
	Item &s = inv[tool];

	if (s.num == 0)
		return;
	if (f == FACE_LF)
		p = p + v3ll(-1, 0, 0);
	else if (f == FACE_RT)
		p = p + v3ll(1, 0, 0);
	else if (f == FACE_DN)
		p = p + v3ll(0, -1, 0);
	else if (f == FACE_UP)
		p = p + v3ll(0, 1, 0);
	else if (f == FACE_BK)
		p = p + v3ll(0, 0, -1);
	else if (f == FACE_FT)
		p = p + v3ll(0, 0, 1);
	if (world->get_shape(p) != SHAPE_NONE)
		return;
	if (s.obj == OBJ_BLOCK) {
		if (move.y0) {
			if (f == FACE_UP)
				world->set_block(p, SHAPE_BLOCK_DN, s.mat);
			else if (f == FACE_DN)
				world->set_block(p, SHAPE_BLOCK_UP, s.mat);
			else if (f == FACE_LF)
				world->set_block(p, SHAPE_BLOCK_RT, s.mat);
			else if (f == FACE_RT)
				world->set_block(p, SHAPE_BLOCK_LF, s.mat);
			else if (f == FACE_BK)
				world->set_block(p, SHAPE_BLOCK_FT, s.mat);
			else if (f == FACE_FT)
				world->set_block(p, SHAPE_BLOCK_BK, s.mat);
		} else {
			world->set_block(p, SHAPE_BLOCK_DN, s.mat);
		}
	} else if (s.obj == OBJ_SLAB) {
		if (move.y0) {
			if (f == FACE_UP)
				world->set_block(p, SHAPE_SLAB_DN, s.mat);
			else if (f == FACE_DN)
				world->set_block(p, SHAPE_SLAB_UP, s.mat);
			else if (f == FACE_LF)
				world->set_block(p, SHAPE_SLAB_RT, s.mat);
			else if (f == FACE_RT)
				world->set_block(p, SHAPE_SLAB_LF, s.mat);
			else if (f == FACE_BK)
				world->set_block(p, SHAPE_SLAB_FT, s.mat);
			else if (f == FACE_FT)
				world->set_block(p, SHAPE_SLAB_BK, s.mat);
		} else {
			if (f == FACE_UP)
				world->set_block(p, SHAPE_SLAB_DN, s.mat);
			else if (f == FACE_DN)
				world->set_block(p, SHAPE_SLAB_UP, s.mat);
			else if (q.y > 0.5)
				world->set_block(p, SHAPE_SLAB_UP, s.mat);
			else
				world->set_block(p, SHAPE_SLAB_DN, s.mat);
		}
	} else if (s.obj == OBJ_STAIRS) {
		if (f == FACE_UP) {
			world->set_block(p,
					SHAPE_STAIRS_DF + (rot.y + 2) % 4, s.mat);
		} else if (f == FACE_DN) {
			world->set_block(p,
					SHAPE_STAIRS_UF + (rot.y + 2) % 4, s.mat);
		} else if (q.y > 0.5) {
			world->set_block(p,
					SHAPE_STAIRS_UF + (rot.y + 2) % 4, s.mat);
		} else {
			world->set_block(p,
					SHAPE_STAIRS_DF + (rot.y + 2) % 4, s.mat);
		}
	} else if (s.obj == OBJ_PANE) {
		if (move.y0) {
			if (f == FACE_UP)
				world->set_block(p, SHAPE_PANE_DN, s.mat);
			else if (f == FACE_DN)
				world->set_block(p, SHAPE_PANE_UP, s.mat);
			else if (f == FACE_LF)
				world->set_block(p, SHAPE_PANE_RT, s.mat);
			else if (f == FACE_RT)
				world->set_block(p, SHAPE_PANE_LF, s.mat);
			else if (f == FACE_BK)
				world->set_block(p, SHAPE_PANE_FT, s.mat);
			else if (f == FACE_FT)
				world->set_block(p, SHAPE_PANE_BK, s.mat);
		} else {
			if (rot.y & 1)
				world->set_block(p, SHAPE_PANE_X, s.mat);
			else
				world->set_block(p, SHAPE_PANE_Z, s.mat);
		}
/*	} else if (s.obj == OBJ_FLUID) {
		flowsim_add(flowsim, p, 1);*/
	}
	--s.num;
}

void Context::update_player()
{
	int w, h;
	int x, y;
	int buttons;
	v3f r, v;

	SDL_GetWindowSize(ml->get_window()->get_sdl_window(), &w, &h);
	buttons = SDL_GetMouseState(&x, &y);
	SDL_WarpMouseInWindow(ml->get_window()->get_sdl_window(), w / 2, h / 2);

	r = player->get_r();
	r.y += (x - w / 2) * .005;
	r.x -= (y - h / 2) * .005;
	if (r.x < -M_PI_2 * .99)
		r.x = -M_PI_2 * .99;
	else if (r.x > M_PI_2 * .99)
		r.x = M_PI_2 * .99;
	player->set_r(r);

	v = player->get_v();
	if (move.x0 == 0 && move.x1 == 0 && move.z0 == 0
			&& move.z1 == 0)
		run = 0;
	v.x = (move.x1 - move.x0) * (run ? .18 : 0.15);
	v.y += (move.y1 - move.y0) * (run ? .18 : 0.15);
	v.z = (move.z1 - move.z0) * (run ? .18 : 0.15);
	v = roty(v, r.y);
	player->set_v(v);
	player->set_step_size(1 + run);

	rot.x = (unsigned int)floor(0.5 + r.x / M_PI_2) & 3;
	rot.y = (unsigned int)floor(0.5 + r.y / M_PI_2) & 3;

	v = v3f(0, 0, -5);
	v = rotx(v, r.x);
	v = roty(v, r.y);
	space->query(renderer->get_cam()->get_p(), v, &cur);

	v3ll p = cur.p;
	int s = world->get_shape(p);
	int m = world->get_mat(p);
	Entity *e = world->get_data(p);

	if (e != nullptr && e->use())
			return;
	if (act == 1) {
		if (cur.face != -1) {
			if (s != SHAPE_NONE) {
				spill_inventory(p);
				drop_block(p);
				world->set_block(p, 0, 0);
			}
		}
	}
	if (use == 1) {
		if (cur.face != -1) {
			use_tool();
		}
	}
	if (pick == 1) {
		if (cur.face != -1) {
			int s = world->get_shape(cur.p);
			int m = world->get_mat(cur.p);
			int o;
			if (s >= SHAPE_BLOCK_DN && s <= SHAPE_BLOCK_FT)
				o = OBJ_BLOCK;
			else if (s >= SHAPE_SLAB_DN && s <= SHAPE_SLAB_FT)
				o = OBJ_SLAB;
			else if (s >= SHAPE_PANE_X && s <= SHAPE_PANE_Z)
				o = OBJ_PANE;
			else if (s >= SHAPE_STAIRS_DF && s <= SHAPE_STAIRS_UR)
				o = OBJ_STAIRS;
			inv[tool] = Item(o, m, 1);
		}
	}
}

void Context::update_chunks()
{
	int x, z;
	Chunk *c;
	std::map<int, Chunk *> out_of_date;
	box2ll bb;
	v3ll m;

	v2ll p(player->get_p().x, player->get_p().z);
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
			update_lighting(world, box3ll(c->get_p().x, 0, c->get_p().y,
						c->get_p().x + CHUNK_W, CHUNK_H, c->get_p().y + CHUNK_D), NULL);
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
	update_player();
	renderer->update_camera();
	if (act > 0) {
		if (++act >= 8)
			act = 1;
	}
	if (use > 0) {
		if (++use >= 8)
			use = 1;
	}
	if (pick > 0) {
		if (++pick >= 8)
			pick = 1;
	}
	update_chunks();
	update_entities();
	++tick;
}
