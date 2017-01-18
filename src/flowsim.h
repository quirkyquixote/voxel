

#ifndef VOXEL_FLOWSIM_H_
#define VOXEL_FLOWSIM_H_

#include "v3.h"
#include "chunk.h"
#include "list.h"

struct flowsim {
	struct world *w;
	struct list volumes;
};

struct fs_volume {
	unsigned long long id;
	struct world *w;
	struct list volumes;
	struct list layers;
	float v;
	float top;
};

struct fs_layer {
	unsigned long long id;
	struct fs_volume *v;
	struct list layers;
	struct array *cells;
	struct array *falls;
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
