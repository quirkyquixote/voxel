
#include "update.h"

#include "context.h"
#include "recipes.h"
#include "lighting.h"

int chunks_by_priority(const void *p1, const void *p2)
{
	const struct chunk *c1, *c2;
	c1 = *(const void **)p1;
	c2 = *(const void **)p2;
	return c1->priority - c2->priority;
}

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

static inline void update_face_lf(struct context *ctx, struct array *buf,
		int64_t x, int64_t y, int64_t z, const struct v2f *mt, const int *tilted)
{
	struct v2f lt;
	if (has_right_side[world_get_shape(ctx->w, v3ll(x - 1, y, z))])
		return;
	lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x - 1, y, z)));
	vertices_add(buf, vertices_face_lf, 6, v3f(x, y, z), lt, mt, tilted);
}

static inline void update_face_rt(struct context *ctx, struct array *buf,
		int64_t x, int64_t y, int64_t z, const struct v2f *mt, const int *tilted)
{
	struct v2f lt;
	if (has_left_side[world_get_shape(ctx->w, v3ll(x + 1, y, z))])
		return;
	lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x + 1, y, z)));
	vertices_add(buf, vertices_face_rt, 6, v3f(x, y, z), lt, mt, tilted);
}

static inline void update_face_dn(struct context *ctx, struct array *buf,
		int64_t x, int64_t y, int64_t z, const struct v2f *mt, const int *tilted)
{
	struct v2f lt;
	if (has_up_side[world_get_shape(ctx->w, v3ll(x, y - 1, z))])
		return;
	lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x, y - 1, z)));
	vertices_add(buf, vertices_face_dn, 6, v3f(x, y, z), lt, mt, tilted);
}

static inline void update_face_up(struct context *ctx, struct array *buf,
		int64_t x, int64_t y, int64_t z, const struct v2f *mt, const int *tilted)
{
	struct v2f lt;
	if (has_down_side[world_get_shape(ctx->w, v3ll(x, y + 1, z))])
		return;
	lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x, y + 1, z)));
	vertices_add(buf, vertices_face_up, 6, v3f(x, y, z), lt, mt, tilted);
}

static inline void update_face_bk(struct context *ctx, struct array *buf,
		int64_t x, int64_t y, int64_t z, const struct v2f *mt, const int *tilted)
{
	struct v2f lt;
	if (has_front_side[world_get_shape(ctx->w, v3ll(x, y, z - 1))])
		return;
	lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x, y, z - 1)));
	vertices_add(buf, vertices_face_bk, 6, v3f(x, y, z), lt, mt, tilted);
}

static inline void update_face_ft(struct context *ctx, struct array *buf,
		int64_t x, int64_t y, int64_t z, const struct v2f *mt, const int *tilted)
{
	struct v2f lt;
	if (has_back_side[world_get_shape(ctx->w, v3ll(x, y, z + 1))])
		return;
	lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x, y, z + 1)));
	vertices_add(buf, vertices_face_ft, 6, v3f(x, y, z), lt, mt, tilted);
}

