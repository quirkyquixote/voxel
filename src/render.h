
#ifndef VOXEL_RENDER_H_
#define VOXEL_RENDER_H_

#include "context.h"

void render(void *data);

void render_obj(struct context *ctx, int obj, int mat, GLfloat alpha);
void render_inventory(struct context *ctx, struct array *inv, struct v3ll p);

#endif
