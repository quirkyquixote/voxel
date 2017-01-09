

#ifndef VOXEL_FLUID_H_
#define VOXEL_FLUID_H_

#include "v3.h"
#include "chunk.h"
#include "stack.h"

#define MAX_GROUPS 1024

struct flowsim {
	struct world *w;
	struct stack *heads;
	struct stack *heads2;
	struct stack *s;
	int vol[WORLD_W][WORLD_H][WORLD_D];
	int gmask[WORLD_W][WORLD_H][WORLD_D];
	struct stack *gs;
};

struct flowsim *flowsim(struct world *w);
void flowsim_destroy(struct flowsim *f);
int flowsim_step(struct flowsim *f);

static inline void flowsim_add_head(struct flowsim *f, struct v3ll p)
{
	stack_push(f->heads, &p);
}

#endif

