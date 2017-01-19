/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_LIGHTING_H_
#define SRC_LIGHTING_H_

#include "world.h"

void update_lighting(World *w, const box3ll &bb, box3ll *rbb);

#endif  // SRC_LIGHTING_H_

