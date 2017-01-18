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
		} else if (e.key.keysym.sym == SDLK_w) {
			player->set_move_z0(1);
		} else if (e.key.keysym.sym == SDLK_a) {
			player->set_move_x0(1);
		} else if (e.key.keysym.sym == SDLK_s) {
			player->set_move_z1(1);
		} else if (e.key.keysym.sym == SDLK_d) {
			player->set_move_x1(1);
		} else if (e.key.keysym.sym == SDLK_LSHIFT) {
			player->set_move_y0(1);
		} else if (e.key.keysym.sym == SDLK_SPACE) {
			player->set_move_y1(1);
		} else if (e.key.keysym.sym == SDLK_r) {
			player->set_pick(1);
		} else if (e.key.keysym.sym == SDLK_LCTRL) {
			player->set_run(1);
		} else if (e.key.keysym.sym == SDLK_1) {
			player->set_tool(0);
		} else if (e.key.keysym.sym == SDLK_2) {
			player->set_tool(1);
		} else if (e.key.keysym.sym == SDLK_3) {
			player->set_tool(2);
		} else if (e.key.keysym.sym == SDLK_4) {
			player->set_tool(3);
		} else if (e.key.keysym.sym == SDLK_5) {
			player->set_tool(4);
		} else if (e.key.keysym.sym == SDLK_6) {
			player->set_tool(5);
		} else if (e.key.keysym.sym == SDLK_7) {
			player->set_tool(6);
		} else if (e.key.keysym.sym == SDLK_8) {
			player->set_tool(7);
		} else if (e.key.keysym.sym == SDLK_9) {
			player->set_tool(8);
		} else if (e.key.keysym.sym == SDLK_q) {
			Item &s = (*player->get_items())[player->get_tool()];
			if (s.num > 0) {
				DropEntity *d = new DropEntity(this, Item(s.obj, s.mat, 1));
				d->get_body()->set_p(renderer->get_cam()->get_p());
				v3f v(0, 0, -.5);
				v = rotx(v, renderer->get_cam()->get_r().x);
				v = roty(v, renderer->get_cam()->get_r().y);
				d->get_body()->set_v(v);
				entities.push_back(d);
				--s.num;
			}
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
		} else if (e.key.keysym.sym == SDLK_w) {
			player->set_move_z0(0);
		} else if (e.key.keysym.sym == SDLK_a) {
			player->set_move_x0(0);
		} else if (e.key.keysym.sym == SDLK_s) {
			player->set_move_z1(0);
		} else if (e.key.keysym.sym == SDLK_d) {
			player->set_move_x1(0);
		} else if (e.key.keysym.sym == SDLK_LSHIFT) {
			player->set_move_y0(0);
		} else if (e.key.keysym.sym == SDLK_SPACE) {
			player->set_move_y1(0);
		} else if (e.key.keysym.sym == SDLK_r) {
			player->set_pick(0);
		} else if (e.key.keysym.sym == SDLK_o) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		} else if (e.key.keysym.sym == SDLK_p) {
			fprintf(stdout, "=== PROFILE DUMP ===\n");
			//profile_manager_dump(prof_mgr);
		} else if (e.key.keysym.sym == SDLK_l) {
			v3f p = player->get_body()->get_p();
			v3f v = player->get_body()->get_v();
			v3f r = player->get_body()->get_r();
			log_info("pos %g,%g,%g", p.x, p.y, p.z);
			log_info("vel %g,%g,%g", v.x, v.y, v.z);
			log_info("rot %g,%g,%g", r.x, r.y, r.z);
			//log_info("facing %d,%s", rot.x, face_names[rot.y]);
		} else if (e.key.keysym.sym == SDLK_PERIOD) {
			mode = MODE_COMMAND;
			SDL_StartTextInput();
		}
	} else if (e.type == SDL_MOUSEBUTTONDOWN) {
		if (e.button.button == SDL_BUTTON_LEFT) {
			player->set_act(1);
		} else if (e.button.button == SDL_BUTTON_RIGHT) {
			player->set_use(1);
		}
	} else if (e.type == SDL_MOUSEBUTTONUP) {
		if (e.button.button == SDL_BUTTON_LEFT) {
			player->set_act(0);
		} else if (e.button.button == SDL_BUTTON_RIGHT) {
			player->set_use(0);
		}
	} else if (e.type == SDL_MOUSEWHEEL) {
		if (player->get_move_y0()) {
			auto &mat = (*player->get_items())[player->get_tool()].mat;
			if (e.wheel.y > 0) {
				do {
					if (mat == 0)
						mat = MAT_COUNT;
					--mat;
				} while (texcoord_from_mat[mat][0] == v2f(0, 0));
			} else if (e.wheel.y < 0) {
				do {
					++mat;
					if (mat == MAT_COUNT)
						mat = 0;
				} while (texcoord_from_mat[mat][0] == v2f(0, 0));
			}
		} else {
			int tool = player->get_tool();
			if (e.wheel.y > 0) {
				if (tool == 0)
					tool = player->get_items()->size();
				--tool;
			} else if (e.wheel.y < 0) {
				++tool;
				if (tool == player->get_items()->size())
					tool = 0;
			}
			player->set_tool(tool);
		}
	}
	return;
}

