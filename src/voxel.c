

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include "chunk.h"
#include "terraform.h"
#include "main_loop.h"
#include "physics.h"
#include "profile.h"
#include "renderer.h"
#include "camera.h"
#include "media.h"

enum {
	OBJ_BLOCK,	/* full block */
	OBJ_SLAB,	/* half block */
	OBJ_STAIRS,	/* stairs */
	OBJ_PANE,	/* thin pane in the middle */
	OBJ_DECOR,	/* thin pane in a side */
	OBJ_COUNT
};

static const char *obj_names[] = {
	"block",
	"slab",
	"stairs",
	"pane",
	"decor",
};

static const char *face_names[] = {
	"front"
	"left",
	"back",
	"right",
	"up",
	"down",
};

struct context {
	char *dir;
	struct main_loop *ml;
	struct world *w;
	struct profile_manager *prof_mgr;
	struct renderer *shard_renderer;
	struct camera *cam;
	GLuint tex_terrain;
	struct space *space;
	struct body *player;
	struct query cur;
	struct aab3c move;
	char act;
	char use;
	char pick;
	char run;
	char obj;
	char mat;
	char roty;
	char rotx;
	int chunks_per_tick;
};

void render(void *data);
void update(void *data);
void event(const SDL_Event *e, void *data);

int load_all(struct context *ctx);
int save_all(struct context *ctx);

int load_world(struct world *w, const char *dir);
int save_world(struct world *w, const char *dir);

int load_chunk(struct chunk *c, const char *dir);
int save_chunk(struct chunk *c, const char *dir);

int main(int argc, char *argv[])
{
	struct context *ctx;

	ctx = calloc(1, sizeof(*ctx));
	ctx->dir = "foo";
	ctx->w = world();
	ctx->prof_mgr = profile_manager();
	ctx->chunks_per_tick = 4;

	/* Setup main loop */
	ctx->ml = main_loop(30);
	main_loop_set_event_callback(ctx->ml, event, ctx);
	main_loop_set_update_callback(ctx->ml, update, ctx);
	main_loop_add_window(ctx->ml, window("voxel", 0, 0, 1280, 768, 0));
	window_set_render_callback(ctx->ml->windows, render, ctx);

	/* Load textures */
	ctx->tex_terrain = texture("data/gradient.png");

	/* Initialize physics */
	ctx->space = space(ctx->w);
	space_set_gravity(ctx->space, v3f(0, -0.05, 0));
	space_set_iterations(ctx->space, 2);
	space_set_impulse(ctx->space, 0.001);
	space_set_terminal_speed(ctx->space, 1);

	/* Setup camera */
	ctx->cam = camera();
	camera_set_max_distance(ctx->cam, 1024);
	camera_set_aspect_ratio(ctx->cam, 1280.0 / 768.0);

	/* Create renderers */
	ctx->shard_renderer = renderer(SHARDS_PER_WORLD, &vertex3_traits);

	/* Load world */
	load_all(ctx);

	/* Run */
	main_loop_run(ctx->ml);

	/* Save the world */
	save_all(ctx);

	/* Destroy everything */
	world_destroy(ctx->w);
	main_loop_destroy(ctx->ml);
	renderer_destroy(ctx->shard_renderer);
	profile_manager_destroy(ctx->prof_mgr);
	free(ctx);
	return 0;
}

int load_all(struct context *ctx)
{
	int x, z;
	struct chunk *c;
	struct v3f p;

	if (load_world(ctx->w, ctx->dir) != 0) {
		/* initialize from scratch */
	}
	for (x = 0; x < CHUNKS_PER_WORLD; ++x) {
		for (z = 0; z < CHUNKS_PER_WORLD; ++z) {
			c = ctx->w->chunks[x][z];
			c->x = ctx->w->x + x * CHUNK_W;
			c->z = ctx->w->z + z * CHUNK_D;
			if (load_chunk(c, ctx->dir) != 0)
				terraform(0, c);
		}
	}
	p.x = ctx->w->x + CHUNK_W * CHUNKS_PER_WORLD / 2;
	p.y = CHUNK_H;
	p.z = ctx->w->z + CHUNK_W * CHUNKS_PER_WORLD / 2;
	ctx->player = body(ctx->space);
	body_set_position(ctx->player, p);
	body_set_size(ctx->player, v2f(0.325, 0.825));
	ctx->obj = OBJ_BLOCK;
	return 0;
}

int save_all(struct context *ctx)
{
	int x, z;
	struct chunk *c;

	if (save_world(ctx->w, ctx->dir) != 0)
		return -1;
	for (x = 0; x < CHUNKS_PER_WORLD; ++x)
		for (z = 0; z < CHUNKS_PER_WORLD; ++z)
			save_chunk(ctx->w->chunks[x][z], ctx->dir);
	return 0;
}

