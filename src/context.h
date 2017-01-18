/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_CONTEXT_H_
#define SRC_CONTEXT_H_

#include <tcl.h>

#include <list>

#include "chunk.h"
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

	void event(const SDL_Event *e);
	void update();

	void spill_inventory(const v3ll &p);
	void drop_block(const v3ll &p);
	void update_chunks();
	void update_entities();
};

unsigned long long next_id(void);
unsigned long long max_id(void);

#define log_info(fmt, ...) \
	fprintf(stderr, "INFO: %s: " fmt "\n", __func__, ##__VA_ARGS__)
#define log_warning(fmt, ...) \
	fprintf(stderr, "WARNING: %s: " fmt "\n", __func__, ##__VA_ARGS__)
#define log_error(fmt, ...) \
	fprintf(stderr, "ERROR: %s: " fmt "\n", __func__, ##__VA_ARGS__)

#endif  // SRC_CONTEXT_H_
