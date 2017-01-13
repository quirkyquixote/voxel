
#include "update.h"

#include "context.h"
#include "recipes.h"


int chunks_by_priority(const void *p1, const void *p2)
{
	const struct chunk *c1, *c2;
	c1 = *(const void **)p1;
	c2 = *(const void **)p2;
	return c1->priority - c2->priority;
}

static const struct v2f texcoord_from_mat[][3] = {
	{ { 0 / 16., 0 / 16. }, { 0 / 16., 1 / 16. }, { 0 / 16., 2 / 16. } },
	{ { 1 / 16., 0 / 16. }, { 1 / 16., 1 / 16. }, { 1 / 16., 2 / 16. } },
	{ { 2 / 16., 0 / 16. }, { 2 / 16., 1 / 16. }, { 2 / 16., 2 / 16. } },
	{ { 3 / 16., 0 / 16. }, { 3 / 16., 1 / 16. }, { 3 / 16., 2 / 16. } },
	{ { 4 / 16., 0 / 16. }, { 4 / 16., 1 / 16. }, { 4 / 16., 2 / 16. } },
	{ { 5 / 16., 0 / 16. }, { 5 / 16., 1 / 16. }, { 5 / 16., 2 / 16. } },
	{ { 6 / 16., 0 / 16. }, { 6 / 16., 1 / 16. }, { 6 / 16., 2 / 16. } },
	{ { 7 / 16., 0 / 16. }, { 7 / 16., 1 / 16. }, { 7 / 16., 2 / 16. } },
	{ { 8 / 16., 0 / 16. }, { 8 / 16., 1 / 16. }, { 8 / 16., 2 / 16. } },
	{ { 9 / 16., 0 / 16. }, { 9 / 16., 1 / 16. }, { 9 / 16., 2 / 16. } },
};

static const char has_left_side[256] = {
	0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1
};

static const char has_right_side[256] = {
	0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1
};

static const char has_down_side[256] = {
	0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1
};

static const char has_up_side[256] = {
	0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1
};

static const char has_back_side[256] = {
	0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1
};

static const char has_front_side[256] = {
	0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1
};

