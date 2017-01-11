

#ifndef VOXEL_FLOWSIM_H_
#define VOXEL_FLOWSIM_H_

#include "v3.h"
#include "chunk.h"
#include "stack.h"
#include "list.h"

struct flowsim {
	struct world *w;
	struct list volumes;
};

struct fs_volume {
	int id;
	struct world *w;
	struct list volumes;
	struct stack *top_layers;
	float roaming;
};

struct fs_layer {
	int id;
	struct fs_volume *v;
	struct stack *cells;
	struct stack *falls;
	float top;
	int y;
	int is_top;
};

struct fs_layer *fs_layer(struct fs_volume *v, int y);
void fs_layer_destroy(struct fs_layer *l);

struct fs_volume *fs_volume(struct world *w);
void fs_volume_destroy(struct fs_volume *v);
void fs_volume_step(struct fs_volume *v);

struct flowsim *flowsim(struct world *w);
void flowsim_destroy(struct flowsim *f);
void flowsim_step(struct flowsim *f);
int flowsim_add(struct flowsim *f, struct v3ll p, float v);

#endif
