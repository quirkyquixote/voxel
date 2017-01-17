

#include "context.h"

#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "lighting.h"
#include "render.h"
#include "update.h"
#include "event.h"

#include "bench_entity.h"
#include "board_entity.h"
#include "crate_entity.h"
#include "pipe_entity.h"
#include "drop_entity.h"

int load_all(struct context *ctx);
int save_all(struct context *ctx);

int load_world(struct world *w, const char *dir);
int save_world(struct world *w, const char *dir);

int load_chunk(struct chunk *c, const char *dir);
int save_chunk(struct chunk *c, const char *dir);

int mkpath(const char *path, mode_t mode);

int main(int argc, char *argv[])
{
	struct context *ctx;

	ctx = calloc(1, sizeof(*ctx));
	ctx->dir = "foo";
	block_traits_init();
	ctx->w = world(ctx);
	ctx->prof_mgr = profile_manager();

	/* Populate entity traits list */
	list_init(&ctx->entity_traits);
	list_append(&ctx->entity_traits, &bench_traits.list);
	list_append(&ctx->entity_traits, &crate_traits.list);
	list_append(&ctx->entity_traits, &pipe_traits.list);
	list_append(&ctx->entity_traits, &board_traits.list);
	list_append(&ctx->entity_traits, &drop_traits.list);
	ctx->chunks_per_tick = 1;

	/* Setup main loop */
	ctx->ml = main_loop(30);
	main_loop_set_event_callback(ctx->ml, event, ctx);
	main_loop_set_update_callback(ctx->ml, update, ctx);
	main_loop_add_window(ctx->ml, window("voxel", 0, 0, 1280, 768, 0));
	window_set_render_callback(ctx->ml->windows, render, ctx);

	/* Initialize Tcl */
	ctx->tcl = Tcl_CreateInterp();
	Tcl_CreateObjCommand(ctx->tcl, "help", cmd_help, ctx, NULL);
	Tcl_CreateObjCommand(ctx->tcl, "ls", cmd_ls, ctx, NULL);
	Tcl_CreateObjCommand(ctx->tcl, "give", cmd_give, ctx, NULL);
	Tcl_CreateObjCommand(ctx->tcl, "take", cmd_take, ctx, NULL);
	Tcl_CreateObjCommand(ctx->tcl, "q", cmd_query, ctx, NULL);
	Tcl_CreateObjCommand(ctx->tcl, "a", cmd_seta, ctx, NULL);
	Tcl_CreateObjCommand(ctx->tcl, "b", cmd_setb, ctx, NULL);
	Tcl_CreateObjCommand(ctx->tcl, "box", cmd_box, ctx, NULL);
	Tcl_CreateObjCommand(ctx->tcl, "hbox", cmd_hbox, ctx, NULL);
	Tcl_CreateObjCommand(ctx->tcl, "walls", cmd_walls, ctx, NULL);
	Tcl_CreateObjCommand(ctx->tcl, "relit", cmd_relit, ctx, NULL);
	ctx->cli = cli();

	/* Load textures */
	ctx->tex_terrain = texture("data/materials.png");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	ctx->tex_font = texture("data/font.png");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	/* Initialize physics */
	ctx->space = space(ctx->w);
	space_set_gravity(ctx->space, v3f(0, -0.05, 0));
	space_set_iterations(ctx->space, 2);
	space_set_impulse(ctx->space, 0.001);
	space_set_terminal_speed(ctx->space, 1);

	/* Initialize flowsim */
	ctx->flowsim = flowsim(ctx->w);

	/* Setup camera */
	ctx->cam = camera();
	camera_set_max_distance(ctx->cam, 1024);
	camera_set_aspect_ratio(ctx->cam, 1280.0 / 768.0);

	/* Create vertex_buffers */
	ctx->shard_vertex_buffer = vertex_buffer(SHARDS_PER_WORLD);
	ctx->obj_vertex_buffer = vertex_buffer(1);

	{
		struct array *a = array(sizeof(struct vertex));
		for (int i = 0; i < MAT_COUNT; ++i) {
			struct v2f lt = v2f(1, 1);
			struct v2f mt[6];
			int tilted[6];
			struct v3f p;
			texcoord_up(i, mt, tilted);
			p = v3f(0, 0, 0);
			vertices_add(a, vertices_face_dn, 6, p, lt, mt, tilted);
			vertices_add(a, vertices_face_lf, 6, p, lt, mt, tilted);
			vertices_add(a, vertices_face_bk, 6, p, lt, mt, tilted);
			vertices_add(a, vertices_face_up, 6, p, lt, mt, tilted);
			vertices_add(a, vertices_face_rt, 6, p, lt, mt, tilted);
			vertices_add(a, vertices_face_ft, 6, p, lt, mt, tilted);

			vertices_add(a, vertices_face_dn, 6, p, lt, mt, tilted);
			vertices_add(a, vertices_slab_dn, 30, p, lt, mt, tilted);

			vertices_add(a, vertices_face_dn, 6, p, lt, mt, tilted);
			vertices_add(a, vertices_face_lf, 6, p, lt, mt, tilted);
			vertices_add(a, vertices_stairs_dl, 42, p, lt, mt, tilted);

			vertices_add(a, vertices_pane_z, 36, p, lt, mt, tilted);
		}
		vertex_buffer_update(ctx->obj_vertex_buffer, 0, a->data, a->size);
		array_destroy(a);
	}

	ctx->tone_mapper = tone_mapper(1. / 30., 16);
	ctx->shader = shader("data/shader.vert", "data/shader.frag");

	/* Load world */
	load_all(ctx);

	/* Run */
	main_loop_run(ctx->ml);

	/* Save the world */
	save_all(ctx);

	/* Destroy everything */
	world_destroy(ctx->w);
	main_loop_destroy(ctx->ml);
	vertex_buffer_destroy(ctx->shard_vertex_buffer);
	tone_mapper_destroy(ctx->tone_mapper);
	profile_manager_destroy(ctx->prof_mgr);
	free(ctx);
	return 0;
}

