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
#include <thread>

#include "drop_entity.h"
#include "option_parser.h"

int mkpath(const char *path, mode_t mode);

int main(int argc, char *argv[])
{
	bool help_flag;
	bool version_flag;

	Option options[] = {
		Option('h', "help", "Show help options", &help_flag),
		Option('v', "version", "Print version", &version_flag),
	};

	int i = parse_arguments(argv, argc, options, 2);

	if (help_flag) {
		printf("usage: %s [OPTIONS] [<path>]\n", argv[0]);
		printf("\n");
		printf("Options:\n");
		print_help(options, 2);
		exit(EXIT_SUCCESS);
	}

	if (version_flag) {
		printf("%s\n", VOXEL_VERSION);
		exit(EXIT_SUCCESS);
	}

	populate_block_traits_table();
	populate_material_texcoord_table();

	std::unique_ptr<Context> ctx(new Context(i < argc ? argv[i] : "foo"));
	ctx->load_all();
	ctx->ml->run();
	ctx->save_all();
	return 0;
}

Context::Context(const char *dir)
	: dir(dir)
{
	world.reset(new World(this));
	// prof_mgr = profile_manager();

	/* Setup main loop */
	ml.reset(new MainLoop(30));
	ml->set_event_callback([this](const SDL_Event &e){event(e);});
	ml->set_update_callback([this](){update();});
	ml->set_window(new Window("voxel", 0, 0, 1280, 768, 0));

	/* Create renderer */
	renderer.reset(new Renderer(this));
	ml->get_window()->set_render_callback([this](){(*this->renderer)();});

	/* Initialize Tcl */
	tcl = Tcl_CreateInterp();
	Tcl_CreateObjCommand(tcl, "help", cmd_help, this, NULL);
	Tcl_CreateObjCommand(tcl, "ls", cmd_ls, this, NULL);
	Tcl_CreateObjCommand(tcl, "tp", cmd_tp, this, NULL);
	Tcl_CreateObjCommand(tcl, "give", cmd_give, this, NULL);
	Tcl_CreateObjCommand(tcl, "take", cmd_take, this, NULL);
	Tcl_CreateObjCommand(tcl, "q", cmd_query, this, NULL);
	Tcl_CreateObjCommand(tcl, "a", cmd_seta, this, NULL);
	Tcl_CreateObjCommand(tcl, "b", cmd_setb, this, NULL);
	Tcl_CreateObjCommand(tcl, "box", cmd_box, this, NULL);
	Tcl_CreateObjCommand(tcl, "hbox", cmd_hbox, this, NULL);
	Tcl_CreateObjCommand(tcl, "walls", cmd_walls, this, NULL);
	Tcl_CreateObjCommand(tcl, "relit", cmd_relit, this, NULL);
	Tcl_CreateObjCommand(tcl, "replace", cmd_replace, this, NULL);
	Tcl_CreateObjCommand(tcl, "terraform", cmd_terraform, this, NULL);
	cli.reset(new CommandLine());

	/* Initialize physics */
	space.reset(new Space(world.get()));
	space->set_gravity(-1.5);
	space->set_impulse(0.001);
	space->set_terminal_speed(30);

	/* Initialize lighting */
	light.reset(new Lighting(world.get()));

	/* Initialize flowsim */
	// flowsim = flowsim(world);

	/* Create player */
	player.reset(new PlayerEntity(this));
}

Context::~Context()
{
	/* close Tcl */
}

bool Context::load_all()
{
	static const box2ll chunk_box(0, 0, World::CHUNK_NUM - 1, World::CHUNK_NUM - 1);

	mkpath(dir, 0700);
	load_world();
	if (!load_player()) {
		v2ll p = world->get_p();
		p.x += Chunk::W * World::CHUNK_NUM / 2;
		p.y += Chunk::W * World::CHUNK_NUM / 2;
		player->get_body()->set_p(v3f(p.x, World::H, p.y));
	}
	for (auto r : chunk_box) {
		auto p = (r * Chunk::W) + world->get_p();
		Chunk *c = new Chunk(this, p);
		if (!load_chunk(c)) {
			terraform(0, c);
			c->set_flags(Chunk::UNLIT);
		}
		c->set_flags(Chunk::UNRENDERED);
		world->set_chunk_at_block(p, c);
	}
	return true;
}

void Context::save_all()
{
	save_world();
	save_player();
	for (auto p : box2ll(0, 0, World::CHUNK_NUM - 1, World::CHUNK_NUM - 1))
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
	char name[64];
	v2ll p = c->get_p();
	snprintf(name, sizeof(name), "%016llx%016llx", p.x >> 4, p.y >> 4);
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
	}
	return ret;
}

