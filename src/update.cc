/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "update.h"

#include <map>

#include "context.h"
#include "recipes.h"
#include "lighting.h"
#include "drop_entity.h"

static const char has_left_side[256] = {
	0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0,
	1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1
};

static const char has_right_side[256] = {
	0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,
	1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1
};

static const char has_down_side[256] = {
	0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0,
	1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1
};

static const char has_up_side[256] = {
	0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
	1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1
};

static const char has_back_side[256] = {
	0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0,
	1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1
};

static const char has_front_side[256] = {
	0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0,
	1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1
};

static inline void update_face_lf(Context *ctx, std::vector<Vertex> *buf,
		int64_t x, int64_t y, int64_t z, const v2f *mt, const int *tilted)
{
	v2f lt;
	if (has_right_side[ctx->w->get_shape(v3ll(x - 1, y, z))])
		return;
	lt = texcoord_from_light(ctx->w->get_light(v3ll(x - 1, y, z)));
	vertices_add(buf, vertices_face_lf, 6, v3f(x, y, z), lt, mt, tilted);
}

static inline void update_face_rt(Context *ctx, std::vector<Vertex> *buf,
		int64_t x, int64_t y, int64_t z, const v2f *mt, const int *tilted)
{
	v2f lt;
	if (has_left_side[ctx->w->get_shape(v3ll(x + 1, y, z))])
		return;
	lt = texcoord_from_light(ctx->w->get_light(v3ll(x + 1, y, z)));
	vertices_add(buf, vertices_face_rt, 6, v3f(x, y, z), lt, mt, tilted);
}

static inline void update_face_dn(Context *ctx, std::vector<Vertex> *buf,
		int64_t x, int64_t y, int64_t z, const v2f *mt, const int *tilted)
{
	v2f lt;
	if (has_up_side[ctx->w->get_shape(v3ll(x, y - 1, z))])
		return;
	lt = texcoord_from_light(ctx->w->get_light(v3ll(x, y - 1, z)));
	vertices_add(buf, vertices_face_dn, 6, v3f(x, y, z), lt, mt, tilted);
}

static inline void update_face_up(Context *ctx, std::vector<Vertex> *buf,
		int64_t x, int64_t y, int64_t z, const v2f *mt, const int *tilted)
{
	v2f lt;
	if (has_down_side[ctx->w->get_shape(v3ll(x, y + 1, z))])
		return;
	lt = texcoord_from_light(ctx->w->get_light(v3ll(x, y + 1, z)));
	vertices_add(buf, vertices_face_up, 6, v3f(x, y, z), lt, mt, tilted);
}

static inline void update_face_bk(Context *ctx, std::vector<Vertex> *buf,
		int64_t x, int64_t y, int64_t z, const v2f *mt, const int *tilted)
{
	v2f lt;
	if (has_front_side[ctx->w->get_shape(v3ll(x, y, z - 1))])
		return;
	lt = texcoord_from_light(ctx->w->get_light(v3ll(x, y, z - 1)));
	vertices_add(buf, vertices_face_bk, 6, v3f(x, y, z), lt, mt, tilted);
}

static inline void update_face_ft(Context *ctx, std::vector<Vertex> *buf,
		int64_t x, int64_t y, int64_t z, const v2f *mt, const int *tilted)
{
	v2f lt;
	if (has_back_side[ctx->w->get_shape(v3ll(x, y, z + 1))])
		return;
	lt = texcoord_from_light(ctx->w->get_light(v3ll(x, y, z + 1)));
	vertices_add(buf, vertices_face_ft, 6, v3f(x, y, z), lt, mt, tilted);
}

