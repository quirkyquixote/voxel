
#ifndef VOXEL_ROAM_H_
#define VOXEL_ROAM_H_

#include "voxel.h"

void roam_update(struct context *ctx);
void roam_render(struct context *ctx);
int roam_event(const SDL_Event *e, struct context *ctx);

#endif