int load_world(struct world *w, const char *dir)
{
	union sz_tag root;
	char path[256];
	int fd;

	snprintf(path, sizeof(path), "%s/world.dat", dir);
	fd = open(path, O_RDONLY, 0400);
	if (fd < 0) {
		perror(path);
		return -1;
	}
	if (sz_read(fd, &root) != 0)
		return -1;
	close(fd);
	if (world_load(w, &root) != 0)
		return -1;
	//sz_destroy(root);
	return 0;
}

int save_world(struct world *w, const char *dir)
{
	union sz_tag *root;
	char path[256];
	int fd;

	if (world_save(w, &root) != 0)
		return -1;
	snprintf(path, sizeof(path), "%s/world.dat", dir);
	fd = creat(path, 0600);
	if (fd < 0) {
		perror(path);
		return -1;
	}
	if (sz_write(fd, root) != 0)
		return -1;
	close(fd);
	sz_destroy(root);
	return 0;
}

int load_chunk(struct chunk *c, const char *dir)
{
	char path[256];
	int fd;
	union sz_tag root;

	snprintf(path, sizeof(path), "%s/%06x%06x.dat", dir, c->x / CHUNK_W, c->z / CHUNK_D);
	fd = open(path, O_RDONLY, 0400);
	if (fd < 0)
		return -1;
	if (sz_read(fd, &root) != 0)
		return -1;
	close(fd);
	if (chunk_load(c, &root) != 0)
		return -1;
	//	sz_destroy(root);
	return 0;
}

int save_chunk(struct chunk *c, const char *dir)
{
	char path[256];
	int fd;
	union sz_tag *root;

	if (chunk_save(c, &root) != 0)
		return -1;
	snprintf(path, sizeof(path), "%s/%06x%06x.dat", dir, c->x / CHUNK_W, c->z / CHUNK_D);
	fd = creat(path, 0600);
	if (fd < 0) {
		perror(path);
		return -1;
	}
	if (sz_write(fd, root) != 0)
		return -1;
	close(fd);
	sz_destroy(root);
	return 0;
}

void render_cursor(struct context *ctx)
{
	if (ctx->cur.face == -1)
		return;
	GLfloat x = ctx->cur.p.x;
	GLfloat y = ctx->cur.p.y;
	GLfloat z = ctx->cur.p.z;

	glColor3f(0, 0, 0);
	glLineWidth(2);

	glBegin(GL_LINE_LOOP);
	glVertex3f(x, y, z);
	glVertex3f(x, y, z + 1);
	glVertex3f(x, y + 1, z + 1);
	glVertex3f(x, y + 1, z);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(x + 1, y, z);
	glVertex3f(x + 1, y, z + 1);
	glVertex3f(x + 1, y + 1, z + 1);
	glVertex3f(x + 1, y + 1, z);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(x, y, z);
	glVertex3f(x, y, z + 1);
	glVertex3f(x + 1, y, z + 1);
	glVertex3f(x + 1, y, z);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(x, y + 1, z);
	glVertex3f(x, y + 1, z + 1);
	glVertex3f(x + 1, y + 1, z + 1);
	glVertex3f(x + 1, y + 1, z);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(x, y, z);
	glVertex3f(x + 1, y, z);
	glVertex3f(x + 1, y + 1, z);
	glVertex3f(x, y + 1, z);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(x, y, z + 1);
	glVertex3f(x + 1, y, z + 1);
	glVertex3f(x + 1, y + 1, z + 1);
	glVertex3f(x, y + 1, z + 1);
	glEnd();
}

void render(void *data)
{
	struct context *ctx = data;
	int64_t x, y, z;
	struct v3f p;
	struct chunk *c;
	struct shard *s;
	int shards_rendered;

	shards_rendered = 0;
	camera_load_gl_matrices(ctx->cam);
	render_cursor(ctx);
	glColor3f(1, 1, 1);
	//	glEnable(GL_BLEND);
	//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, ctx->tex_terrain);
	renderer_begin(ctx->shard_renderer);
	for (x = 0; x < CHUNKS_PER_WORLD; ++x) {
		for (z = 0; z < CHUNKS_PER_WORLD; ++z) {
			c = ctx->w->chunks[x][z];
			p.x = c->x + CHUNK_W / 2;
			p.z = c->z + CHUNK_W / 2;
			for (y = 0; y < SHARDS_PER_CHUNK; ++y) {
				s = c->shards[y];
				if (ctx->shard_renderer->vbo_sizes[s->id] == 0)
					continue;
				p.y = (s->y + 0.5) * SHARD_W;
				if (camera_visible(ctx->cam, p, SHARD_W) == 0)
					continue;
				renderer_buffer(ctx->shard_renderer, GL_TRIANGLES, s->id);
				++shards_rendered;
			}
		}
	}
	renderer_end(ctx->shard_renderer);
	glDisable(GL_TEXTURE_2D);
}

