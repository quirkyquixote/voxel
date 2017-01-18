/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "fps_manager.h"

#include <assert.h>

#include "SDL2/SDL.h"

#define MAX_FPS 200
#define MIN_FPS 1

FpsManager::FpsManager(int fps)
	: fps(fps)
{
	assert((fps >= MIN_FPS) && (fps <= MAX_FPS));
	t0 = SDL_GetTicks();
	t1 = t0;
	frame_count = 0;
}

FpsManager::~FpsManager()
{
}

int FpsManager::sync()
{
	int target_ticks;
	int time_elapsed = 0;
	int ticks;

	frame_count++;

	ticks = SDL_GetTicks();
	time_elapsed = ticks - t1;
	t1 = ticks;
	target_ticks = t0 + (frame_count * 1000) / fps;

	if (ticks <= target_ticks) {
		SDL_Delay(target_ticks - ticks);
	} else {
		t0 = t1;
		frame_count = 0;
	}

	return time_elapsed;
}
