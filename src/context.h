/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_CONTEXT_H_
#define SRC_CONTEXT_H_

#include <tcl.h>

#include <deque>

#include "world.h"
#include "terraform.h"
#include "main_loop.h"
#include "physics.h"
#include "profile.h"
#include "renderer.h"
#include "inventory.h"
#include "lighting.h"
#include "cli.h"
#include "tcl_commands.h"
#include "roaming_entity.h"
#include "player_entity.h"
#include "callback.h"

enum {
	MODE_ROAM,
	MODE_COMMAND,
};

class Context {
 public:
	const char *dir;
	std::unique_ptr<MainLoop> ml;
	Tcl_Interp *tcl;
	std::unique_ptr<CommandLine> cli;
	std::deque<char *> scrollback;
	std::unique_ptr<World> world;
	std::unique_ptr<Renderer> renderer;
	std::unique_ptr<Space> space;
	std::unique_ptr<Lighting> light;
	std::unique_ptr<PlayerEntity> player;
	PtrList<RoamingEntity> entities;
	PtrList<Callback> callback_list;
	char mode;
	int max_update_time;
	uint64_t tick;

	explicit Context(const char *dir);
	~Context();

	void event(const SDL_Event &e);
	void update();

	void spill_inventory(const v3ll &p);
	void drop_block(const v3ll &p);

	bool load_all();
	void save_all();

	inline void add_callback(Callback *cb) { callback_list.push_back(cb); }

 private:
	void update_chunks();
	void update_entities();

	bool load_world();
	bool load_player();
	bool load_chunk(Chunk *c);

	void save_world();
	void save_player();
	void save_chunk(Chunk *c);
};

unsigned long long next_id(void);
unsigned long long max_id(void);

#endif  // SRC_CONTEXT_H_