int chunks_by_priority(const void *p1, const void *p2)
{
	const struct chunk *c1, *c2;
	c1 = *(const void **)p1;
	c2 = *(const void **)p2;
	return c1->priority - c2->priority;
}

void update_player(struct context *ctx)
{
	struct v3ll p;
	int f;
	int c;
	if (ctx->act == 1) {
		if (ctx->cur.face != -1) {
			p = ctx->cur.p;
			if (WORLD_AT(ctx->w, shape, p.x, p.y, p.z) != 0)
				world_set(ctx->w, p, 0, 0);
		}
	}
	if (ctx->use == 1) {
		f = ctx->cur.face;
		c = ctx->cur.cell;
		if (f != -1) {
			if (f == FACE_LF)
				p = v3_add(ctx->cur.p, v3c(-1, 0, 0));
			else if (f == FACE_RT)
				p = v3_add(ctx->cur.p, v3c(1, 0, 0));
			else if (f == FACE_DN)
				p = v3_add(ctx->cur.p, v3c(0, -1, 0));
			else if (f == FACE_UP)
				p = v3_add(ctx->cur.p, v3c(0, 1, 0));
			else if (f == FACE_BK)
				p = v3_add(ctx->cur.p, v3c(0, 0, -1));
			else if (f == FACE_FT)
				p = v3_add(ctx->cur.p, v3c(0, 0, 1));
			if (WORLD_AT(ctx->w, shape, p.x, p.y, p.z) == 0) {
				if (ctx->obj == OBJ_BLOCK) {
					world_set(ctx->w, p, SHAPE_BLOCK_DN, 255);
				} else if (ctx->obj == OBJ_SLAB) {
					if (f == FACE_UP) {
						world_set(ctx->w, p, SHAPE_SLAB_DN, 255);
					} else if (f == FACE_DN) {
						world_set(ctx->w, p, SHAPE_SLAB_UP, 255);
					} else if (c & (CELL_LDB | CELL_LDF | CELL_RDB | CELL_RDF)) {
						world_set(ctx->w, p, SHAPE_SLAB_UP, 255);
					} else {
						world_set(ctx->w, p, SHAPE_SLAB_DN, 255);
					}
				} else if (ctx->obj == OBJ_STAIRS) {
					if (f == FACE_UP) {
						world_set(ctx->w, p, SHAPE_STAIRS_DF + (ctx->roty + 2) % 4, 255);
					} else if (f == FACE_DN) {
						world_set(ctx->w, p, SHAPE_STAIRS_UF + (ctx->roty + 2) % 4, 255);
					} else if (c & (CELL_LDB | CELL_LDF | CELL_RDB | CELL_RDF)) {
						world_set(ctx->w, p, SHAPE_STAIRS_UF + (ctx->roty + 2) % 4, 255);
					} else {
						world_set(ctx->w, p, SHAPE_STAIRS_DF + (ctx->roty + 2) % 4, 255);
					}
				}
			}
		}
	}
	if (ctx->pick == 1) {
	}
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

	camera_set_fovy(ctx->cam, (ctx->cam->fovy + (ctx->run ? 70.f : 60.f)) * 0.5);
	camera_set_position(ctx->cam, v3_add(ctx->player->p, v3f(0, .6, 0)));
	camera_set_rotation(ctx->cam, ctx->player->r);

	ctx->rotx = (unsigned int)floor(0.5 + r.x / M_PI_2) & 3;
	ctx->roty = (unsigned int)floor(0.5 + r.y / M_PI_2) & 3;

	v = v3f(0, 0, -5);
	v = v3_rotx(v, r.x);
	v = v3_roty(v, r.y);
	space_query(ctx->space, ctx->cam->p, v, &ctx->cur);
}

void tcoord_by_material(uint8_t m, struct aab2f *tc)
{
	tc->x0 = (m % 16) / 16.;
	tc->y0 = (m / 16) / 16.;
	tc->x1 = tc->x0 + 1 / 16.;
	tc->y1 = tc->y0 + 1 / 16.;
}

static const char has_left_side[256] = {
	0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0
};

static const char has_right_side[256] = {
	0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1
};

static const char has_down_side[256] = {
	0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0
};

