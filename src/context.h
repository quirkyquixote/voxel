
#ifndef VOXEL_H_
#define VOXEL_H_

#include "chunk.h"
#include "terraform.h"
#include "main_loop.h"
#include "physics.h"
#include "profile.h"
#include "renderer.h"
#include "camera.h"
#include "media.h"
#include "inventory.h"

enum {
	MODE_ROAM,
	MODE_CRAFT,
};

enum {
	OBJ_BLOCK,	/* full block */
	OBJ_SLAB,	/* half block */
	OBJ_STAIRS,	/* stairs */
	OBJ_PANE,	/* thin pane in the middle */
	OBJ_WORKBENCH,	/* module for a workbench */
	OBJ_CRATE,	/* basic container */
	OBJ_COUNT
};

struct context {
	char *dir;
	struct main_loop *ml;
	struct world *w;
	struct profile_manager *prof_mgr;
	struct renderer *shard_renderer;
	struct camera *cam;
	GLuint tex_terrain;
	struct space *space;
	struct body *player;
	struct query cur;
	struct aab3c move;
	struct inventory *inv;
	char mode;
	char act;
	char use;
	char pick;
	char run;
	char tool;
	char roty;
	char rotx;
	int chunks_per_tick;
};

extern const char *obj_names[];
extern const char *mat_names[];
extern const char *face_names[];

#endif
