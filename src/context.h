
#ifndef VOXEL_H_
#define VOXEL_H_

#include <tcl.h>

#include <list>

#include "chunk.h"
#include "terraform.h"
#include "main_loop.h"
#include "physics.h"
#include "profile.h"
#include "vertex_buffer.h"
#include "tone_mapper.h"
#include "shader.h"
#include "camera.h"
#include "media.h"
#include "inventory.h"
#include "cli.h"
#include "tcl_commands.h"
#include "roaming_entity.h"

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
	World *w;
	VertexBuffer *shard_vertex_buffer;
	VertexBuffer *obj_vertex_buffer;
	ToneMapper *tone_mapper;
	Shader *shader;
	Camera *cam;
	GLuint tex_terrain;
	GLuint tex_font;
	Space *space;
	Body *player;
	Query cur;
	box3c move;
	Inventory inv;
	std::list<RoamingEntity*> entities;
	char mode;
	char act;
	char use;
	char pick;
	char run;
	char tool;
	char roty;
	char rotx;
	int chunks_per_tick;
	uint64_t tick;
};

unsigned long long next_id(void);
unsigned long long max_id(void);

#define log_info(fmt,...) fprintf(stderr, "INFO: %s: " fmt "\n", __func__, ##__VA_ARGS__)
#define log_warning(fmt,...) fprintf(stderr, "WARNING: %s: " fmt "\n", __func__, ##__VA_ARGS__)
#define log_error(fmt,...) fprintf(stderr, "ERROR: %s: " fmt "\n", __func__, ##__VA_ARGS__)

#endif
