
#ifndef VOXEL_RENDER_H_
#define VOXEL_RENDER_H_

#include "context.h"

void render(Context *ctx);

void render_obj(Context *ctx, int obj, int mat, GLfloat alpha);
void render_inventory(Context *ctx, const Inventory &inv, const v3ll &p);

#endif
