/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_EVENT_H_
#define SRC_EVENT_H_

#include <SDL2/SDL.h>

#include "context.h"

void event(const SDL_Event *e, Context *ctx);

#endif  // SRC_EVENT_H_