void update_cell(Context *ctx, std::vector<Vertex> *buf, int64_t x, int64_t y,
		int64_t z)
{
	int8_t s, sl, sd, sb, sr, su, sf;
	v2f mt[6];
	int tilted[6];
	v2f lt;

	s = ctx->w->get_shape(v3ll(x, y, z));
	if (s == SHAPE_NONE)
		return;

	if (s == SHAPE_BLOCK_DN) {
		texcoord_up(ctx->w->get_mat(v3ll(x, y, z)), mt, tilted);
		update_face_lf(ctx, buf, x, y, z, mt, tilted);
		update_face_rt(ctx, buf, x, y, z, mt, tilted);
		update_face_dn(ctx, buf, x, y, z, mt, tilted);
		update_face_up(ctx, buf, x, y, z, mt, tilted);
		update_face_bk(ctx, buf, x, y, z, mt, tilted);
		update_face_ft(ctx, buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_BLOCK_UP) {
		texcoord_dn(ctx->w->get_mat(v3ll(x, y, z)), mt, tilted);
		update_face_lf(ctx, buf, x, y, z, mt, tilted);
		update_face_rt(ctx, buf, x, y, z, mt, tilted);
		update_face_dn(ctx, buf, x, y, z, mt, tilted);
		update_face_up(ctx, buf, x, y, z, mt, tilted);
		update_face_bk(ctx, buf, x, y, z, mt, tilted);
		update_face_ft(ctx, buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_BLOCK_LF) {
		texcoord_lf(ctx->w->get_mat(v3ll(x, y, z)), mt, tilted);
		update_face_lf(ctx, buf, x, y, z, mt, tilted);
		update_face_rt(ctx, buf, x, y, z, mt, tilted);
		update_face_dn(ctx, buf, x, y, z, mt, tilted);
		update_face_up(ctx, buf, x, y, z, mt, tilted);
		update_face_bk(ctx, buf, x, y, z, mt, tilted);
		update_face_ft(ctx, buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_BLOCK_RT) {
		texcoord_rt(ctx->w->get_mat(v3ll(x, y, z)), mt, tilted);
		update_face_lf(ctx, buf, x, y, z, mt, tilted);
		update_face_rt(ctx, buf, x, y, z, mt, tilted);
		update_face_dn(ctx, buf, x, y, z, mt, tilted);
		update_face_up(ctx, buf, x, y, z, mt, tilted);
		update_face_bk(ctx, buf, x, y, z, mt, tilted);
		update_face_ft(ctx, buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_BLOCK_BK) {
		texcoord_bk(ctx->w->get_mat(v3ll(x, y, z)), mt, tilted);
		update_face_lf(ctx, buf, x, y, z, mt, tilted);
		update_face_rt(ctx, buf, x, y, z, mt, tilted);
		update_face_dn(ctx, buf, x, y, z, mt, tilted);
		update_face_up(ctx, buf, x, y, z, mt, tilted);
		update_face_bk(ctx, buf, x, y, z, mt, tilted);
		update_face_ft(ctx, buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_BLOCK_FT) {
		texcoord_ft(ctx->w->get_mat(v3ll(x, y, z)), mt, tilted);
		update_face_lf(ctx, buf, x, y, z, mt, tilted);
		update_face_rt(ctx, buf, x, y, z, mt, tilted);
		update_face_dn(ctx, buf, x, y, z, mt, tilted);
		update_face_up(ctx, buf, x, y, z, mt, tilted);
		update_face_bk(ctx, buf, x, y, z, mt, tilted);
		update_face_ft(ctx, buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_SLAB_DN) {
		texcoord_up(ctx->w->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->w->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_slab_dn, 30, v3f(x, y, z), lt, mt, tilted);
		update_face_dn(ctx, buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_SLAB_UP) {
		texcoord_up(ctx->w->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->w->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_slab_up, 30, v3f(x, y, z), lt, mt, tilted);
		update_face_up(ctx, buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_SLAB_LF) {
		texcoord_up(ctx->w->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->w->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_slab_lf, 30, v3f(x, y, z), lt, mt, tilted);
		update_face_lf(ctx, buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_SLAB_RT) {
		texcoord_up(ctx->w->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->w->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_slab_rt, 30, v3f(x, y, z), lt, mt, tilted);
		update_face_rt(ctx, buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_SLAB_BK) {
		texcoord_up(ctx->w->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->w->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_slab_bk, 30, v3f(x, y, z), lt, mt, tilted);
		update_face_bk(ctx, buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_SLAB_FT) {
		texcoord_up(ctx->w->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->w->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_slab_ft, 30, v3f(x, y, z), lt, mt, tilted);
		update_face_ft(ctx, buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_STAIRS_DF) {
		texcoord_up(ctx->w->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->w->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_stairs_df, 42, v3f(x, y, z), lt, mt, tilted);
		update_face_dn(ctx, buf, x, y, z, mt, tilted);
		update_face_ft(ctx, buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_STAIRS_DL) {
		texcoord_up(ctx->w->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->w->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_stairs_dl, 42, v3f(x, y, z), lt, mt, tilted);
		update_face_dn(ctx, buf, x, y, z, mt, tilted);
		update_face_lf(ctx, buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_STAIRS_DB) {
		texcoord_up(ctx->w->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->w->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_stairs_db, 42, v3f(x, y, z), lt, mt, tilted);
		update_face_dn(ctx, buf, x, y, z, mt, tilted);
		update_face_bk(ctx, buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_STAIRS_DR) {
		texcoord_up(ctx->w->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->w->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_stairs_dr, 42, v3f(x, y, z), lt, mt, tilted);
		update_face_dn(ctx, buf, x, y, z, mt, tilted);
		update_face_rt(ctx, buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_STAIRS_UF) {
		texcoord_up(ctx->w->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->w->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_stairs_uf, 42, v3f(x, y, z), lt, mt, tilted);
		update_face_up(ctx, buf, x, y, z, mt, tilted);
		update_face_ft(ctx, buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_STAIRS_UL) {
		texcoord_up(ctx->w->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->w->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_stairs_ul, 42, v3f(x, y, z), lt, mt, tilted);
		update_face_up(ctx, buf, x, y, z, mt, tilted);
		update_face_lf(ctx, buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_STAIRS_UB) {
		texcoord_up(ctx->w->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->w->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_stairs_ub, 42, v3f(x, y, z), lt, mt, tilted);
		update_face_up(ctx, buf, x, y, z, mt, tilted);
		update_face_bk(ctx, buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_STAIRS_UR) {
		texcoord_up(ctx->w->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->w->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_stairs_ur, 42, v3f(x, y, z), lt, mt, tilted);
		update_face_up(ctx, buf, x, y, z, mt, tilted);
		update_face_rt(ctx, buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_PANE_Y) {
		texcoord_up(ctx->w->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->w->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_pane_y, 36, v3f(x, y, z), lt, mt, tilted);
	} else if (s == SHAPE_PANE_X) {
		texcoord_up(ctx->w->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->w->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_pane_x, 36, v3f(x, y, z), lt, mt, tilted);
	} else if (s == SHAPE_PANE_Z) {
		texcoord_up(ctx->w->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->w->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_pane_z, 36, v3f(x, y, z), lt, mt, tilted);
	} else if (s == SHAPE_PANE_DN) {
		texcoord_up(ctx->w->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->w->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_pane_dn, 30, v3f(x, y, z), lt, mt, tilted);
		update_face_dn(ctx, buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_PANE_UP) {
		texcoord_up(ctx->w->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->w->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_pane_up, 30, v3f(x, y, z), lt, mt, tilted);
		update_face_up(ctx, buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_PANE_LF) {
		texcoord_up(ctx->w->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->w->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_pane_lf, 30, v3f(x, y, z), lt, mt, tilted);
		update_face_lf(ctx, buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_PANE_RT) {
		texcoord_up(ctx->w->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->w->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_pane_rt, 30, v3f(x, y, z), lt, mt, tilted);
		update_face_rt(ctx, buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_PANE_BK) {
		texcoord_up(ctx->w->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->w->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_pane_bk, 30, v3f(x, y, z), lt, mt, tilted);
		update_face_bk(ctx, buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_PANE_FT) {
		texcoord_up(ctx->w->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->w->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_pane_ft, 30, v3f(x, y, z), lt, mt, tilted);
		update_face_ft(ctx, buf, x, y, z, mt, tilted);
	}
}

void update_vbo(Context *ctx, int id, int64_t x0, int64_t y0, int64_t z0)
{
	int64_t x1, y1, z1;
	int64_t x, y, z;
	std::vector<Vertex> buf;

	x1 = x0 + SHARD_W;
	y1 = y0 + SHARD_H;
	z1 = z0 + SHARD_D;

	for (x = x0; x < x1; ++x) {
		for (y = y0; y < y1; ++y) {
			for (z = z0; z < z1; ++z) {
				update_cell(ctx, &buf, x, y, z);
			}
		}
	}

	ctx->shard_vertex_buffer->update(id, buf.data(), buf.size());
}

void spill_inventory(Context *ctx, const v3ll &p)
{
	Entity *e;
	e = ctx->w->get_data(p);
	if (e == nullptr || e->get_items()->size() == 0)
		return;
	for (auto &s : *e->get_items()) {
		if (s.num) {
			DropEntity *d = new DropEntity(ctx, s);
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
			ctx->entities.push_back(d);
		}
	}
}

void use_inventory(Context *ctx, std::vector<Item> *items)
{
	v3ll p = ctx->cur.p;
	v3f q = ctx->cur.q;
	int side = sqrt(items->size());
	int i = side * floor(q.x * side) + floor(q.z * side);
	Item &s1 = ctx->inv[ctx->tool];
	Item &s2 = (*items)[i];
	if (ctx->act == 1) {
		if (ctx->move.y0) {
			if (s2.num == 0) {
				log_info("nothing to take");
				return;
			}
			int acc = inventory_add(&ctx->inv, s2);
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
	} else if (ctx->use == 1) {
		if (ctx->move.y0) {
			if (s2.num == 0) {
				log_info("nothing to take");
				return;
			}
			int acc = inventory_add(&ctx->inv, Item(s2.obj, s2.mat, 1));
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

void drop_block(Context *ctx, const v3ll &p)
{
	int s2, m2;
	Item s;
	s2 = ctx->w->get_shape(p);
	m2 = ctx->w->get_mat(p);
	s = block_traits[m2][s2].drop;
	if (s.num == 0)
		return;
	DropEntity *d = new DropEntity(ctx, s);
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
	ctx->entities.push_back(d);
}

void use_workbench(Context *ctx, std::vector<Item> *inv)
{
	if (ctx->act == 1) {
		Item s;
		if (recipe_match(inv, &s)) {
			int i = 0;
			do {
				inventory_add(&ctx->inv, s);
				++i;
			} while (recipe_match(inv, &s));
			log_info("take %s %s %d", mat_names[s.mat],
					obj_names[s.obj], s.num * i);
		} else {
			log_info("not a recipe");
		}
	} else if (ctx->use == 1) {
		Item s;
		if (recipe_match(inv, &s)) {
			inventory_add(&ctx->inv, s);
			log_info("take %s %s %d", mat_names[s.mat],
					obj_names[s.obj], s.num);
		} else {
			log_info("not a recipe");
		}
	}
}

void use_tool(Context *ctx)
{
	v3ll p = ctx->cur.p;
	v3f q = ctx->cur.q;
	int f = ctx->cur.face;
	Item &s = ctx->inv[ctx->tool];

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
	if (ctx->w->get_shape(p) != SHAPE_NONE)
		return;
	if (s.obj == OBJ_BLOCK) {
		if (ctx->move.y0) {
			if (f == FACE_UP)
				ctx->w->set_block(p, SHAPE_BLOCK_DN, s.mat);
			else if (f == FACE_DN)
				ctx->w->set_block(p, SHAPE_BLOCK_UP, s.mat);
			else if (f == FACE_LF)
				ctx->w->set_block(p, SHAPE_BLOCK_RT, s.mat);
			else if (f == FACE_RT)
				ctx->w->set_block(p, SHAPE_BLOCK_LF, s.mat);
			else if (f == FACE_BK)
				ctx->w->set_block(p, SHAPE_BLOCK_FT, s.mat);
			else if (f == FACE_FT)
				ctx->w->set_block(p, SHAPE_BLOCK_BK, s.mat);
		} else {
			ctx->w->set_block(p, SHAPE_BLOCK_DN, s.mat);
		}
	} else if (s.obj == OBJ_SLAB) {
		if (ctx->move.y0) {
			if (f == FACE_UP)
				ctx->w->set_block(p, SHAPE_SLAB_DN, s.mat);
			else if (f == FACE_DN)
				ctx->w->set_block(p, SHAPE_SLAB_UP, s.mat);
			else if (f == FACE_LF)
				ctx->w->set_block(p, SHAPE_SLAB_RT, s.mat);
			else if (f == FACE_RT)
				ctx->w->set_block(p, SHAPE_SLAB_LF, s.mat);
			else if (f == FACE_BK)
				ctx->w->set_block(p, SHAPE_SLAB_FT, s.mat);
			else if (f == FACE_FT)
				ctx->w->set_block(p, SHAPE_SLAB_BK, s.mat);
		} else {
			if (f == FACE_UP)
				ctx->w->set_block(p, SHAPE_SLAB_DN, s.mat);
			else if (f == FACE_DN)
				ctx->w->set_block(p, SHAPE_SLAB_UP, s.mat);
			else if (q.y > 0.5)
				ctx->w->set_block(p, SHAPE_SLAB_UP, s.mat);
			else
				ctx->w->set_block(p, SHAPE_SLAB_DN, s.mat);
		}
	} else if (s.obj == OBJ_STAIRS) {
		if (f == FACE_UP) {
			ctx->w->set_block(p,
					SHAPE_STAIRS_DF + (ctx->rot.y + 2) % 4, s.mat);
		} else if (f == FACE_DN) {
			ctx->w->set_block(p,
					SHAPE_STAIRS_UF + (ctx->rot.y + 2) % 4, s.mat);
		} else if (q.y > 0.5) {
			ctx->w->set_block(p,
					SHAPE_STAIRS_UF + (ctx->rot.y + 2) % 4, s.mat);
		} else {
			ctx->w->set_block(p,
					SHAPE_STAIRS_DF + (ctx->rot.y + 2) % 4, s.mat);
		}
	} else if (s.obj == OBJ_PANE) {
		if (ctx->move.y0) {
			if (f == FACE_UP)
				ctx->w->set_block(p, SHAPE_PANE_DN, s.mat);
			else if (f == FACE_DN)
				ctx->w->set_block(p, SHAPE_PANE_UP, s.mat);
			else if (f == FACE_LF)
				ctx->w->set_block(p, SHAPE_PANE_RT, s.mat);
			else if (f == FACE_RT)
				ctx->w->set_block(p, SHAPE_PANE_LF, s.mat);
			else if (f == FACE_BK)
				ctx->w->set_block(p, SHAPE_PANE_FT, s.mat);
			else if (f == FACE_FT)
				ctx->w->set_block(p, SHAPE_PANE_BK, s.mat);
		} else {
			if (ctx->rot.y & 1)
				ctx->w->set_block(p, SHAPE_PANE_X, s.mat);
			else
				ctx->w->set_block(p, SHAPE_PANE_Z, s.mat);
		}
/*	} else if (s.obj == OBJ_FLUID) {
		flowsim_add(ctx->flowsim, p, 1);*/
	}
	--s.num;
}

void update_player(Context *ctx)
{
	v3ll p = ctx->cur.p;
	int s = ctx->w->get_shape(p);
	int m = ctx->w->get_mat(p);
	Entity *e = ctx->w->get_data(p);

	if (e != nullptr && e->use())
			return;
	if (ctx->act == 1) {
		if (ctx->cur.face != -1) {
			if (s != SHAPE_NONE) {
				spill_inventory(ctx, p);
				drop_block(ctx, p);
				ctx->w->set_block(p, 0, 0);
			}
		}
	}
	if (ctx->use == 1) {
		if (ctx->cur.face != -1) {
			use_tool(ctx);
		}
	}
	if (ctx->pick == 1) {
		if (ctx->cur.face != -1) {
			int s = ctx->w->get_shape(ctx->cur.p);
			int m = ctx->w->get_mat(ctx->cur.p);
			int o;
			if (s >= SHAPE_BLOCK_DN && s <= SHAPE_BLOCK_FT)
				o = OBJ_BLOCK;
			else if (s >= SHAPE_SLAB_DN && s <= SHAPE_SLAB_FT)
				o = OBJ_SLAB;
			else if (s >= SHAPE_PANE_X && s <= SHAPE_PANE_Z)
				o = OBJ_PANE;
			else if (s >= SHAPE_STAIRS_DF && s <= SHAPE_STAIRS_UR)
				o = OBJ_STAIRS;
			ctx->inv[ctx->tool] = Item(o, m, 1);
		}
	}
}

void update_camera(Context *ctx)
{
	int w, h;
	int x, y;
	int buttons;
	v3f r, v;

	SDL_GetWindowSize(ctx->ml->get_window()->get_sdl_window(), &w, &h);
	buttons = SDL_GetMouseState(&x, &y);
	SDL_WarpMouseInWindow(ctx->ml->get_window()->get_sdl_window(), w / 2, h / 2);

	r = ctx->player->get_r();
	r.y += (x - w / 2) * .005;
	r.x -= (y - h / 2) * .005;
	if (r.x < -M_PI_2 * .99)
		r.x = -M_PI_2 * .99;
	else if (r.x > M_PI_2 * .99)
		r.x = M_PI_2 * .99;
	ctx->player->set_r(r);

	v = ctx->player->get_v();
	if (ctx->move.x0 == 0 && ctx->move.x1 == 0 && ctx->move.z0 == 0
			&& ctx->move.z1 == 0)
		ctx->run = 0;
	v.x = (ctx->move.x1 - ctx->move.x0) * (ctx->run ? .18 : 0.15);
	v.y += (ctx->move.y1 - ctx->move.y0) * (ctx->run ? .18 : 0.15);
	v.z = (ctx->move.z1 - ctx->move.z0) * (ctx->run ? .18 : 0.15);
	v = roty(v, r.y);
	ctx->player->set_v(v);
	ctx->player->set_step_size(1 + ctx->run);

	ctx->cam->set_fovy((ctx->cam->get_fovy() + (ctx->run ? 70.f : 60.f)) * 0.5);
	ctx->cam->set_p(ctx->player->get_p() + v3f(0, .8, 0));
	ctx->cam->set_r(ctx->player->get_r());

	ctx->rot.x = (unsigned int)floor(0.5 + r.x / M_PI_2) & 3;
	ctx->rot.y = (unsigned int)floor(0.5 + r.y / M_PI_2) & 3;

	v = v3f(0, 0, -5);
	v = rotx(v, r.x);
	v = roty(v, r.y);
	ctx->space->query(ctx->cam->get_p(), v, &ctx->cur);
}

void update_chunks(Context *ctx)
{
	int x, z;
	Chunk *c;
	std::map<int, Chunk *> out_of_date;
	box2ll bb;
	v3ll m;

	v2ll p(ctx->player->get_p().x, ctx->player->get_p().z);
	bb.x0 = floor(p.x) - CHUNK_W * CHUNKS_PER_WORLD / 2;
	bb.y0 = floor(p.y) - CHUNK_D * CHUNKS_PER_WORLD / 2;
	bb.x1 = bb.x0 + CHUNK_W * CHUNKS_PER_WORLD;
	bb.y1 = bb.y0 + CHUNK_D * CHUNKS_PER_WORLD;
	m.x = CHUNK_W * CHUNKS_PER_WORLD * floor(bb.x1 / (CHUNK_W * CHUNKS_PER_WORLD));
	m.y = CHUNK_D * CHUNKS_PER_WORLD * floor(bb.y1 / (CHUNK_D * CHUNKS_PER_WORLD));

	for (x = 0; x < CHUNKS_PER_WORLD; ++x) {
		for (z = 0; z < CHUNKS_PER_WORLD; ++z) {
			c = ctx->w->get_chunk(v2ll(x, z));
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
		if (i >= ctx->chunks_per_tick)
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
			update_lighting(ctx->w, box3ll(c->get_p().x, 0, c->get_p().y,
						c->get_p().x + CHUNK_W, CHUNK_H, c->get_p().y + CHUNK_D), NULL);
			c->unset_flags(CHUNK_UNLIT);
			c->set_flags(CHUNK_UNRENDERED);
		}
		if ((c->get_flags() & CHUNK_UNRENDERED) != 0) {
//			log_info("update vbos");
			for (int k = 0; k < SHARDS_PER_CHUNK; ++k)
				update_vbo(ctx, c->get_shard(k)->get_id(), c->get_p().x,
						k * SHARD_H, c->get_p().y);
			c->unset_flags(CHUNK_UNRENDERED);
		}
		++i;
	}
//		log_info("update %d of %zd chunks (max: %d)", i, out_of_date.size(), ctx->chunks_per_tick);
}

void update_entities(Context *ctx)
{
	for (auto &e : ctx->entities)
		e->update();
	ctx->entities.remove_if([](RoamingEntity *e){ return e->get_die(); });
}

void update(Context *ctx)
{
	v3ll p;

	if (!ctx->ml->get_window()->has_mouse_focus())
		return;
	p.x = floor(ctx->cam->get_p().x);
	p.y = floor(ctx->cam->get_p().y);
	p.z = floor(ctx->cam->get_p().z);
	ctx->space->run();
/*	if ((ctx->tick & 1) == 0)
		flowsim_step(ctx->flowsim);*/
	update_player(ctx);
	update_camera(ctx);
	if (ctx->act > 0) {
		if (++ctx->act >= 8)
			ctx->act = 1;
	}
	if (ctx->use > 0) {
		if (++ctx->use >= 8)
			ctx->use = 1;
	}
	if (ctx->pick > 0) {
		if (++ctx->pick >= 8)
			ctx->pick = 1;
	}
	update_chunks(ctx);
	update_entities(ctx);
	ctx->tone_mapper->update((ctx->w->get_light(p) << 4) / 255., 0);
	++ctx->tick;
}
