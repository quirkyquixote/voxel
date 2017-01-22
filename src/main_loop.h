/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_MAIN_LOOP_H_
#define SRC_MAIN_LOOP_H_

#include <functional>

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_mixer.h"
#define GL_GLEXT_PROTOTYPES 1
#include "SDL2/SDL_opengl.h"
#include "GL/glext.h"
#undef GL_GLEXT_PROTOTYPES
#include "GL/glu.h"

#include "noncopyable.h"

class Window : public NonCopyable {
 public:
	typedef void RenderFunc(void);

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

	inline void set_render_callback(const std::function<RenderFunc> &func)
	{
		render_func = func;
	}

	inline SDL_Window *get_sdl_window() { return sdl_window; }
	inline SDL_GLContext get_gl_context() { return gl_context; }

 private:
	SDL_Window *sdl_window;
	SDL_GLContext gl_context;
	std::function<RenderFunc> render_func;
};

class MainLoop : public NonCopyable {
 public:
	typedef void EventFunc(const SDL_Event &);
	typedef void UpdateFunc(void);

	explicit MainLoop(int fps);
	~MainLoop();

	void run();

	inline void kill() { keep_going = false; }
	inline void set_window(Window *w) { window = w; }
	inline Window *get_window() { return window; }

	inline void set_update_callback(const std::function<UpdateFunc> &func)
	{
		update_func = func;
	}

	inline void set_event_callback(const std::function<EventFunc> &func)
	{
		event_func = func;
	}

 private:
	std::function<UpdateFunc> update_func;
	std::function<EventFunc> event_func;
	Window *window;
	int fps;
	int keep_going;
};


#endif  // SRC_MAIN_LOOP_H_

