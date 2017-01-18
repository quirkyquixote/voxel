/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_RENDER_H_
#define SRC_RENDER_H_

#include "context.h"

void render(Context *ctx);

void render_obj(Context *ctx, int obj, int mat, GLfloat alpha);
void render_inventory(Context *ctx, const Inventory &inv, const v3ll &p);

#endif  // SRC_RENDER_H_
