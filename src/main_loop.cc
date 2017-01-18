
#include "main_loop.h"

#include <assert.h>

#include "fps_manager.h"
#include "context.h"

Window::Window(const char *title, int x, int y, int sx, int sy, int flags)
{
	sdl_window = SDL_CreateWindow(title, x, y, sx, sy, flags | SDL_WINDOW_OPENGL);
	if (sdl_window == NULL) {
		log_error("%s", SDL_GetError());
		abort();
	}
	gl_context = SDL_GL_CreateContext(sdl_window);
	if (gl_context == NULL) {
		log_error("%s", SDL_GetError());
		abort();
	}
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	//	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 3);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
}

Window::~Window()
{
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(sdl_window);
}

MainLoop::MainLoop(int fps)
	: fps(fps), keep_going(false)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK)) {
		log_error("%s", SDL_GetError());
		abort();
	}
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024) != 0) {
		log_error("%s", Mix_GetError());
		abort();
	}
	Mix_Init(0);
}

MainLoop::~MainLoop()
{
	SDL_Quit();
}

void MainLoop::run()
{
	FpsManager fps_mgr(fps);
	SDL_Event e;

	keep_going = true;

	while (keep_going) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT)
				keep_going = false;
			else
				event_func(&e);
		}
		update_func();
		window->redraw();
		fps_mgr.sync();
	}
}