void update_cell(struct context *ctx, struct array *buf, int64_t x, int64_t y, int64_t z)
{
	int8_t s, l, d, b;
	struct v2f lt;
	const struct v2f *mt;

	s = world_get_shape(ctx->w, v3ll(x, y, z));
	l = world_get_shape(ctx->w, v3ll(x - 1, y, z));
	d = y == 0 ? 0 : world_get_shape(ctx->w, v3ll(x, y - 1, z));
	b = world_get_shape(ctx->w, v3ll(x, y, z - 1));
	if (has_down_side[s] && !has_up_side[d]) {
		mt = texcoord_from_mat[world_get_mat(ctx->w, v3ll(x, y, z))];
		lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x, y - 1, z)));
		vertices_add(buf, vertices_face_dn, 6, v3f(x, y, z), lt, mt);
	}
	if (has_up_side[d] && !has_down_side[s]) {
		mt = texcoord_from_mat[world_get_mat(ctx->w, v3ll(x, y - 1, z))];
		lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x, y, z)));
		vertices_add(buf, vertices_face_up, 6, v3f(x, y, z), lt, mt);
	}
	if (has_left_side[s] && !has_right_side[l]) {
		mt = texcoord_from_mat[world_get_mat(ctx->w, v3ll(x, y, z))];
		lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x - 1, y, z)));
		vertices_add(buf, vertices_face_lf, 6, v3f(x, y, z), lt, mt);
	}
	if (has_right_side[l] && !has_left_side[s]) {
		mt = texcoord_from_mat[world_get_mat(ctx->w, v3ll(x - 1, y, z))];
		lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x, y, z)));
		vertices_add(buf, vertices_face_rt, 6, v3f(x, y, z), lt, mt);
	}
	if (has_back_side[s] && !has_front_side[b]) {
		mt = texcoord_from_mat[world_get_mat(ctx->w, v3ll(x, y, z))];
		lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x, y, z - 1)));
		vertices_add(buf, vertices_face_bk, 6, v3f(x, y, z), lt, mt);
	}
	if (has_front_side[b] && !has_back_side[s]) {
		mt = texcoord_from_mat[world_get_mat(ctx->w, v3ll(x, y, z - 1))];
		lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x, y, z)));
		vertices_add(buf, vertices_face_ft, 6, v3f(x, y, z), lt, mt);
	}
	if (s == SHAPE_SLAB_DN) {
		mt = texcoord_from_mat[world_get_mat(ctx->w, v3ll(x, y, z))];
		lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x, y, z)));
		vertices_add(buf, vertices_slab_dn, 30, v3f(x, y, z), lt, mt);
	} else if (s == SHAPE_SLAB_UP) {
		mt = texcoord_from_mat[world_get_mat(ctx->w, v3ll(x, y, z))];
		lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x, y, z)));
		vertices_add(buf, vertices_slab_up, 30, v3f(x, y, z), lt, mt);
	} else if (s == SHAPE_SLAB_LF) {
		mt = texcoord_from_mat[world_get_mat(ctx->w, v3ll(x, y, z))];
		lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x, y, z)));
		vertices_add(buf, vertices_slab_lf, 30, v3f(x, y, z), lt, mt);
	} else if (s == SHAPE_SLAB_RT) {
		mt = texcoord_from_mat[world_get_mat(ctx->w, v3ll(x, y, z))];
		lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x, y, z)));
		vertices_add(buf, vertices_slab_rt, 30, v3f(x, y, z), lt, mt);
	} else if (s == SHAPE_SLAB_BK) {
		mt = texcoord_from_mat[world_get_mat(ctx->w, v3ll(x, y, z))];
		lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x, y, z)));
		vertices_add(buf, vertices_slab_bk, 30, v3f(x, y, z), lt, mt);
	} else if (s == SHAPE_SLAB_FT) {
		mt = texcoord_from_mat[world_get_mat(ctx->w, v3ll(x, y, z))];
		lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x, y, z)));
		vertices_add(buf, vertices_slab_ft, 30, v3f(x, y, z), lt, mt);
	} else if (s == SHAPE_STAIRS_DF) {
		mt = texcoord_from_mat[world_get_mat(ctx->w, v3ll(x, y, z))];
		lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x, y, z)));
		vertices_add(buf, vertices_stairs_df, 42, v3f(x, y, z), lt, mt);
	} else if (s == SHAPE_STAIRS_DL) {
		mt = texcoord_from_mat[world_get_mat(ctx->w, v3ll(x, y, z))];
		lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x, y, z)));
		vertices_add(buf, vertices_stairs_dl, 42, v3f(x, y, z), lt, mt);
	} else if (s == SHAPE_STAIRS_DB) {
		mt = texcoord_from_mat[world_get_mat(ctx->w, v3ll(x, y, z))];
		lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x, y, z)));
		vertices_add(buf, vertices_stairs_db, 42, v3f(x, y, z), lt, mt);
	} else if (s == SHAPE_STAIRS_DR) {
		mt = texcoord_from_mat[world_get_mat(ctx->w, v3ll(x, y, z))];
		lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x, y, z)));
		vertices_add(buf, vertices_stairs_dr, 42, v3f(x, y, z), lt, mt);
	} else if (s == SHAPE_STAIRS_UF) {
		mt = texcoord_from_mat[world_get_mat(ctx->w, v3ll(x, y, z))];
		lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x, y, z)));
		vertices_add(buf, vertices_stairs_uf, 42, v3f(x, y, z), lt, mt);
	} else if (s == SHAPE_STAIRS_UL) {
		mt = texcoord_from_mat[world_get_mat(ctx->w, v3ll(x, y, z))];
		lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x, y, z)));
		vertices_add(buf, vertices_stairs_ul, 42, v3f(x, y, z), lt, mt);
	} else if (s == SHAPE_STAIRS_UB) {
		mt = texcoord_from_mat[world_get_mat(ctx->w, v3ll(x, y, z))];
		lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x, y, z)));
		vertices_add(buf, vertices_stairs_ub, 42, v3f(x, y, z), lt, mt);
	} else if (s == SHAPE_STAIRS_UR) {
		mt = texcoord_from_mat[world_get_mat(ctx->w, v3ll(x, y, z))];
		lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x, y, z)));
		vertices_add(buf, vertices_stairs_ur, 42, v3f(x, y, z), lt, mt);
	} else if (s == SHAPE_PANE_Y) {
		mt = texcoord_from_mat[world_get_mat(ctx->w, v3ll(x, y, z))];
		lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x, y, z)));
		vertices_add(buf, vertices_pane_y, 36, v3f(x, y, z), lt, mt);
	} else if (s == SHAPE_PANE_X) {
		mt = texcoord_from_mat[world_get_mat(ctx->w, v3ll(x, y, z))];
		lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x, y, z)));
		vertices_add(buf, vertices_pane_x, 36, v3f(x, y, z), lt, mt);
	} else if (s == SHAPE_PANE_Z) {
		mt = texcoord_from_mat[world_get_mat(ctx->w, v3ll(x, y, z))];
		lt = texcoord_from_light(world_get_light(ctx->w, v3ll(x, y, z)));
		vertices_add(buf, vertices_pane_z, 36, v3f(x, y, z), lt, mt);
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

	vertex_buffer_update(ctx->shard_vertex_buffer, id, buf->data, buf->size);
	array_destroy(buf);
}

