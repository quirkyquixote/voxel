
#include "fps_manager.h"

#include <assert.h>

#include "SDL2/SDL.h"

#define MAX_FPS 200
#define MIN_FPS 1

struct fps_manager *fps_manager(int fps)
{
	assert((fps >= MIN_FPS) && (fps <= MAX_FPS));
	struct fps_manager *f = calloc(1, sizeof(*f));
	f->t0 = SDL_GetTicks();
	f->t1 = f->t0;
	f->frame_count = 0;
	f->fps = fps;
}

void fps_manager_destroy(struct fps_manager *f)
{
	free(f);
}

int fps_manager_sync(struct fps_manager *f)
{
	int target_ticks;
	int time_elapsed = 0;
	int ticks;

	f->frame_count++;

	ticks = SDL_GetTicks();
	time_elapsed = ticks - f->t1;
	f->t1 = ticks;
	target_ticks = f->t0 + (f->frame_count * 1000) / f->fps;

	if (ticks <= target_ticks) {
		SDL_Delay(target_ticks - ticks);
	} else {
		f->t0 = f->t1;
		f->frame_count = 0;
	}

	return time_elapsed;
}
