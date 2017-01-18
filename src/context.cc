/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

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
#include "update.h"

int load_all(Context *ctx);
int save_all(Context *ctx);

int load_world(World *w, const char *dir);
int save_world(World *w, const char *dir);

int load_chunk(Chunk *c, const char *dir);
int save_chunk(Chunk *c, const char *dir);

int mkpath(const char *path, mode_t mode);

int main(int argc, char *argv[])
{
	Context *ctx = new Context();

	ctx->dir = "foo";
	block_traits_init();
	ctx->w = new World(ctx);
	//ctx->prof_mgr = profile_manager();
	ctx->chunks_per_tick = 1;
	populate_material_texcoord_table();

	/* Setup main loop */
	ctx->ml = new MainLoop(30);
	ctx->ml->set_event_callback([ctx](const SDL_Event *e){ctx->event(e);});
	ctx->ml->set_update_callback(std::bind(update, ctx));
	ctx->ml->set_window(new Window("voxel", 0, 0, 1280, 768, 0));
	ctx->ml->get_window()->set_render_callback([ctx](){ctx->render();});

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
	ctx->cli = new CommandLine();

	/* Load textures */
	ctx->tex_terrain = texture("data/materials.png");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	ctx->tex_font = texture("data/font.png");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	/* Initialize physics */
	ctx->space = new Space(ctx->w);
	ctx->space->set_gravity(-0.05);
	ctx->space->set_iterations(2);
	ctx->space->set_impulse(0.001);
	ctx->space->set_terminal_speed(1);

	/* Initialize flowsim */
	//ctx->flowsim = flowsim(ctx->w);

	/* Setup camera */
	ctx->cam = new Camera();
	ctx->cam->set_max_distance(1024);
	ctx->cam->set_aspect_ratio(1280.0 / 768.0);

	/* Create vertex_buffers */
	ctx->shard_vertex_buffer = new VertexBuffer(SHARDS_PER_WORLD);
	ctx->obj_vertex_buffer = new VertexBuffer(1);

	{
		std::vector<Vertex> a;
		for (int i = 0; i < MAT_COUNT; ++i) {
			v2f lt(1, 1);
			v2f mt[6];
			int tilted[6];
			v3f p;
			texcoord_up(i, mt, tilted);
			p = v3f(0, 0, 0);
			vertices_add(&a, vertices_face_dn, 6, p, lt, mt, tilted);
			vertices_add(&a, vertices_face_lf, 6, p, lt, mt, tilted);
			vertices_add(&a, vertices_face_bk, 6, p, lt, mt, tilted);
			vertices_add(&a, vertices_face_up, 6, p, lt, mt, tilted);
			vertices_add(&a, vertices_face_rt, 6, p, lt, mt, tilted);
			vertices_add(&a, vertices_face_ft, 6, p, lt, mt, tilted);

			vertices_add(&a, vertices_face_dn, 6, p, lt, mt, tilted);
			vertices_add(&a, vertices_slab_dn, 30, p, lt, mt, tilted);

			vertices_add(&a, vertices_face_dn, 6, p, lt, mt, tilted);
			vertices_add(&a, vertices_face_lf, 6, p, lt, mt, tilted);
			vertices_add(&a, vertices_stairs_dl, 42, p, lt, mt, tilted);

			vertices_add(&a, vertices_pane_z, 36, p, lt, mt, tilted);
		}
		ctx->obj_vertex_buffer->update(0, a.data(), a.size());
	}

	ctx->tone_mapper = new ToneMapper(1. / 30., 16);
	ctx->shader = new Shader("data/shader.vert", "data/shader.frag");

	/* Load world */
	load_all(ctx);

	/* Run */
	ctx->ml->run();

	/* Save the world */
	save_all(ctx);

	/* Destroy everything */
	delete ctx->w;
	delete ctx->ml;
	delete ctx->shard_vertex_buffer;
	delete ctx->tone_mapper;
//	profile_manager_destroy(ctx->prof_mgr);
	delete ctx;
	return 0;
}