void use_inventory(struct context *ctx, struct array *inv)
{
	if (inv == NULL) {
		fprintf(stderr, "WARNING: no inventory found\n");
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
				printf("nothing to take\n");
				return;
			}
			int acc = inventory_add(ctx->inv, s2);
			inventory_set_num(inv, i, s2.num - acc);
			if (acc == 0)
				printf("no space to take\n");
			else
				printf("take %s %s %d\n", mat_names[s1.mat], obj_names[s1.obj], acc);
		} else {
			if (s2.num)
				printf("take %s %s %d\n", mat_names[s2.mat], obj_names[s2.obj], s2.num);
			if (s1.num)
				printf("left %s %s %d\n", mat_names[s1.mat], obj_names[s1.obj], s1.num);
			inventory_set(inv, i, s1);
			inventory_set(ctx->inv, ctx->tool, s2);
		}
	} else if (ctx->use == 1) {
		if (ctx->move.y0) {
			if (s2.num == 0) {
				printf("nothing to take\n");
				return;
			}
			int acc = inventory_add(ctx->inv, slot(s2.obj, s2.mat, 1));
			inventory_set_num(inv, i, s2.num - acc);
			if (acc == 0)
				printf("no space to take\n");
			else
				printf("take %s %s 1\n", mat_names[s1.mat], obj_names[s1.obj], acc);
		} else {
			if (s1.num == 0) {
				printf("nothing to leave\n");
				return;
			}
			if (s2.num >= 64) {
				printf("no space to leave\n");
				return;
			}
			if (s2.num == 0) {
				inventory_set_obj(inv, i, s1.obj);
				inventory_set_mat(inv, i, s1.mat);
			} else if (s1.obj != s2.obj || s1.mat != s2.mat) {
				printf("not the same object\n");
				return;
			}
			inventory_set_num(inv, i, s2.num + 1);
			inventory_set_num(ctx->inv, ctx->tool, s1.num - 1);
			printf("left %s %s 1\n", mat_names[s1.mat], obj_names[s1.obj]);
		}
	}
	if (world_get_mat(ctx->w, p) == MAT_WORKBENCH) {
		const struct recipe *r;
		int i;
		for (r = recipes; r->num != 0; ++r) {
			while (recipe_match(r, inv)) {
				do {
					for (i = 0; i < 9; ++i) {
						int j = inventory_get_num(inv, i);
						if (j)
							inventory_set_num(inv, i, j - 1);
					}
					inventory_add(ctx->inv, slot(r->obj, 255, r->num));
					printf("take %s %s %d\n", mat_names[255], obj_names[r->obj], r->num);
				} while (recipe_match(r, inv));
				break;
			}
		}
	}
}

void use_tool(struct context *ctx)
{
	struct v3ll p = ctx->cur.p;
	struct v3f q = ctx->cur.q;
	int f = ctx->cur.face;
	struct slot s = inventory_get(ctx->inv, ctx->tool);

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
		if (s.mat == MAT_WORKBENCH)
			world_set(ctx->w, p, SHAPE_BLOCK_DN, s.mat, inventory(9));
		else if (s.mat == MAT_CRATE)
			world_set(ctx->w, p, SHAPE_BLOCK_DN, s.mat, inventory(16));
		else
			world_set(ctx->w, p, SHAPE_BLOCK_DN, s.mat, NULL);
	} else if (s.obj == OBJ_SLAB) {
		if (f == FACE_UP) {
			world_set(ctx->w, p, SHAPE_SLAB_DN, s.mat, NULL);
		} else if (f == FACE_DN) {
			world_set(ctx->w, p, SHAPE_SLAB_UP, s.mat, NULL);
		} else if (q.y > 0.5) {
			world_set(ctx->w, p, SHAPE_SLAB_UP, s.mat, NULL);
		} else {
			world_set(ctx->w, p, SHAPE_SLAB_DN, s.mat, NULL);
		}
	} else if (s.obj == OBJ_STAIRS) {
		if (f == FACE_UP) {
			world_set(ctx->w, p, SHAPE_STAIRS_DF + (ctx->roty + 2) % 4, s.mat, NULL);
		} else if (f == FACE_DN) {
			world_set(ctx->w, p, SHAPE_STAIRS_UF + (ctx->roty + 2) % 4, s.mat, NULL);
		} else if (q.y > 0.5) {
			world_set(ctx->w, p, SHAPE_STAIRS_UF + (ctx->roty + 2) % 4, s.mat, NULL);
		} else {
			world_set(ctx->w, p, SHAPE_STAIRS_DF + (ctx->roty + 2) % 4, s.mat, NULL);
		}
	} else if (s.obj == OBJ_PANE) {
		if (ctx->roty & 1)
			world_set(ctx->w, p, SHAPE_PANE_X, s.mat, NULL);
		else
			world_set(ctx->w, p, SHAPE_PANE_Z, s.mat, NULL);
	} else if (s.obj == OBJ_FLUID) {
		flowsim_add(ctx->flowsim, p, 1);
	}
}

