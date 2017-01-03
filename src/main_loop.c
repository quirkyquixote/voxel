
#include "main_loop.h"

#include <assert.h>

#include "fps_manager.h"

struct window *window(const char *title, int x, int y, int sx, int sy, int flags)
{
	struct window *w = calloc(1, sizeof(*w));
	w->sdl_window = SDL_CreateWindow(title, x, y, sx, sy, flags | SDL_WINDOW_OPENGL);
	if (w->sdl_window == NULL) {
	        fprintf(stderr, "%s:%s\n", __func__, SDL_GetError());
		return NULL;
	}
	w->gl_context = SDL_GL_CreateContext(w->sdl_window);
	if (w->gl_context == NULL) {
		fprintf(stderr, "%s:%s\n", __func__, SDL_GetError());
		return NULL;
	}
	return w;
}

void window_destroy(struct window *w)
{
    SDL_GL_DeleteContext(w->gl_context);
    SDL_DestroyWindow(w->sdl_window);
}

void window_redraw(struct window *w)
{
    SDL_GL_MakeCurrent(w->sdl_window, w->gl_context);
    w->render_func(w->render_data);
    SDL_GL_SwapWindow(w->sdl_window);
}

int window_has_keyboard_focus(struct window *w)
{
    return SDL_GetKeyboardFocus() == w->sdl_window;
}

int window_has_mouse_focus(struct window *w)
{
    return SDL_GetMouseFocus() == w->sdl_window;
}

void window_on_render(struct window *w, void(*func)(void *), void *data)
{
	w->render_func = func;
	w->render_data = data;
}

struct main_loop *main_loop(int fps)
{
	struct main_loop *ml = calloc(1, sizeof(*ml));
	ml->fps = fps;
	ml->keep_going = 0;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK)) {
		fprintf(stderr, "%s:%s\n", __func__, SDL_GetError());
		return NULL;
	}
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024) != 0) {
		fprintf(stderr, "%s:%s\n", __func__, Mix_GetError());
		return NULL;
	}
	Mix_Init(0);
	return ml;
}

void main_loop_destroy(struct main_loop *ml)
{
	SDL_Quit();
}

void main_loop_run(struct main_loop *ml)
{
	struct fps_manager *fps_mgr;
	SDL_Event e;
	struct window *w;

	ml->keep_going = 1;
	fps_mgr = fps_manager(ml->fps);

	while (ml->keep_going) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT)
				ml->keep_going = 0;
			else
				ml->event_func(&e, ml->event_data);
		}
		ml->update_func(ml->update_data);
		for (w = ml->windows; w != NULL; w = w->next)
			window_redraw(w);
		fps_manager_sync(fps_mgr);
	}
	fps_manager_destroy(fps_mgr);
}

void main_loop_kill(struct main_loop *ml)
{
	ml->keep_going = 0;
}

void main_loop_add_window(struct main_loop *ml, struct window* w)
{
	assert(ml->keep_going == 0);
	w->next = ml->windows;
	ml->windows = w;
}

void main_loop_remove_window(struct main_loop *ml, struct window *w)
{
	struct window *prev;
	assert(ml->keep_going == 0);
	if (w == ml->windows) {
		ml->windows = w->next;
		return;
	}
	for (prev = ml->windows; prev != NULL; prev = prev->next) {
		if (prev->next == w) {
			prev->next = w->next;
			return;
		}
	}
}

void main_loop_on_update(struct main_loop *ml, void(*func)(void *), void *data)
{
	ml->update_func = func;
	ml->update_data = data;
}

void main_loop_on_event(struct main_loop *ml, void(*func)(const SDL_Event *, void *), void *data)
{
	ml->event_func = func;
	ml->event_data = data;
}