int load_all(Context *ctx)
{
	int x, z;
	Chunk *c;
	int from_scratch;

	mkpath(ctx->dir, 0700);
	from_scratch = load_world(ctx->w, ctx->dir);
	for (x = 0; x < CHUNKS_PER_WORLD; ++x) {
		for (z = 0; z < CHUNKS_PER_WORLD; ++z) {
			c = ctx->w->get_chunk(v2ll(x, z));
			v2ll p = ctx->w->get_p();
			p.x += x * CHUNK_W;
			p.y += z * CHUNK_D;
			c->set_p(p);
			if (load_chunk(c, ctx->dir) != 0) {
				terraform(0, c);
				c->set_flags(CHUNK_UNLIT);
			}
		}
	}
	/*
	   if (from_scratch)
	   update_lighting(ctx->w, box3ll(0, 0, 0, WORLD_W, WORLD_H, WORLD_D), NULL);*/
	v2ll p = ctx->w->get_p();
	p.x += CHUNK_W * CHUNKS_PER_WORLD / 2;
	p.y += CHUNK_W * CHUNKS_PER_WORLD / 2;
	ctx->player = ctx->space->create_body();
	ctx->player->set_p(v3f(p.x, WORLD_H, p.y));
	ctx->player->set_size(v2f(0.325, 0.825));
	ctx->player->set_step_size(1);
	ctx->inv.resize(9);
	ctx->tool = 0;
	return 0;
}

int save_all(Context *ctx)
{
	v2ll p;
	if (save_world(ctx->w, ctx->dir) != 0)
		return -1;
	for (p.x = 0; p.x < CHUNKS_PER_WORLD; ++p.x)
		for (p.y = 0; p.y < CHUNKS_PER_WORLD; ++p.y)
			save_chunk(ctx->w->get_chunk(p), ctx->dir);
	return 0;
}

int load_world(World *w, const char *dir)
{
	sz_Tag *root;
	char path[256];
	int fd;

	snprintf(path, sizeof(path), "%s/world.dat", dir);
	fd = open(path, O_RDONLY, 0400);
	if (fd < 0) {
		perror(path);
		return -1;
	}
	try {
		root = sz_read(fd);
		w->load(root);
		delete root;
	} catch (sz_Exception &ex) {
		return -1;
	}
	close(fd);
	return 0;
}

int save_world(World *w, const char *dir)
{
	sz_Tag *root;
	char path[256];
	int fd;

	root = w->save();
	snprintf(path, sizeof(path), "%s/world.dat", dir);
	fd = creat(path, 0600);
	if (fd < 0) {
		perror(path);
		return -1;
	}
	try {
		sz_write(fd, root);
	} catch (sz_Exception &ex) {
		return -1;
	}
	close(fd);
	delete root;
	return 0;
}

int load_chunk(Chunk *c, const char *dir)
{
	char path[256];
	int fd;
	sz_Tag *root;
	v2ll p;

	p = c->get_p();
	snprintf(path, sizeof(path), "%s/%06llx%06llx.dat", dir, p.x / CHUNK_W, p.y / CHUNK_D);
	fd = open(path, O_RDONLY, 0400);
	if (fd < 0)
		return -1;
	try {
		root = sz_read(fd);
		c->load(root);
		delete root;
	} catch (sz_Exception &ex) {
		return -1;
	}
	close(fd);
	return 0;
}

int save_chunk(Chunk *c, const char *dir)
{
	char path[256];
	int fd;
	sz_Tag *root;
	v2ll p;

	root = c->save();
	p = c->get_p();
	snprintf(path, sizeof(path), "%s/%06llx%06llx.dat", dir, p.x / CHUNK_W, p.y / CHUNK_D);
	fd = creat(path, 0600);
	if (fd < 0) {
		perror(path);
		return -1;
	}
	try {
		sz_write(fd, root);
	} catch (sz_Exception &ex) {
		return -1;
	}
	close(fd);
	delete root;
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