int load_all(struct context *ctx)
{
	int x, z;
	struct chunk *c;
	struct v3f p;
	int from_scratch;

	mkpath(ctx->dir, 0700);
	from_scratch = load_world(ctx->w, ctx->dir);
	for (x = 0; x < CHUNKS_PER_WORLD; ++x) {
		for (z = 0; z < CHUNKS_PER_WORLD; ++z) {
			c = ctx->w->chunks[x][z];
			c->x = ctx->w->x + x * CHUNK_W;
			c->z = ctx->w->z + z * CHUNK_D;
			if (load_chunk(c, ctx->dir) != 0) {
				terraform(0, c);
				c->flags |= CHUNK_UNLIT;
			}
		}
	}
	/*
	   if (from_scratch)
	   update_lighting(ctx->w, box3ll(0, 0, 0, WORLD_W, WORLD_H, WORLD_D), NULL);*/
	p.x = ctx->w->x + CHUNK_W * CHUNKS_PER_WORLD / 2;
	p.y = CHUNK_H;
	p.z = ctx->w->z + CHUNK_W * CHUNKS_PER_WORLD / 2;
	ctx->player = body(ctx->space);
	body_set_position(ctx->player, p);
	body_set_size(ctx->player, v2f(0.325, 0.825));
	body_set_step_size(ctx->player, 1);
	ctx->inv = inventory(9);
	ctx->tool = 0;
	list_init(&ctx->entities);
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
	union sz_tag *root;
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
	if (world_load(w, root) != 0)
		return -1;
	sz_destroy(root);
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
	union sz_tag *root;

	snprintf(path, sizeof(path), "%s/%06x%06x.dat", dir, c->x / CHUNK_W, c->z / CHUNK_D);
	fd = open(path, O_RDONLY, 0400);
	if (fd < 0)
		return -1;
	if (sz_read(fd, &root) != 0)
		return -1;
	close(fd);
	if (chunk_load(c, root) != 0)
		return -1;
	sz_destroy(root);
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

static unsigned long long id = 0;

unsigned long long next_id(void)
{
	return id++;
}

unsigned long long max_id(void)
{
	return id;
}

static int do_mkdir(const char *path, mode_t mode)
{
	struct stat     st;
	int             status = 0;

	if (stat(path, &st) != 0) {
		/* Directory does not exist. EEXIST for race condition */
		if (mkdir(path, mode) != 0 && errno != EEXIST)
			status = -1;
	}
	else if (!S_ISDIR(st.st_mode)) {
		errno = ENOTDIR;
		status = -1;
	}

	return status;
}

/**
 * ** mkpath - ensure all directories in path exist
 * ** Algorithm takes the pessimistic view and works top-down to ensure
 * ** each directory in path exists, rather than optimistically creating
 * ** the last element and working backwards.
 * */
int mkpath(const char *path, mode_t mode)
{
	char           *pp;
	char           *sp;
	int             status;
	char           *copypath = strdup(path);

	status = 0;
	pp = copypath;
	while (status == 0 && (sp = strchr(pp, '/')) != 0) {
		if (sp != pp) {
			/* Neither root nor double slash in path */
			*sp = '\0';
			status = do_mkdir(copypath, mode);
			*sp = '/';
		}
		pp = sp + 1;
	}
	if (status == 0)
		status = do_mkdir(path, mode);
	free(copypath);
	return status;
}
