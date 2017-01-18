
#ifndef VOXEL_MAIN_LOOP_H_
#define VOXEL_MAIN_LOOP_H_

#include <functional>

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_mixer.h"
#define GL_GLEXT_PROTOTYPES 1
#include "SDL2/SDL_opengl.h"
#include "GL/glext.h"
#undef GL_GLEXT_PROTOTYPES
#include "GL/glu.h"

class Window {
public:
	Window(const char *title, int x, int y, int w, int h, int flags);
	~Window();

	inline void redraw()
	{
		SDL_GL_MakeCurrent(sdl_window, gl_context);
		render_func();
		SDL_GL_SwapWindow(sdl_window);
	}

	inline bool has_keyboard_focus()
	{
		return SDL_GetKeyboardFocus() == sdl_window;
	}

	inline bool has_mouse_focus()
	{
		return SDL_GetMouseFocus() == sdl_window;
	}

	inline void set_render_callback(const std::function<void(void)> &func)
	{
		render_func = func;
	}

	inline SDL_Window *get_sdl_window() { return sdl_window; }
	inline SDL_GLContext get_gl_context() { return gl_context; }

private:
	SDL_Window *sdl_window;
	SDL_GLContext gl_context;
	std::function<void(void)> render_func;
};

class MainLoop {
public:
	MainLoop(int fps);
	~MainLoop();

	void run();

	inline void kill() { keep_going = false; }
	inline void set_window(Window *w) { window = w; }
	inline Window *get_window() { return window; }

	inline void set_update_callback(const std::function<void(void)> &func)
	{
		update_func = func;
	}

	inline void set_event_callback(const std::function<void(const SDL_Event *)> &func)
	{
		event_func = func;
	}

private:
	std::function<void(void)> update_func;
	std::function<void(const SDL_Event *)> event_func;
	Window *window;
	int fps;
	int keep_going;
};


#endif