void Context::save_chunk(Chunk *c)
{
	if (c->get_flags() & Chunk::UNLOADED)
		return;

	char name[64];
	v2ll p = c->get_p();
	snprintf(name, sizeof(name), "%016llx%016llx", p.x >> 4, p.y >> 4);
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
	static const box2ll chunk_box(0, 0, World::CHUNK_NUM - 1, World::CHUNK_NUM - 1);

	std::vector<std::thread> tasks;
	std::multimap<int, Chunk *> out_of_date;
	v2ll p(player->get_body()->get_p().x, player->get_body()->get_p().z);
	v2ll world_p((p & ~0xfLL) - v2ll(World::W, World::D) / 2LL);
	if (world_p != world->get_p()) {
		// log_info("off center!");
		world->set_p(world_p);
		int chunks_moved = 0;
		for (auto r : chunk_box) {
			auto p = (r * Chunk::W) + world_p;
			Chunk *c = world->get_chunk_at_block(p);
			if (c->get_p() != p) {
				tasks.emplace_back([this, c](){
						save_chunk(c);
						delete c;
						});
				c = new Chunk(this, p);
				c->set_flags(Chunk::UNLOADED);
				world->set_chunk_at_block(p, c);
				++chunks_moved;
			}
		}
		// log_info("%d chunks moved", chunks_moved);
	}

	for (auto q : chunk_box) {
		Chunk *c = world->get_chunk(q);
		if (c->get_flags() != 0) {
			int d = dist(c->get_p(), p);
			out_of_date.emplace(d, c);
		}
	}

	int max_t = SDL_GetTicks() + 500 / ml->get_fps();
	for (auto &iter : out_of_date) {
		Chunk *c = iter.second;
		v2ll p = c->get_p();
		// log_info("Update chunk %lld,%lld; priority:%d", p.x, p.y, iter.first);
		if ((c->get_flags() & Chunk::UNLOADED) != 0) {
			// log_info("load from file");
			/* load this chunk */
			if (!load_chunk(c)) {
				terraform(0, c);
				c->set_flags(Chunk::UNLIT);
			} else {
				c->set_flags(Chunk::UNRENDERED);
			}
			c->unset_flags(Chunk::UNLOADED);
			if (SDL_GetTicks() > max_t)
				break;
		}
		if ((c->get_flags() & Chunk::UNLIT) != 0) {
			// log_info("lit up");
			light->init(box2ll(p.x, p.y, p.x + Chunk::W - 1, p.y + Chunk::D - 1));
			c->unset_flags(Chunk::UNLIT);
			c->set_flags(Chunk::UNRENDERED);
			if (SDL_GetTicks() > max_t)
				break;
		}
		if ((c->get_flags() & Chunk::UNRENDERED) != 0) {
			bool neighbours_loaded = true;
			for (auto o : { v2ll(Chunk::W, 0), v2ll(-Chunk::W, 0),
					v2ll(0, Chunk::D), v2ll(0, -Chunk::D) }) {
				v2ll p2 = p + o;
				Chunk *c2 = world->get_chunk_at_block(p2);
				if (c2->get_p() != p2
						|| (c2->get_flags() & Chunk::UNLOADED) != 0
						|| (c2->get_flags() & Chunk::UNLIT) != 0) {
					neighbours_loaded = false;
					break;
				}
			}
			if (!neighbours_loaded)
				continue;
			// log_info("update vbos");
			for (int k = 0; k < Chunk::SHARD_NUM; ++k)
				renderer->update_shard(c->get_shard(k)->get_id(),
						v3ll(p.x, k * Shard::H, p.y));
			c->unset_flags(Chunk::UNRENDERED);
			if (SDL_GetTicks() > max_t)
				break;
		}
	}
	//		log_info("update %d of %zd chunks (max: %d)", i, out_of_date.size(), chunks_per_tick);
	for (auto &t : tasks)
		t.join();
}

void Context::update()
{
	if (!ml->get_window()->has_mouse_focus())
		return;
	space->step(1.0 / ml->get_fps());
	/*	if ((tick & 1) == 0)
		flowsim_step(flowsim);*/
	player->update();
	renderer->update_camera();
	update_chunks();
	for (auto f : callback_list)
		(*f)();
	++tick;
}
void Context::event(const SDL_Event &e)
{
	if (mode == MODE_COMMAND) {
		if (e.type == SDL_KEYDOWN) {
			if (e.key.keysym.sym == SDLK_RETURN) {
				scrollback.push_front(strdup(cli->get_visible()));
				Tcl_Eval(tcl, cli->get_visible());
				const char *ret = Tcl_GetStringResult(tcl);
				if (ret && *ret)
					scrollback.push_front(strdup(ret));
				while (scrollback.size() > 10)
					scrollback.pop_back();
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