void update_player(struct context *ctx)
{
	struct v3ll p = ctx->cur.p;
	int s = world_get_shape(ctx->w, p);
	int m = world_get_mat(ctx->w, p);

	if (ctx->cur.face == FACE_UP) {
		if (m == MAT_WORKBENCH || m == MAT_CRATE) {
			use_inventory(ctx, world_get_data(ctx->w, p));
			return;
		}
	}
	if (ctx->act == 1) {
		if (ctx->cur.face != -1) {
			if (s != SHAPE_NONE) {
				world_set(ctx->w, p, 0, 0, NULL);
			}
		}
	}
	if (ctx->use == 1) {
		if (ctx->cur.face != -1) {
			use_tool(ctx);
		}
	}
	if (ctx->pick == 1) {
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
		r.x = M_PI_2 *.99;
	body_set_rotation(ctx->player, r);

	v = body_get_velocity(ctx->player);
	if (ctx->move.x0 == 0 && ctx->move.x1 == 0 && ctx->move.z0 == 0 && ctx->move.z1 == 0)
		ctx->run = 0;
	v.x = (ctx->move.x1 - ctx->move.x0) * (ctx->run ? .18 : 0.15);
	v.y += (ctx->move.y1 - ctx->move.y0) * (ctx->run ? .18 : 0.15);
	v.z = (ctx->move.z1 - ctx->move.z0) * (ctx->run ? .18 : 0.15);
	v = v3_roty(v, r.y);
	body_set_velocity(ctx->player, v);
	body_set_step_size(ctx->player, 1 + ctx->run);

	camera_set_fovy(ctx->cam, (ctx->cam->fovy + (ctx->run ? 70.f : 60.f)) * 0.5);
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
	m.x = CHUNK_W * CHUNKS_PER_WORLD * floor(bb.x1 / (CHUNK_W * CHUNKS_PER_WORLD));
	m.y = CHUNK_D * CHUNKS_PER_WORLD * floor(bb.y1 / (CHUNK_D * CHUNKS_PER_WORLD));

	i = 0;
	for (x = 0; x < CHUNKS_PER_WORLD; ++x) {
		for (z = 0; z < CHUNKS_PER_WORLD; ++z) {
			c = ctx->w->chunks[x][z];/*
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
						    }*/
			if (c->flags != 0) {
				c->priority = hypot((double)c->x - ctx->player->p.x,
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
		//		fprintf(stdout, "Update chunk %d (%d,%d); priority:%d", c->id, c->x, c->z, c->priority);
		if ((c->flags & CHUNK_UNLOADED) != 0) {
			//			printf("; load from file");
			/* load this chunk */
			c->flags ^= CHUNK_UNLOADED;
		}
		if ((c->flags & CHUNK_UNRENDERED) != 0) {
			//			printf("; update vertex buffers");
			for (k = 0; k < SHARDS_PER_CHUNK; ++k)
				update_vbo(ctx, c->shards[k]->id, c->x, k * SHARD_H, c->z);
			c->flags ^= CHUNK_UNRENDERED;
		}
		//		fprintf(stdout, "\n");
	}
}

void update(void *data)
{
	struct context *ctx = data;
	struct v3ll p;
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
	tone_mapper_update(ctx->tone_mapper, (world_get_light(ctx->w, p) << 4) / 255., 0);
	++ctx->tick;
}

