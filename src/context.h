
#ifndef VOXEL_H_
#define VOXEL_H_

#include "chunk.h"
#include "terraform.h"
#include "main_loop.h"
#include "physics.h"
#include "flowsim.h"
#include "profile.h"
#include "vertex_buffer.h"
#include "tone_mapper.h"
#include "shader.h"
#include "camera.h"
#include "media.h"
#include "inventory.h"

enum {
	MODE_ROAM,
	MODE_CRAFT,
};

struct context {
	char *dir;
	struct main_loop *ml;
	struct world *w;
	struct profile_manager *prof_mgr;
	struct vertex_buffer *shard_vertex_buffer;
	struct tone_mapper *tone_mapper;
	struct shader *shader;
	struct camera *cam;
	GLuint tex_terrain;
	struct space *space;
	struct flowsim *flowsim;
	struct body *player;
	struct query cur;
	struct aab3c move;
	struct inventory *inv;
	struct list drops;
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

#endif
