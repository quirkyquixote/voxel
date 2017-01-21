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
#include <map>

#include "drop_entity.h"

int mkpath(const char *path, mode_t mode);

int main(int argc, char *argv[])
{
	populate_block_traits_table();
	populate_material_texcoord_table();

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
	ml->get_window()->set_render_callback([this](){(*this->renderer)();});

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
	space->set_impulse(0.001);
	space->set_terminal_speed(1);

	/* Initialize lighting */
	light = new Lighting(world);

	/* Initialize flowsim */
	//flowsim = flowsim(world);

	/* Create player */
	player = new PlayerEntity(this);
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
	if (!load_player()) {
		v2ll p = world->get_p();
		p.x += CHUNK_W * CHUNKS_PER_WORLD / 2;
		p.y += CHUNK_W * CHUNKS_PER_WORLD / 2;
		player->get_body()->set_p(v3f(p.x, WORLD_H, p.y));
	}
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
	return true;
}

void Context::save_all()
{
	v2ll p;
	save_world();
	save_player();
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

bool Context::load_player()
{
	bool ret = false;
	std::string path(dir);
	path += "/player.dat";
	int fd = open(path.c_str(), O_RDONLY, 0400);
	if (fd >= 0) {
		try {
			std::unique_ptr<sz_Tag> root(sz_read(fd));
			player->load(root.get());
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

void Context::save_player()
{
	std::string path(dir);
	path += "/player.dat";
	int fd = creat(path.c_str(), 0600);
	if (fd >= 0) {
		try {
			std::unique_ptr<sz_Tag> root(player->save());
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

void Context::update()
{
	if (!ml->get_window()->has_mouse_focus())
		return;
	space->step();
/*	if ((tick & 1) == 0)
		flowsim_step(flowsim);*/
	player->update();
	renderer->update_camera();
	update_chunks();
	for (auto &f : callback_list)
		f();
	++tick;
}
void Context::event(const SDL_Event &e)
{
	if (mode == MODE_COMMAND) {
		if (e.type == SDL_KEYDOWN) {
			if (e.key.keysym.sym == SDLK_RETURN) {
				Tcl_Eval(tcl, cli->get_visible());
				const char *ret = Tcl_GetStringResult(tcl);
				if (ret && *ret)
					log_info("%s", ret);
				cli->push();
				mode = MODE_ROAM;
				SDL_StopTextInput();
			} else if (e.key.keysym.sym == SDLK_ESCAPE) {
				mode = MODE_ROAM;
				SDL_StopTextInput();
			} else if (e.key.keysym.sym == SDLK_DELETE) {
				cli->delete_forward();
			} else if (e.key.keysym.sym == SDLK_BACKSPACE) {
				cli->delete_backward();
			} else  if (e.key.keysym.sym == SDLK_HOME) {
				cli->first_char();
			} else  if (e.key.keysym.sym == SDLK_END) {
				cli->last_char();
			} else  if (e.key.keysym.sym == SDLK_UP) {
				cli->prev_line();
			} else  if (e.key.keysym.sym == SDLK_DOWN) {
				cli->next_line();
			} else  if (e.key.keysym.sym == SDLK_LEFT) {
				cli->prev_char();
			} else  if (e.key.keysym.sym == SDLK_RIGHT) {
				cli->next_char();
			}
		} else if (e.type == SDL_TEXTINPUT) {
			cli->append(e.text.text);
		} else if (e.type == SDL_TEXTEDITING) {
			log_warning("text editing event");
		}
		return;
	}

	if (e.type == SDL_KEYDOWN) {
		if (e.key.repeat) {
			return;
		} else if (e.key.keysym.sym == SDLK_ESCAPE) {
			ml->kill();
		} else if (e.key.keysym.sym == SDLK_o) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		} else if (e.key.keysym.sym == SDLK_p) {
			fprintf(stdout, "=== PROFILE START ===\n");
		//	profile_manager_reset(prof_mgr);
		}
	} else if (e.type == SDL_KEYUP) {
		if (e.key.repeat) {
			return;
		} else if (e.key.keysym.sym == SDLK_o) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		} else if (e.key.keysym.sym == SDLK_p) {
			fprintf(stdout, "=== PROFILE DUMP ===\n");
			//profile_manager_dump(prof_mgr);
		} else if (e.key.keysym.sym == SDLK_PERIOD) {
			mode = MODE_COMMAND;
			SDL_StartTextInput();
		}
	}
	player->handle_event(e);
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
