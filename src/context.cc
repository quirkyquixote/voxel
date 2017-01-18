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

int load_all(Context *ctx);
int save_all(Context *ctx);

int load_world(World *w, const char *dir);
int save_world(World *w, const char *dir);

int load_chunk(Chunk *c, const char *dir);
int save_chunk(Chunk *c, const char *dir);

int mkpath(const char *path, mode_t mode);

int main(int argc, char *argv[])
{
	block_traits_init();
	Context *ctx = new Context("foo");
	load_all(ctx);
	ctx->ml->run();
	save_all(ctx);
	delete ctx;
	return 0;
}

Context::Context(const char *dir)
	: dir(dir)
{
	world = new World(this);
	//prof_mgr = profile_manager();
	chunks_per_tick = 1;

	/* Setup main loop */
	ml = new MainLoop(30);
	ml->set_event_callback([this](const SDL_Event &e){event(e);});
	ml->set_update_callback([this](){update();});
	ml->set_window(new Window("voxel", 0, 0, 1280, 768, 0));

	/* Create renderer */
	renderer = new Renderer(this);
	ml->get_window()->set_render_callback(*renderer);

	/* Initialize Tcl */
	tcl = Tcl_CreateInterp();
	Tcl_CreateObjCommand(tcl, "help", cmd_help, this, NULL);
	Tcl_CreateObjCommand(tcl, "ls", cmd_ls, this, NULL);
	Tcl_CreateObjCommand(tcl, "give", cmd_give, this, NULL);
	Tcl_CreateObjCommand(tcl, "take", cmd_take, this, NULL);
	Tcl_CreateObjCommand(tcl, "q", cmd_query, this, NULL);
	Tcl_CreateObjCommand(tcl, "a", cmd_seta, this, NULL);
	Tcl_CreateObjCommand(tcl, "b", cmd_setb, this, NULL);
	Tcl_CreateObjCommand(tcl, "box", cmd_box, this, NULL);
	Tcl_CreateObjCommand(tcl, "hbox", cmd_hbox, this, NULL);
	Tcl_CreateObjCommand(tcl, "walls", cmd_walls, this, NULL);
	Tcl_CreateObjCommand(tcl, "relit", cmd_relit, this, NULL);
	cli = new CommandLine();

	/* Initialize physics */
	space = new Space(world);
	space->set_gravity(-0.05);
	space->set_iterations(2);
	space->set_impulse(0.001);
	space->set_terminal_speed(1);

	/* Initialize flowsim */
	//flowsim = flowsim(world);
}

Context::~Context()
{
	delete world;
	delete ml;
	delete renderer;
	/* close Tcl */
	delete cli;
	delete space;
}

int load_all(Context *ctx)
{
	int x, z;
	Chunk *c;
	int from_scratch;

	mkpath(ctx->dir, 0700);
	from_scratch = load_world(ctx->world, ctx->dir);
	for (x = 0; x < CHUNKS_PER_WORLD; ++x) {
		for (z = 0; z < CHUNKS_PER_WORLD; ++z) {
			c = ctx->world->get_chunk(v2ll(x, z));
			v2ll p = ctx->world->get_p();
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
	   update_lighting(ctx->world, box3ll(0, 0, 0, WORLD_W, WORLD_H, WORLD_D), NULL);*/
	v2ll p = ctx->world->get_p();
	p.x += CHUNK_W * CHUNKS_PER_WORLD / 2;
	p.y += CHUNK_W * CHUNKS_PER_WORLD / 2;
	ctx->player = new PlayerEntity(ctx);
	ctx->player->get_body()->set_p(v3f(p.x, WORLD_H, p.y));
	return 0;
}

int save_all(Context *ctx)
{
	v2ll p;
	if (save_world(ctx->world, ctx->dir) != 0)
		return -1;
	for (p.x = 0; p.x < CHUNKS_PER_WORLD; ++p.x)
		for (p.y = 0; p.y < CHUNKS_PER_WORLD; ++p.y)
			save_chunk(ctx->world->get_chunk(p), ctx->dir);
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
