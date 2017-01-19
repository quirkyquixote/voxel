/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_CONTEXT_H_
#define SRC_CONTEXT_H_

#include <tcl.h>

#include <list>

#include "world.h"
#include "terraform.h"
#include "main_loop.h"
#include "physics.h"
#include "profile.h"
#include "renderer.h"
#include "inventory.h"
#include "cli.h"
#include "tcl_commands.h"
#include "roaming_entity.h"
#include "player_entity.h"

enum {
	MODE_ROAM,
	MODE_COMMAND,
};

class Context {
 public:
	const char *dir;
	MainLoop *ml;
	Tcl_Interp *tcl;
	CommandLine *cli;
	World *world;
	Renderer *renderer;
	Space *space;
	PlayerEntity *player;
	std::list<RoamingEntity*> entities;
	char mode;
	int chunks_per_tick;
	uint64_t tick;

	Context(const char *dir);
	~Context();

	void event(const SDL_Event &e);
	void update();

	void spill_inventory(const v3ll &p);
	void drop_block(const v3ll &p);
	void update_chunks();
	void update_entities();

	bool load_all();
	bool load_world();
	bool load_player();
	bool load_chunk(Chunk *c);

	void save_all();
	void save_world();
	void save_player();
	void save_chunk(Chunk *c);

};

unsigned long long next_id(void);
unsigned long long max_id(void);

#endif  // SRC_CONTEXT_H_
