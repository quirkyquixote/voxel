/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "context.h"
#include "lighting.h"
#include "drop_entity.h"

void Context::event(const SDL_Event &e)
{
	if (mode == MODE_COMMAND) {
		if (e.type == SDL_KEYDOWN) {
			if (e.key.keysym.sym == SDLK_RETURN) {
				Tcl_Eval(tcl, cli->get_visible());
				const char *ret = Tcl_GetStringResult(tcl);
				if (ret && *ret)
					log_info("%s", ret);
				cli->push();
				mode = MODE_ROAM;
				SDL_StopTextInput();
			} else if (e.key.keysym.sym == SDLK_ESCAPE) {
				mode = MODE_ROAM;
				SDL_StopTextInput();
			} else if (e.key.keysym.sym == SDLK_DELETE) {
				cli->delete_forward();
			} else if (e.key.keysym.sym == SDLK_BACKSPACE) {
				cli->delete_backward();
			} else  if (e.key.keysym.sym == SDLK_HOME) {
				cli->first_char();
			} else  if (e.key.keysym.sym == SDLK_END) {
				cli->last_char();
			} else  if (e.key.keysym.sym == SDLK_UP) {
				cli->prev_line();
			} else  if (e.key.keysym.sym == SDLK_DOWN) {
				cli->next_line();
			} else  if (e.key.keysym.sym == SDLK_LEFT) {
				cli->prev_char();
			} else  if (e.key.keysym.sym == SDLK_RIGHT) {
				cli->next_char();
			}
		} else if (e.type == SDL_TEXTINPUT) {
			cli->append(e.text.text);
		} else if (e.type == SDL_TEXTEDITING) {
			log_warning("text editing event");
		}
		return;
	}

	if (e.type == SDL_KEYDOWN) {
		if (e.key.repeat) {
			return;
		} else if (e.key.keysym.sym == SDLK_ESCAPE) {
			ml->kill();
		} else if (e.key.keysym.sym == SDLK_o) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		} else if (e.key.keysym.sym == SDLK_p) {
			fprintf(stdout, "=== PROFILE START ===\n");
		//	profile_manager_reset(prof_mgr);
		}
	} else if (e.type == SDL_KEYUP) {
		if (e.key.repeat) {
			return;
		} else if (e.key.keysym.sym == SDLK_o) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		} else if (e.key.keysym.sym == SDLK_p) {
			fprintf(stdout, "=== PROFILE DUMP ===\n");
			//profile_manager_dump(prof_mgr);
		} else if (e.key.keysym.sym == SDLK_PERIOD) {
			mode = MODE_COMMAND;
			SDL_StartTextInput();
		}
	}
	player->handle_event(e);
}