static const char has_up_side[256] = {
	0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1
};

static const char has_back_side[256] = {
	0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0
};

static const char has_front_side[256] = {
	0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0
};


void update_cell(struct context *ctx, struct vertex3_buf *buf, int64_t x, int64_t y, int64_t z)
{
	int8_t s, l, d, b, m;
	struct aab2f tc;

	s = WORLD_AT(ctx->w, shape, x, y, z);
	l = WORLD_AT(ctx->w, shape, x - 1, y, z);
	d = y == 0 ? 0 : WORLD_AT(ctx->w, shape, x, y - 1, z);
	b = WORLD_AT(ctx->w, shape, x, y, z - 1);
	if (has_down_side[s] && !has_up_side[d]) {
		m = WORLD_AT(ctx->w, mat, x, y, z);
		tcoord_by_material(m, &tc);
		vertex3_buf_down(buf, v3f(x, y, z), 1, 1, tc);
	}
	if (has_up_side[d] && !has_down_side[s]) {
		m = WORLD_AT(ctx->w, mat, x, y - 1, z);
		tcoord_by_material(m, &tc);
		vertex3_buf_up(buf, v3f(x, y, z), 1, 1, tc);
	}
	if (has_left_side[s] && !has_right_side[l]) {
		m = WORLD_AT(ctx->w, mat, x, y, z);
		tcoord_by_material(m, &tc);
		vertex3_buf_left(buf, v3f(x, y, z), 1, 1, tc);
	}
	if (has_right_side[l] && !has_left_side[s]) {
		m = WORLD_AT(ctx->w, mat, x - 1, y, z);
		tcoord_by_material(m, &tc);
		vertex3_buf_right(buf, v3f(x, y, z), 1, 1, tc);
	}
	if (has_back_side[s] && !has_front_side[b]) {
		m = WORLD_AT(ctx->w, mat, x, y, z);
		tcoord_by_material(m, &tc);
		vertex3_buf_back(buf, v3f(x, y, z), 1, 1, tc);
	}
	if (has_front_side[b] && !has_back_side[s]) {
		m = WORLD_AT(ctx->w, mat, x, y, z - 1);
		tcoord_by_material(m, &tc);
		vertex3_buf_front(buf, v3f(x, y, z), 1, 1, tc);
	}
	if (s == SHAPE_SLAB_DN) {
		m = WORLD_AT(ctx->w, mat, x, y, z);
		tcoord_by_material(m, &tc);
		vertex3_buf_up(buf, v3f(x, y + 0.5, z), 1, 1, tc);
		vertex3_buf_left(buf, v3f(x, y, z), 0.5, 1, tc);
		vertex3_buf_right(buf, v3f(x + 1, y, z), 0.5, 1, tc);
		vertex3_buf_back(buf, v3f(x, y, z), 1, 0.5, tc);
		vertex3_buf_front(buf, v3f(x, y, z + 1), 1, 0.5, tc);
	} else if (s == SHAPE_SLAB_UP) {
		m = WORLD_AT(ctx->w, mat, x, y, z);
		tcoord_by_material(m, &tc);
		vertex3_buf_down(buf, v3f(x, y + 0.5, z), 1, 1, tc);
		vertex3_buf_left(buf, v3f(x, y + 0.5, z), 0.5, 1, tc);
		vertex3_buf_right(buf, v3f(x + 1, y + 0.5, z), 0.5, 1, tc);
		vertex3_buf_back(buf, v3f(x, y + 0.5, z), 1, 0.5, tc);
		vertex3_buf_front(buf, v3f(x, y + 0.5, z + 1), 1, 0.5, tc);
	} else if (s == SHAPE_STAIRS_DF) {
		m = WORLD_AT(ctx->w, mat, x, y, z);
		tcoord_by_material(m, &tc);
		vertex3_buf_up(buf, v3f(x, y + 1, z + 0.5), 1, 0.5, tc);
		vertex3_buf_up(buf, v3f(x, y + 0.5, z), 1, 0.5, tc);
		vertex3_buf_left(buf, v3f(x, y, z), 0.5, 1, tc);
		vertex3_buf_left(buf, v3f(x, y + 0.5, z + 0.5), 0.5, 0.5, tc);
		vertex3_buf_right(buf, v3f(x + 1, y, z), 0.5, 1, tc);
		vertex3_buf_right(buf, v3f(x + 1, y + 0.5, z + 0.5), 0.5, 0.5, tc);
		vertex3_buf_back(buf, v3f(x, y, z), 1, 0.5, tc);
		vertex3_buf_back(buf, v3f(x, y + 0.5, z + 0.5), 1, 0.5, tc);
	} else if (s == SHAPE_STAIRS_DL) {
		m = WORLD_AT(ctx->w, mat, x, y, z);
		tcoord_by_material(m, &tc);
		vertex3_buf_up(buf, v3f(x, y + 1, z), 0.5, 1, tc);
		vertex3_buf_up(buf, v3f(x + 0.5, y + 0.5, z), 0.5, 1, tc);
		vertex3_buf_right(buf, v3f(x + 1, y, z), 0.5, 1, tc);
		vertex3_buf_right(buf, v3f(x + 0.5, y + 0.5, z), 0.5, 1, tc);
		vertex3_buf_back(buf, v3f(x, y, z), 1, 0.5, tc);
		vertex3_buf_back(buf, v3f(x, y + 0.5, z), 0.5, 0.5, tc);
		vertex3_buf_front(buf, v3f(x, y, z + 1), 1, 0.5, tc);
		vertex3_buf_front(buf, v3f(x, y + 0.5, z + 1), 0.5, 0.5, tc);
	} else if (s == SHAPE_STAIRS_DB) {
		m = WORLD_AT(ctx->w, mat, x, y, z);
		tcoord_by_material(m, &tc);
		vertex3_buf_up(buf, v3f(x, y + 1, z), 1, 0.5, tc);
		vertex3_buf_up(buf, v3f(x, y + 0.5, z + 0.5), 1, 0.5, tc);
		vertex3_buf_left(buf, v3f(x, y, z), 0.5, 1, tc);
		vertex3_buf_left(buf, v3f(x, y + 0.5, z), 0.5, 0.5, tc);
		vertex3_buf_right(buf, v3f(x + 1, y, z), 0.5, 1, tc);
		vertex3_buf_right(buf, v3f(x + 1, y + 0.5, z), 0.5, 0.5, tc);
		vertex3_buf_front(buf, v3f(x, y, z + 1), 1, 0.5, tc);
		vertex3_buf_front(buf, v3f(x, y + 0.5, z + 0.5), 1, 0.5, tc);
	} else if (s == SHAPE_STAIRS_DR) {
		m = WORLD_AT(ctx->w, mat, x, y, z);
		tcoord_by_material(m, &tc);
		vertex3_buf_up(buf, v3f(x + 0.5, y + 1, z), 0.5, 1, tc);
		vertex3_buf_up(buf, v3f(x, y + 0.5, z), 0.5, 1, tc);
		vertex3_buf_left(buf, v3f(x, y, z), 0.5, 1, tc);
		vertex3_buf_left(buf, v3f(x + 0.5, y + 0.5, z), 0.5, 1, tc);
		vertex3_buf_back(buf, v3f(x, y, z), 1, 0.5, tc);
		vertex3_buf_back(buf, v3f(x + 0.5, y + 0.5, z), 0.5, 0.5, tc);
		vertex3_buf_front(buf, v3f(x, y, z + 1), 1, 0.5, tc);
		vertex3_buf_front(buf, v3f(x + 0.5, y + 0.5, z + 1), 0.5, 0.5, tc);
	} else if (s == SHAPE_STAIRS_UF) {
		m = WORLD_AT(ctx->w, mat, x, y, z);
		tcoord_by_material(m, &tc);
		vertex3_buf_down(buf, v3f(x, y, z + 0.5), 1, 0.5, tc);
		vertex3_buf_down(buf, v3f(x, y + 0.5, z), 1, 0.5, tc);
		vertex3_buf_left(buf, v3f(x, y + 0.5, z), 0.5, 1, tc);
		vertex3_buf_left(buf, v3f(x, y, z + 0.5), 0.5, 0.5, tc);
		vertex3_buf_right(buf, v3f(x + 1, y + 0.5, z), 0.5, 1, tc);
		vertex3_buf_right(buf, v3f(x + 1, y, z + 0.5), 0.5, 0.5, tc);
		vertex3_buf_back(buf, v3f(x, y + 0.5, z), 1, 0.5, tc);
		vertex3_buf_back(buf, v3f(x, y, z + 0.5), 1, 0.5, tc);
	} else if (s == SHAPE_STAIRS_UL) {
		m = WORLD_AT(ctx->w, mat, x, y, z);
		tcoord_by_material(m, &tc);
		vertex3_buf_down(buf, v3f(x, y, z), 0.5, 1, tc);
		vertex3_buf_down(buf, v3f(x + 0.5, y + 0.5, z), 0.5, 1, tc);
		vertex3_buf_right(buf, v3f(x + 1, y + 0.5, z), 0.5, 1, tc);
		vertex3_buf_right(buf, v3f(x + 0.5, y, z), 0.5, 1, tc);
		vertex3_buf_back(buf, v3f(x, y + 0.5, z), 1, 0.5, tc);
		vertex3_buf_back(buf, v3f(x, y, z), 0.5, 0.5, tc);
		vertex3_buf_front(buf, v3f(x, y + 0.5, z + 1), 1, 0.5, tc);
		vertex3_buf_front(buf, v3f(x, y, z + 1), 0.5, 0.5, tc);
	} else if (s == SHAPE_STAIRS_UB) {
		m = WORLD_AT(ctx->w, mat, x, y, z);
		tcoord_by_material(m, &tc);
		vertex3_buf_down(buf, v3f(x, y, z), 1, 0.5, tc);
		vertex3_buf_down(buf, v3f(x, y + 0.5, z + 0.5), 1, 0.5, tc);
		vertex3_buf_left(buf, v3f(x, y + 0.5, z), 0.5, 1, tc);
		vertex3_buf_left(buf, v3f(x, y, z), 0.5, 0.5, tc);
		vertex3_buf_right(buf, v3f(x + 1, y + 0.5, z), 0.5, 1, tc);
		vertex3_buf_right(buf, v3f(x + 1, y, z), 0.5, 0.5, tc);
		vertex3_buf_front(buf, v3f(x, y + 0.5, z + 1), 1, 0.5, tc);
		vertex3_buf_front(buf, v3f(x, y, z + 0.5), 1, 0.5, tc);
	} else if (s == SHAPE_STAIRS_UR) {
		m = WORLD_AT(ctx->w, mat, x, y, z);
		tcoord_by_material(m, &tc);
		vertex3_buf_down(buf, v3f(x + 0.5, y, z), 0.5, 1, tc);
		vertex3_buf_down(buf, v3f(x, y + 0.5, z), 0.5, 1, tc);
		vertex3_buf_left(buf, v3f(x, y + 0.5, z), 0.5, 1, tc);
		vertex3_buf_left(buf, v3f(x + 0.5, y, z), 0.5, 1, tc);
		vertex3_buf_back(buf, v3f(x, y + 0.5, z), 1, 0.5, tc);
		vertex3_buf_back(buf, v3f(x + 0.5, y, z), 0.5, 0.5, tc);
		vertex3_buf_front(buf, v3f(x, y + 0.5, z + 1), 1, 0.5, tc);
		vertex3_buf_front(buf, v3f(x + 0.5, y, z + 1), 0.5, 0.5, tc);
	} else if (s == SHAPE_SLAB_LF) {
		m = WORLD_AT(ctx->w, mat, x, y, z);
		tcoord_by_material(m, &tc);
		vertex3_buf_right(buf, v3f(x + 0.5, y, z), 1, 1, tc);
		vertex3_buf_down(buf, v3f(x, y, z), 0.5, 1, tc);
		vertex3_buf_up(buf, v3f(x, y + 1, z), 0.5, 1, tc);
		vertex3_buf_back(buf, v3f(x, y, z), 0.5, 1, tc);
		vertex3_buf_front(buf, v3f(x, y, z + 1), 0.5, 1, tc);
	} else if (s == SHAPE_SLAB_RT) {
		m = WORLD_AT(ctx->w, mat, x, y, z);
		tcoord_by_material(m, &tc);
		vertex3_buf_left(buf, v3f(x + 0.5, y, z), 1, 1, tc);
		vertex3_buf_down(buf, v3f(x + 0.5, y, z), 0.5, 1, tc);
		vertex3_buf_up(buf, v3f(x + 0.5, y + 1, z), 0.5, 1, tc);
		vertex3_buf_back(buf, v3f(x + 0.5, y, z), 0.5, 1, tc);
		vertex3_buf_front(buf, v3f(x + 0.5, y, z + 1), 0.5, 1, tc);
	} else if (s == SHAPE_SLAB_BK) {
		m = WORLD_AT(ctx->w, mat, x, y, z);
		tcoord_by_material(m, &tc);
		vertex3_buf_front(buf, v3f(x, y, z + 0.5), 1, 1, tc);
		vertex3_buf_down(buf, v3f(x, y, z), 1, 0.5, tc);
		vertex3_buf_up(buf, v3f(x, y + 1, z), 1, 0.5, tc);
		vertex3_buf_left(buf, v3f(x, y, z), 1, 0.5, tc);
		vertex3_buf_right(buf, v3f(x + 1, y, z), 1, 0.5, tc);
	} else if (s == SHAPE_SLAB_FT) {
		m = WORLD_AT(ctx->w, mat, x, y, z);
		tcoord_by_material(m, &tc);
		vertex3_buf_back(buf, v3f(x, y, z + 0.5), 1, 1, tc);
		vertex3_buf_down(buf, v3f(x, y, z + 0.5), 1, 0.5, tc);
		vertex3_buf_up(buf, v3f(x, y + 1, z + 0.5), 1, 0.5, tc);
		vertex3_buf_left(buf, v3f(x, y, z + 0.5), 1, 0.5, tc);
		vertex3_buf_right(buf, v3f(x + 1, y, z + 0.5), 1, 0.5, tc);
	} else if (s == SHAPE_PANE_Y) {
		m = WORLD_AT(ctx->w, mat, x, y, z);
		tcoord_by_material(m, &tc);
		vertex3_buf_down(buf, v3f(x, y + 0.46875, z), 1, 1, tc);
		vertex3_buf_up(buf, v3f(x, y + 0.53125, z), 1, 1, tc);
		vertex3_buf_left(buf, v3f(x, y + 0.46875, z), 0.0625, 1, tc);
		vertex3_buf_right(buf, v3f(x + 1, y + 0.46875, z), 0.0625, 1, tc);
		vertex3_buf_back(buf, v3f(x, y + 0.46875, z), 1, 0.0625, tc);
		vertex3_buf_front(buf, v3f(x, y + 0.46875, z + 1), 1, 0.0625, tc);
	} else if (s == SHAPE_PANE_X) {
		m = WORLD_AT(ctx->w, mat, x, y, z);
		tcoord_by_material(m, &tc);
		vertex3_buf_left(buf, v3f(x + 0.46875, y, z), 1, 1, tc);
		vertex3_buf_right(buf, v3f(x + 0.53125, y, z), 1, 1, tc);
		vertex3_buf_down(buf, v3f(x + 0.46875, y, z), 0.0625, 1, tc);
		vertex3_buf_up(buf, v3f(x + 0.46875, y + 1, z), 0.0625, 1, tc);
		vertex3_buf_back(buf, v3f(x + 0.46875, y, z), 0.0625, 1, tc);
		vertex3_buf_front(buf, v3f(x + 0.46875, y, z + 1), 0.0625, 1, tc);
	} else if (s == SHAPE_PANE_Z) {
		m = WORLD_AT(ctx->w, mat, x, y, z);
		tcoord_by_material(m, &tc);
		vertex3_buf_back(buf, v3f(x, y, z + 0.46875), 1, 1, tc);
		vertex3_buf_front(buf, v3f(x, y, z + 0.53125), 1, 1, tc);
		vertex3_buf_down(buf, v3f(x, y, z + 0.46875), 1, 0.0625, tc);
		vertex3_buf_up(buf, v3f(x, y + 1, z + 0.46875), 1, 0.0625, tc);
		vertex3_buf_left(buf, v3f(x, y, z + 0.46875), 1, 0.0625, tc);
		vertex3_buf_right(buf, v3f(x + 1, y, z + 0.46875), 1, 0.0625, tc);
	}
}