void update_cell(struct context *ctx, struct array *buf, int64_t x, int64_t y,
		int64_t z)
{
	int8_t s, sl, sd, sb, sr, su, sf;
	struct v2f mt[6];
	int tilted[6];
	struct v2f lt;

	s = world_get_shape(ctx->w, v3ll(x, y, z));
	if (s == SHAPE_NONE)
		return;

	if (s == SHAPE_BLOCK_DN) {
		texcoord_up(world_get_mat(ctx->w, v3ll(x, y, z)), mt, tilted);
		update_face_lf(ctx, buf, x, y, z, mt, tilted);
		update_face_rt(ctx, buf, x, y, z, mt, tilted);
		update_face_dn(ctx, buf, x, y, z, mt, tilted);
		update_face_up(ctx, buf, x, y, z, mt, tilted);
		update_face_bk(ctx, buf, x, y, z, mt, tilted);
		update_face_ft(ctx, buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_BLOCK_UP) {
		texcoord_dn(world_get_mat(ctx->w, v3ll(x, y, z)), mt, tilted);
		update_face_lf(ctx, buf, x, y, z, mt, tilted);
		update_face_rt(ctx, buf, x, y, z, mt, tilted);
		update_face_dn(ctx, buf, x, y, z, mt, tilted);
		update_face_up(ctx, buf, x, y, z, mt, tilted);
		update_face_bk(ctx, buf, x, y, z, mt, tilted);
		update_face_ft(ctx, buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_BLOCK_LF) {
		texcoord_lf(world_get_mat(ctx->w, v3ll(x, y, z)), mt, tilted);
		update_face_lf(ctx, buf, x, y, z, mt, tilted);
		update_face_rt(ctx, buf, x, y, z, mt, tilted);
		update_face_dn(ctx, buf, x, y, z, mt, tilted);
		update_face_up(ctx, buf, x, y, z, mt, tilted);
		update_face_bk(ctx, buf, x, y, z, mt, tilted);
		update_face_ft(ctx, buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_BLOCK_RT) {
		texcoord_rt(world_get_mat(ctx->w, v3ll(x, y, z)), mt, tilted);
		update_face_lf(ctx, buf, x, y, z, mt, tilted);
		update_face_rt(ctx, buf, x, y, z, mt, tilted);
		update_face_dn(ctx, buf, x, y, z, mt, tilted);
		update_face_up(ctx, buf, x, y, z, mt, tilted);
		update_face_bk(ctx, buf, x, y, z, mt, tilted);
		update_face_ft(ctx, buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_BLOCK_BK) {
		texcoord_bk(world_get_mat(ctx->w, v3ll(x, y, z)), mt, tilted);
		update_face_lf(ctx, buf, x, y, z, mt, tilted);
		update_face_rt(ctx, buf, x, y, z, mt, tilted);
		update_face_dn(ctx, buf, x, y, z, mt, tilted);
		update_face_up(ctx, buf, x, y, z, mt, tilted);
		update_face_bk(ctx, buf, x, y, z, mt, tilted);
		update_face_ft(ctx, buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_BLOCK_FT) {
		texcoord_ft(world_get_mat(ctx->w, v3ll(x, y, z)), mt, tilted);
		update_face_lf(ctx, buf, x, y, z, mt, tilted);
		update_face_rt(ctx, buf, x, y, z, mt, tilted);
		update_face_dn(ctx, buf, x, y, z, mt, tilted);
		update_face_up(ctx, buf, x, y, z, mt, tilted);
		update_face_bk(ctx, buf, x, y, z, mt, tilted);
		update_face_ft(ctx, buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_SLAB_DN) {
		texcoord_up(world_get_mat(ctx->w, v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x, y, z)));
		vertices_add(buf, vertices_slab_dn, 30, v3f(x, y, z), lt, mt, tilted);
		update_face_dn(ctx, buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_SLAB_UP) {
		texcoord_up(world_get_mat(ctx->w, v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x, y, z)));
		vertices_add(buf, vertices_slab_up, 30, v3f(x, y, z), lt, mt, tilted);
		update_face_up(ctx, buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_SLAB_LF) {
		texcoord_up(world_get_mat(ctx->w, v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x, y, z)));
		vertices_add(buf, vertices_slab_lf, 30, v3f(x, y, z), lt, mt, tilted);
		update_face_lf(ctx, buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_SLAB_RT) {
		texcoord_up(world_get_mat(ctx->w, v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x, y, z)));
		vertices_add(buf, vertices_slab_rt, 30, v3f(x, y, z), lt, mt, tilted);
		update_face_rt(ctx, buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_SLAB_BK) {
		texcoord_up(world_get_mat(ctx->w, v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x, y, z)));
		vertices_add(buf, vertices_slab_bk, 30, v3f(x, y, z), lt, mt, tilted);
		update_face_bk(ctx, buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_SLAB_FT) {
		texcoord_up(world_get_mat(ctx->w, v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x, y, z)));
		vertices_add(buf, vertices_slab_ft, 30, v3f(x, y, z), lt, mt, tilted);
		update_face_ft(ctx, buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_STAIRS_DF) {
		texcoord_up(world_get_mat(ctx->w, v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x, y, z)));
		vertices_add(buf, vertices_stairs_df, 42, v3f(x, y, z), lt, mt, tilted);
		update_face_dn(ctx, buf, x, y, z, mt, tilted);
		update_face_ft(ctx, buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_STAIRS_DL) {
		texcoord_up(world_get_mat(ctx->w, v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x, y, z)));
		vertices_add(buf, vertices_stairs_dl, 42, v3f(x, y, z), lt, mt, tilted);
		update_face_dn(ctx, buf, x, y, z, mt, tilted);
		update_face_lf(ctx, buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_STAIRS_DB) {
		texcoord_up(world_get_mat(ctx->w, v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x, y, z)));
		vertices_add(buf, vertices_stairs_db, 42, v3f(x, y, z), lt, mt, tilted);
		update_face_dn(ctx, buf, x, y, z, mt, tilted);
		update_face_bk(ctx, buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_STAIRS_DR) {
		texcoord_up(world_get_mat(ctx->w, v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x, y, z)));
		vertices_add(buf, vertices_stairs_dr, 42, v3f(x, y, z), lt, mt, tilted);
		update_face_dn(ctx, buf, x, y, z, mt, tilted);
		update_face_rt(ctx, buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_STAIRS_UF) {
		texcoord_up(world_get_mat(ctx->w, v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x, y, z)));
		vertices_add(buf, vertices_stairs_uf, 42, v3f(x, y, z), lt, mt, tilted);
		update_face_up(ctx, buf, x, y, z, mt, tilted);
		update_face_ft(ctx, buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_STAIRS_UL) {
		texcoord_up(world_get_mat(ctx->w, v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x, y, z)));
		vertices_add(buf, vertices_stairs_ul, 42, v3f(x, y, z), lt, mt, tilted);
		update_face_up(ctx, buf, x, y, z, mt, tilted);
		update_face_lf(ctx, buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_STAIRS_UB) {
		texcoord_up(world_get_mat(ctx->w, v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x, y, z)));
		vertices_add(buf, vertices_stairs_ub, 42, v3f(x, y, z), lt, mt, tilted);
		update_face_up(ctx, buf, x, y, z, mt, tilted);
		update_face_bk(ctx, buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_STAIRS_UR) {
		texcoord_up(world_get_mat(ctx->w, v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x, y, z)));
		vertices_add(buf, vertices_stairs_ur, 42, v3f(x, y, z), lt, mt, tilted);
		update_face_up(ctx, buf, x, y, z, mt, tilted);
		update_face_rt(ctx, buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_PANE_Y) {
		texcoord_up(world_get_mat(ctx->w, v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x, y, z)));
		vertices_add(buf, vertices_pane_y, 36, v3f(x, y, z), lt, mt, tilted);
	} else if (s == SHAPE_PANE_X) {
		texcoord_up(world_get_mat(ctx->w, v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x, y, z)));
		vertices_add(buf, vertices_pane_x, 36, v3f(x, y, z), lt, mt, tilted);
	} else if (s == SHAPE_PANE_Z) {
		texcoord_up(world_get_mat(ctx->w, v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x, y, z)));
		vertices_add(buf, vertices_pane_z, 36, v3f(x, y, z), lt, mt, tilted);
	}
}

void update_vbo(struct context *ctx, int id, int64_t x0, int64_t y0, int64_t z0)
{
	int64_t x1, y1, z1;
	int64_t x, y, z;
	struct array *buf;

	x1 = x0 + SHARD_W;
	y1 = y0 + SHARD_H;
	z1 = z0 + SHARD_D;
	buf = array(sizeof(struct vertex));

	for (x = x0; x < x1; ++x) {
		for (y = y0; y < y1; ++y) {
			for (z = z0; z < z1; ++z) {
				update_cell(ctx, buf, x, y, z);
			}
		}
	}

	vertex_buffer_update(ctx->shard_vertex_buffer, id, buf->data,
			buf->size);
	array_destroy(buf);
}

void use_inventory(struct context *ctx, struct array *inv)
{
	if (inv == NULL) {
		log_warning("no inventory found");
		return;
	}
	struct v3ll p = ctx->cur.p;
	struct v3f q = ctx->cur.q;
	int side = sqrt(inv->size);
	int i = side * floor(q.x * side) + floor(q.z * side);
	struct slot s1 = inventory_get(ctx->inv, ctx->tool);
	struct slot s2 = inventory_get(inv, i);
	if (ctx->act == 1) {
		if (ctx->move.y0) {
			if (s2.num == 0) {
				log_info("nothing to take");
				return;
			}
			int acc = inventory_add(ctx->inv, s2);
			inventory_set_num(inv, i, s2.num - acc);
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
			inventory_set(inv, i, s1);
			inventory_set(ctx->inv, ctx->tool, s2);
		}
	} else if (ctx->use == 1) {
		if (ctx->move.y0) {
			if (s2.num == 0) {
				log_info("nothing to take");
				return;
			}
			int acc =
				inventory_add(ctx->inv, slot(s2.obj, s2.mat, 1));
			inventory_set_num(inv, i, s2.num - acc);
			if (acc == 0)
				log_info("no space to take");
			else
				log_info("take %s %s 1", mat_names[s1.mat],
						obj_names[s1.obj], acc);
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
				inventory_set_obj(inv, i, s1.obj);
				inventory_set_mat(inv, i, s1.mat);
			} else if (s1.obj != s2.obj || s1.mat != s2.mat) {
				log_info("not the same object");
				return;
			}
			inventory_set_num(inv, i, s2.num + 1);
			inventory_set_num(ctx->inv, ctx->tool, s1.num - 1);
			log_info("left %s %s 1", mat_names[s1.mat],
					obj_names[s1.obj]);
		}
	}
}

void use_workbench(struct context *ctx, struct array *inv)
{
	if (ctx->act == 1) {
		struct slot s;
		if (recipe_match(inv, &s)) {
			int i = 0;
			do {
				inventory_add(ctx->inv, s);
				++i;
			} while (recipe_match(inv, &s));
			log_info("take %s %s %d", mat_names[s.mat],
					obj_names[s.obj], s.num * i);
		} else {
			log_info("not a recipe");
		}
	} else if (ctx->use == 1) {
		struct slot s;
		if (recipe_match(inv, &s)) {
			inventory_add(ctx->inv, s);
			log_info("take %s %s %d", mat_names[s.mat],
					obj_names[s.obj], s.num);
		} else {
			log_info("not a recipe");
		}
	}
}

void use_tool(struct context *ctx)
{
	struct v3ll p = ctx->cur.p;
	struct v3f q = ctx->cur.q;
	int f = ctx->cur.face;
	struct slot s = inventory_get(ctx->inv, ctx->tool);

	if (s.num == 0)
		return;
	if (f == FACE_LF)
		p = v3_add(p, v3c(-1, 0, 0));
	else if (f == FACE_RT)
		p = v3_add(p, v3c(1, 0, 0));
	else if (f == FACE_DN)
		p = v3_add(p, v3c(0, -1, 0));
	else if (f == FACE_UP)
		p = v3_add(p, v3c(0, 1, 0));
	else if (f == FACE_BK)
		p = v3_add(p, v3c(0, 0, -1));
	else if (f == FACE_FT)
		p = v3_add(p, v3c(0, 0, 1));
	if (world_get_shape(ctx->w, p) != SHAPE_NONE)
		return;
	if (s.obj == OBJ_BLOCK) {
		if (ctx->move.y0) {
			if (f == FACE_UP)
				world_set(ctx->w, p, SHAPE_BLOCK_DN, s.mat);
			else if (f == FACE_DN)
				world_set(ctx->w, p, SHAPE_BLOCK_UP, s.mat);
			else if (f == FACE_LF)
				world_set(ctx->w, p, SHAPE_BLOCK_RT, s.mat);
			else if (f == FACE_RT)
				world_set(ctx->w, p, SHAPE_BLOCK_LF, s.mat);
			else if (f == FACE_BK)
				world_set(ctx->w, p, SHAPE_BLOCK_FT, s.mat);
			else if (f == FACE_FT)
				world_set(ctx->w, p, SHAPE_BLOCK_BK, s.mat);
		} else {
			world_set(ctx->w, p, SHAPE_BLOCK_DN, s.mat);
		}
	} else if (s.obj == OBJ_SLAB) {
		if (ctx->move.y0) {
			if (f == FACE_UP)
				world_set(ctx->w, p, SHAPE_SLAB_DN, s.mat);
			else if (f == FACE_DN)
				world_set(ctx->w, p, SHAPE_SLAB_UP, s.mat);
			else if (f == FACE_LF)
				world_set(ctx->w, p, SHAPE_SLAB_RT, s.mat);
			else if (f == FACE_RT)
				world_set(ctx->w, p, SHAPE_SLAB_LF, s.mat);
			else if (f == FACE_BK)
				world_set(ctx->w, p, SHAPE_SLAB_FT, s.mat);
			else if (f == FACE_FT)
				world_set(ctx->w, p, SHAPE_SLAB_BK, s.mat);
		} else {
			if (f == FACE_UP)
				world_set(ctx->w, p, SHAPE_SLAB_DN, s.mat);
			else if (f == FACE_DN)
				world_set(ctx->w, p, SHAPE_SLAB_UP, s.mat);
			else if (q.y > 0.5)
				world_set(ctx->w, p, SHAPE_SLAB_UP, s.mat);
			else
				world_set(ctx->w, p, SHAPE_SLAB_DN, s.mat);
		}
	} else if (s.obj == OBJ_STAIRS) {
		if (f == FACE_UP) {
			world_set(ctx->w, p,
					SHAPE_STAIRS_DF + (ctx->roty + 2) % 4, s.mat);
		} else if (f == FACE_DN) {
			world_set(ctx->w, p,
					SHAPE_STAIRS_UF + (ctx->roty + 2) % 4, s.mat);
		} else if (q.y > 0.5) {
			world_set(ctx->w, p,
					SHAPE_STAIRS_UF + (ctx->roty + 2) % 4, s.mat);
		} else {
			world_set(ctx->w, p,
					SHAPE_STAIRS_DF + (ctx->roty + 2) % 4, s.mat);
		}
	} else if (s.obj == OBJ_PANE) {
		if (ctx->roty & 1)
			world_set(ctx->w, p, SHAPE_PANE_X, s.mat);
		else
			world_set(ctx->w, p, SHAPE_PANE_Z, s.mat);
	} else if (s.obj == OBJ_FLUID) {
		flowsim_add(ctx->flowsim, p, 1);
	}
}

void update_player(struct context *ctx)
{
	struct v3ll p = ctx->cur.p;
	int s = world_get_shape(ctx->w, p);
	int m = world_get_mat(ctx->w, p);

	if (ctx->bench != NULL && v3_dist(ctx->bench_p, ctx->player->p) > 4) {
		array_destroy(ctx->bench);
		ctx->bench = NULL;
	}

	if (ctx->cur.face == FACE_UP) {
		if (ctx->bench != NULL && v3_eql(ctx->cur.p, ctx->bench_p)) {
			use_inventory(ctx, ctx->bench);
			return;
		}
		if (block_traits[m][s].is_inventory) {
			use_inventory(ctx, world_get_data(ctx->w, p));
			return;
		}
	}
	if (ctx->bench != NULL && v3_eql(ctx->cur.p, ctx->bench_p)) {
		use_workbench(ctx, ctx->bench);
		return;
	}
	if (block_traits[m][s].is_workbench) {
		use_workbench(ctx, world_get_data(ctx->w, p));
		return;
	}
	if (ctx->act == 1) {
		if (ctx->cur.face != -1) {
			if (s != SHAPE_NONE) {
				world_set(ctx->w, p, 0, 0);
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
			int s = world_get_shape(ctx->w, ctx->cur.p);
			int m = world_get_mat(ctx->w, ctx->cur.p);
			int o;
			if (s >= SHAPE_BLOCK_DN && s <= SHAPE_BLOCK_FT)
				o = OBJ_BLOCK;
			else if (s >= SHAPE_SLAB_DN && s <= SHAPE_SLAB_FT)
				o = OBJ_SLAB;
			else if (s >= SHAPE_PANE_X && s <= SHAPE_PANE_Z)
				o = OBJ_PANE;
			else if (s >= SHAPE_STAIRS_DF && s <= SHAPE_STAIRS_UR)
				o = OBJ_STAIRS;
			inventory_set(ctx->inv, ctx->tool, slot(o, m, 1));
		}
	}
}

void update_camera(struct context *ctx)
{
	int w, h;
	int x, y;
	int buttons;
	struct v3f r, v;

	SDL_GetWindowSize(ctx->ml->windows->sdl_window, &w, &h);
	buttons = SDL_GetMouseState(&x, &y);
	SDL_WarpMouseInWindow(ctx->ml->windows->sdl_window, w / 2, h / 2);

	r = body_get_rotation(ctx->player);
	r.y += (x - w / 2) * .005;
	r.x -= (y - h / 2) * .005;
	if (r.x < -M_PI_2 * .99)
		r.x = -M_PI_2 * .99;
	else if (r.x > M_PI_2 * .99)
		r.x = M_PI_2 * .99;
	body_set_rotation(ctx->player, r);

	v = body_get_velocity(ctx->player);
	if (ctx->move.x0 == 0 && ctx->move.x1 == 0 && ctx->move.z0 == 0
			&& ctx->move.z1 == 0)
		ctx->run = 0;
	v.x = (ctx->move.x1 - ctx->move.x0) * (ctx->run ? .18 : 0.15);
	v.y += (ctx->move.y1 - ctx->move.y0) * (ctx->run ? .18 : 0.15);
	v.z = (ctx->move.z1 - ctx->move.z0) * (ctx->run ? .18 : 0.15);
	v = v3_roty(v, r.y);
	body_set_velocity(ctx->player, v);
	body_set_step_size(ctx->player, 1 + ctx->run);

	camera_set_fovy(ctx->cam,
			(ctx->cam->fovy + (ctx->run ? 70.f : 60.f)) * 0.5);
	camera_set_position(ctx->cam, v3_add(ctx->player->p, v3f(0, .8, 0)));
	camera_set_rotation(ctx->cam, ctx->player->r);

	ctx->rotx = (unsigned int)floor(0.5 + r.x / M_PI_2) & 3;
	ctx->roty = (unsigned int)floor(0.5 + r.y / M_PI_2) & 3;

	v = v3f(0, 0, -5);
	v = v3_rotx(v, r.x);
	v = v3_roty(v, r.y);
	space_query(ctx->space, ctx->cam->p, v, &ctx->cur);
}

void update_chunks(struct context *ctx)
{
	int x, z, i, j, k;
	struct chunk *c;
	struct chunk *out_of_date[CHUNKS_PER_WORLD * CHUNKS_PER_WORLD];
	struct box2ll bb;
	struct v3ll m;

	bb.x0 = floor(ctx->player->p.x) - CHUNK_W * CHUNKS_PER_WORLD / 2;
	bb.y0 = floor(ctx->player->p.z) - CHUNK_D * CHUNKS_PER_WORLD / 2;
	bb.x1 = bb.x0 + CHUNK_W * CHUNKS_PER_WORLD;
	bb.y1 = bb.y0 + CHUNK_D * CHUNKS_PER_WORLD;
	m.x =
		CHUNK_W * CHUNKS_PER_WORLD * floor(bb.x1 / (CHUNK_W *
					CHUNKS_PER_WORLD));
	m.y =
		CHUNK_D * CHUNKS_PER_WORLD * floor(bb.y1 / (CHUNK_D *
					CHUNKS_PER_WORLD));

	i = 0;
	for (x = 0; x < CHUNKS_PER_WORLD; ++x) {
		for (z = 0; z < CHUNKS_PER_WORLD; ++z) {
			c = ctx->w->chunks[x][z];	/*
							   if (c->x < bb.x0 || c->x >= bb.x1 || c->z < bb.y0 || c->z >= bb.y1) {
							   c->up_to_date = 0;
							   c->x = m.x + x * CHUNK_W;
							   if (c->x >= bb.x1)
							   c->x -= CHUNK_W * CHUNKS_PER_WORLD;
							   c->z = m.y + z * CHUNK_D;
							   if (c->z >= bb.y1)
							   c->z -= CHUNK_D * CHUNKS_PER_WORLD;
							   if (load_chunk(c, ctx->dir) != 0)
							   terraform(0, c);
							   } */
			if (c->flags != 0) {
				c->priority =
					hypot((double)c->x - ctx->player->p.x,
							(double)c->z - ctx->player->p.z);
				out_of_date[i++] = c;
			}
		}
	}
	if (i == 0)
		return;
	qsort(out_of_date, i, sizeof(*out_of_date), chunks_by_priority);
	i = i < ctx->chunks_per_tick ? i : ctx->chunks_per_tick;
	for (j = 0; j < i; ++j) {
		c = out_of_date[j];
		//      log_info("Update chunk %d (%d,%d); priority:%d", c->id, c->x, c->z, c->priority);
		if ((c->flags & CHUNK_UNLOADED) != 0) {
			//      log_info("load from file");
			/* load this chunk */
			c->flags ^= CHUNK_UNLOADED;
		}
		if ((c->flags & CHUNK_UNLIT) != 0) {
			//      log_info("lit up");
			update_lighting(ctx->w, box3ll(c->x, 0, c->z,
						c->x + CHUNK_W, CHUNK_H, c->z + CHUNK_D), NULL);
			c->flags ^= CHUNK_UNLIT;
			c->flags |= CHUNK_UNRENDERED;
		}
		if ((c->flags & CHUNK_UNRENDERED) != 0) {
			//      log_info("update vbos");
			for (k = 0; k < SHARDS_PER_CHUNK; ++k)
				update_vbo(ctx, c->shards[k]->id, c->x,
						k * SHARD_H, c->z);
			c->flags ^= CHUNK_UNRENDERED;
		}
	}
}

void update_entities(struct context *ctx)
{
	struct entity *e;

	list_foreach(e, &ctx->entities, entities) {
		e->traits->update_func(e);
	}
	list_foreach_safe(e, &ctx->entities, entities) {
		if (e->die)
			e->traits->destroy_func(e);
	}
}

void update(void *data)
{
	struct context *ctx = data;
	struct v3ll p;
	struct drop *d;

	if (!window_has_mouse_focus(ctx->ml->windows))
		return;
	p.x = floor(ctx->cam->p.x);
	p.y = floor(ctx->cam->p.y);
	p.z = floor(ctx->cam->p.z);
	space_run(ctx->space);
	if ((ctx->tick & 1) == 0)
		flowsim_step(ctx->flowsim);
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
	tone_mapper_update(ctx->tone_mapper, (world_get_light(ctx->w,
					p) << 4) / 255., 0);
	++ctx->tick;
}
