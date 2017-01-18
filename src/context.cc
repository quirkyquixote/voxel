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

#include <memory>

#include "lighting.h"

int mkpath(const char *path, mode_t mode);

int main(int argc, char *argv[])
{
	block_traits_init();
	Context *ctx = new Context("foo");
	ctx->load_all();
	ctx->ml->run();
	ctx->save_all();
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

bool Context::load_all()
{
	int x, z;
	Chunk *c;

	mkpath(dir, 0700);
	load_world();
	for (x = 0; x < CHUNKS_PER_WORLD; ++x) {
		for (z = 0; z < CHUNKS_PER_WORLD; ++z) {
			c = world->get_chunk(v2ll(x, z));
			v2ll p = world->get_p();
			p.x += x * CHUNK_W;
			p.y += z * CHUNK_D;
			c->set_p(p);
			if (!load_chunk(c)) {
				terraform(0, c);
				c->set_flags(CHUNK_UNLIT);
			}
		}
	}
	v2ll p = world->get_p();
	p.x += CHUNK_W * CHUNKS_PER_WORLD / 2;
	p.y += CHUNK_W * CHUNKS_PER_WORLD / 2;
	player = new PlayerEntity(this);
	player->get_body()->set_p(v3f(p.x, WORLD_H, p.y));
	return true;
}

void Context::save_all()
{
	v2ll p;
	save_world();
	for (p.x = 0; p.x < CHUNKS_PER_WORLD; ++p.x)
		for (p.y = 0; p.y < CHUNKS_PER_WORLD; ++p.y)
			save_chunk(world->get_chunk(p));
}

bool Context::load_world()
{
	bool ret = false;
	std::string path(dir);
	path += "/world.dat";
	int fd = open(path.c_str(), O_RDONLY, 0400);
	if (fd >= 0) {
		try {
			std::unique_ptr<sz_Tag> root(sz_read(fd));
			world->load(root.get());
			ret = true;
		} catch (sz_Exception &ex) {
			/* do nothing */
		}
		close(fd);
	} else {
		log_error("%s: %s", path.c_str(), strerror(errno));
	}
	return ret;
}

void Context::save_world()
{
	std::string path(dir);
	path += "/world.dat";
	int fd = creat(path.c_str(), 0600);
	if (fd >= 0) {
		try {
			std::unique_ptr<sz_Tag> root(world->save());
			sz_write(fd, root.get());
		} catch(sz_Exception &ex) {
			/* do nothing */
		}
		close(fd);
	} else {
		log_error("%s: %s", path.c_str(), strerror(errno));
	}
}

bool Context::load_chunk(Chunk *c)
{
	bool ret;
	char name[16];
	v2ll p = c->get_p();
	snprintf(name, sizeof(name), "%06llx%06llx", p.x / CHUNK_W, p.y / CHUNK_D);
	std::string path(dir);
	path += "/";
	path += name;
	path += ".dat";
	int fd = open(path.c_str(), O_RDONLY, 0400);
	if (fd >= 0) {
		try {
			std::unique_ptr<sz_Tag> root(sz_read(fd));
			c->load(root.get());
			ret = true;
		} catch (sz_Exception &ex) {
			/* do nothing */
		}
		close(fd);
	} else {
		log_error("%s: %s", path.c_str(), strerror(errno));
	}
	return ret;
}

void Context::save_chunk(Chunk *c)
{
	char name[16];
	v2ll p = c->get_p();
	snprintf(name, sizeof(name), "%06llx%06llx", p.x / CHUNK_W, p.y / CHUNK_D);
	std::string path(dir);
	path += "/";
	path += name;
	path += ".dat";
	int fd = creat(path.c_str(), 0600);
	if (fd >= 0) {
		try {
			std::unique_ptr<sz_Tag> root(c->save());
			sz_write(fd, root.get());
		} catch (sz_Exception &ex) {
			/* do nothing */
		}
		close(fd);
	} else {
		log_error("%s: %s", path.c_str(), strerror(errno));
	}
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