void update_vbo(struct context *ctx, int id, int64_t x0, int64_t y0, int64_t z0)
{
	int64_t x1, y1, z1;
	int64_t x, y, z;
	struct vertex3_buf *buf;

	x1 = x0 + SHARD_W;
	y1 = y0 + SHARD_H;
	z1 = z0 + SHARD_D;
	buf = vertex3_buf();

	for (x = x0; x < x1; ++x) {
		for (y = y0; y < y1; ++y) {
			for (z = z0; z < z1; ++z) {
				update_cell(ctx, buf, x, y, z);
			}
		}
	}

	renderer_update(ctx->shard_renderer, id, buf->data, buf->size);
	vertex3_buf_destroy(buf);
}

void update_chunks(struct context *ctx)
{
	int x, z, i, j, k;
	struct chunk *c;
	struct chunk *out_of_date[CHUNKS_PER_WORLD * CHUNKS_PER_WORLD];

	i = 0;
	for (x = 0; x < CHUNKS_PER_WORLD; ++x) {
		for (z = 0; z < CHUNKS_PER_WORLD; ++z) {
			c = ctx->w->chunks[x][z];
			if (c->up_to_date == 0) {
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
		fprintf(stdout, "Update chunk %d (%d,%d); priority:%d", c->id, c->x, c->z, c->priority);
		for (k = 0; k < SHARDS_PER_CHUNK; ++k)
			update_vbo(ctx, c->shards[k]->id, c->x, k * SHARD_H, c->z);
		fprintf(stdout, "\n");
		c->up_to_date = 1;
	}
}

void update(void *data)
{
	struct context *ctx = data;
	space_run(ctx->space);
	update_player(ctx);
	update_camera(ctx);
	update_chunks(ctx);
}

void event(const SDL_Event *e, void *data)
{
	struct context *ctx = data;

	if (e->type == SDL_KEYDOWN) {
		if (e->key.repeat) {
			return;
		} else if (e->key.keysym.sym == SDLK_ESCAPE) {
			main_loop_kill(ctx->ml);
		} else if (e->key.keysym.sym == SDLK_w) {
			ctx->move.z0 = 1;
		} else if (e->key.keysym.sym == SDLK_a) {
			ctx->move.x0 = 1;
		} else if (e->key.keysym.sym == SDLK_s) {
			ctx->move.z1 = 1;
		} else if (e->key.keysym.sym == SDLK_d) {
			ctx->move.x1 = 1;
		} else if (e->key.keysym.sym == SDLK_LSHIFT) {
			ctx->move.y0 = 1;
		} else if (e->key.keysym.sym == SDLK_SPACE) {
			ctx->move.y1 = 1;
		} else if (e->key.keysym.sym == SDLK_LCTRL) {
			ctx->run = 1;
		} else if (e->key.keysym.sym == SDLK_o) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		} else if (e->key.keysym.sym == SDLK_p) {
			fprintf(stdout, "=== PROFILE START ===\n");
			profile_manager_reset(ctx->prof_mgr);
		}
	} else if (e->type == SDL_KEYUP) {
		if (e->key.repeat) {
			return;
		} else if (e->key.keysym.sym == SDLK_w) {
			ctx->move.z0 = 0;
		} else if (e->key.keysym.sym == SDLK_a) {
			ctx->move.x0 = 0;
		} else if (e->key.keysym.sym == SDLK_s) {
			ctx->move.z1 = 0;
		} else if (e->key.keysym.sym == SDLK_d) {
			ctx->move.x1 = 0;
		} else if (e->key.keysym.sym == SDLK_LSHIFT) {
			ctx->move.y0 = 0;
		} else if (e->key.keysym.sym == SDLK_SPACE) {
			ctx->move.y1 = 0;
		} else if (e->key.keysym.sym == SDLK_o) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		} else if (e->key.keysym.sym == SDLK_p) {
			fprintf(stdout, "=== PROFILE DUMP ===\n");
			profile_manager_dump(ctx->prof_mgr);
		} else if (e->key.keysym.sym == SDLK_q) {
			if (ctx->cur.face != -1)
				printf("looking at %d,%d,%d; face %s; cell %02x; mat %d; shape %d\n",
						ctx->cur.p.x, ctx->cur.p.y, ctx->cur.p.z,
						face_names[ctx->cur.face], ctx->cur.cell,
						WORLD_AT(ctx->w, mat, ctx->cur.p.x, ctx->cur.p.y, ctx->cur.p.z),
						WORLD_AT(ctx->w, shape, ctx->cur.p.x, ctx->cur.p.y, ctx->cur.p.z));
		} else if (e->key.keysym.sym == SDLK_c) {
			printf("at %g,%g,%g; rot %g,%g; facing %d,%s\n",
				ctx->cam->p.x, ctx->cam->p.y, ctx->cam->p.x,
				ctx->cam->r.x, ctx->cam->r.y,
				ctx->rotx, face_names[ctx->roty]);
		}
	} else if (e->type == SDL_MOUSEBUTTONDOWN) {
		if (e->button.button == SDL_BUTTON_LEFT) {
			ctx->act = 1;
		} else if (e->button.button == SDL_BUTTON_RIGHT) {
			ctx->use = 1;
		} else if (e->button.button == SDL_BUTTON_MIDDLE) {
			ctx->pick = 1;
		}
	} else if (e->type == SDL_MOUSEBUTTONUP) {
		if (e->button.button == SDL_BUTTON_LEFT) {
			ctx->act = 0;
		} else if (e->button.button == SDL_BUTTON_RIGHT) {
			ctx->use = 0;
		} else if (e->button.button == SDL_BUTTON_MIDDLE) {
			ctx->pick = 0;
		}
	} else if (e->type == SDL_MOUSEWHEEL) {
		if (e->wheel.y > 0) {
			if (ctx->obj == 0)
				ctx->obj = OBJ_COUNT;
			--ctx->obj;
			printf("Holding %s\n", obj_names[ctx->obj]);
		} else if (e->wheel.y < 0) {
			++ctx->obj;
			if (ctx->obj == OBJ_COUNT)
				ctx->obj = 0;
			printf("Holding %s\n", obj_names[ctx->obj]);
		}
	}
}
