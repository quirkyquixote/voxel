
#ifndef VOXEL_MAIN_LOOP_H_
#define VOXEL_MAIN_LOOP_H_

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_mixer.h"
#include "SDL2/SDL_opengl.h"
#include "GL/glu.h"

#include "resource.h"

struct window {
	struct window *next;
	SDL_Window *sdl_window;
	SDL_GLContext gl_context;
	void(*render_func)(void *);
	void *render_data;
};

struct main_loop {
	void(*update_func)(void *);
	void *update_data;
	void(*event_func)(const SDL_Event *, void *);
	void *event_data;
	struct window *windows;
	struct resource_factory *textures;
	struct resource_factory *samples;
	int fps;
	int keep_going;
};

struct window *window(const char *title, int x, int y, int w, int h, int flags);
void window_destroy(struct window *w);
void window_redraw(struct window *w);
int window_has_keyboard_focus(struct window *w);
int window_has_mouse_focus(struct window *w);
void window_on_render(struct window *w, void(*func)(void *), void *data);

struct main_loop *main_loop(int fps);
void main_loop_destroy(struct main_loop *ml);
void main_loop_run(struct main_loop *ml);
void main_loop_kill(struct main_loop *ml);
void main_loop_add_window(struct main_loop *ml, struct window *w);
void main_loop_remove_window(struct main_loop *ml, struct window *w);
void main_loop_on_update(struct main_loop *ml, void(*func)(void *), void *data);
void main_loop_on_event(struct main_loop *ml, void(*func)(const SDL_Event *, void *), void *data);

#endif

