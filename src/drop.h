
#ifndef VOXEL_DROP_H_
#define VOXEL_DROP_H_

#include "context.h"

struct drop {
	struct list list;
	struct body *body;
	uint8_t obj;
	uint8_t mat;
	uint8_t num;
	int ttl;
};

struct drop *drop(struct context *ctx, uint8_t obj, uint8_t mat, uint8_t num);
void drop_destroy(struct drop *d);

#endif

