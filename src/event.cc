/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "event.h"

#include "context.h"
#include "lighting.h"
#include "drop_entity.h"

void event(const SDL_Event *e, Context *ctx)
{
	if (ctx->mode == MODE_COMMAND) {
		if (e->type == SDL_KEYDOWN) {
			if (e->key.keysym.sym == SDLK_RETURN) {
				Tcl_Eval(ctx->tcl, ctx->cli->get_visible());
				const char *ret = Tcl_GetStringResult(ctx->tcl);
				if (ret && *ret)
					log_info("%s", ret);
				ctx->cli->push();
				ctx->mode = MODE_ROAM;
				SDL_StopTextInput();
			} else if (e->key.keysym.sym == SDLK_ESCAPE) {
				ctx->mode = MODE_ROAM;
				SDL_StopTextInput();
			} else if (e->key.keysym.sym == SDLK_DELETE) {
				ctx->cli->delete_forward();
			} else if (e->key.keysym.sym == SDLK_BACKSPACE) {
				ctx->cli->delete_backward();
			} else  if (e->key.keysym.sym == SDLK_HOME) {
				ctx->cli->first_char();
			} else  if (e->key.keysym.sym == SDLK_END) {
				ctx->cli->last_char();
			} else  if (e->key.keysym.sym == SDLK_UP) {
				ctx->cli->prev_line();
			} else  if (e->key.keysym.sym == SDLK_DOWN) {
				ctx->cli->next_line();
			} else  if (e->key.keysym.sym == SDLK_LEFT) {
				ctx->cli->prev_char();
			} else  if (e->key.keysym.sym == SDLK_RIGHT) {
				ctx->cli->next_char();
			}
		} else if (e->type == SDL_TEXTINPUT) {
			ctx->cli->append(e->text.text);
		} else if (e->type == SDL_TEXTEDITING) {
			log_warning("text editing event");
		}
		return;
	}

	if (e->type == SDL_KEYDOWN) {
		if (e->key.repeat) {
			return;
		} else if (e->key.keysym.sym == SDLK_w) {
			ctx->move.z0 = 1;
		} else if (e->key.keysym.sym == SDLK_a) {
			ctx->move.x0 = 1;
		} else if (e->key.keysym.sym == SDLK_s) {
			ctx->move.z1 = 1;
		} else if (e->key.keysym.sym == SDLK_d) {
			ctx->move.x1 = 1;
		} else if (e->key.keysym.sym == SDLK_LSHIFT) {
			ctx->move.y0 = 1;
		} else if (e->key.keysym.sym == SDLK_SPACE) {
			ctx->move.y1 = 1;
		} else if (e->key.keysym.sym == SDLK_r) {
			ctx->pick = 1;
		} else if (e->key.keysym.sym == SDLK_LCTRL) {
			ctx->run = 1;
		} else if (e->key.keysym.sym == SDLK_1) {
			ctx->tool = 0;
		} else if (e->key.keysym.sym == SDLK_2) {
			ctx->tool = 1;
		} else if (e->key.keysym.sym == SDLK_3) {
			ctx->tool = 2;
		} else if (e->key.keysym.sym == SDLK_4) {
			ctx->tool = 3;
		} else if (e->key.keysym.sym == SDLK_5) {
			ctx->tool = 4;
		} else if (e->key.keysym.sym == SDLK_6) {
			ctx->tool = 5;
		} else if (e->key.keysym.sym == SDLK_7) {
			ctx->tool = 6;
		} else if (e->key.keysym.sym == SDLK_8) {
			ctx->tool = 7;
		} else if (e->key.keysym.sym == SDLK_9) {
			ctx->tool = 8;
		} else if (e->key.keysym.sym == SDLK_q) {
			Item &s = ctx->inv[ctx->tool];
			if (s.num > 0) {
				DropEntity *e = new DropEntity(ctx, Item(s.obj, s.mat, 1));
				e->get_body()->set_p(ctx->cam->get_p());
				v3f v(0, 0, -.5);
				v = rotx(v, ctx->cam->get_r().x);
				v = roty(v, ctx->cam->get_r().y);
				e->get_body()->set_v(v);
				ctx->entities.push_back(e);
				--s.num;
			}
		} else if (e->key.keysym.sym == SDLK_ESCAPE) {
			ctx->ml->kill();
		} else if (e->key.keysym.sym == SDLK_o) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		} else if (e->key.keysym.sym == SDLK_p) {
			fprintf(stdout, "=== PROFILE START ===\n");
		//	profile_manager_reset(ctx->prof_mgr);
		}
	} else if (e->type == SDL_KEYUP) {
		if (e->key.repeat) {
			return;
		} else if (e->key.keysym.sym == SDLK_w) {
			ctx->move.z0 = 0;
		} else if (e->key.keysym.sym == SDLK_a) {
			ctx->move.x0 = 0;
		} else if (e->key.keysym.sym == SDLK_s) {
			ctx->move.z1 = 0;
		} else if (e->key.keysym.sym == SDLK_d) {
			ctx->move.x1 = 0;
		} else if (e->key.keysym.sym == SDLK_LSHIFT) {
			ctx->move.y0 = 0;
		} else if (e->key.keysym.sym == SDLK_SPACE) {
			ctx->move.y1 = 0;
		} else if (e->key.keysym.sym == SDLK_r) {
			ctx->pick = 0;
		} else if (e->key.keysym.sym == SDLK_o) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		} else if (e->key.keysym.sym == SDLK_p) {
			fprintf(stdout, "=== PROFILE DUMP ===\n");
			//profile_manager_dump(ctx->prof_mgr);
		} else if (e->key.keysym.sym == SDLK_l) {
			v3f p = ctx->player->get_p();
			v3f v = ctx->player->get_v();
			v3f r = ctx->player->get_r();
			log_info("pos %g,%g,%g", p.x, p.y, p.z);
			log_info("vel %g,%g,%g", v.x, v.y, v.z);
			log_info("rot %g,%g,%g", r.x, r.y, r.z);
			log_info("facing %d,%s", ctx->rotx, face_names[ctx->roty]);
		} else if (e->key.keysym.sym == SDLK_PERIOD) {
			ctx->mode = MODE_COMMAND;
			SDL_StartTextInput();
		}
	} else if (e->type == SDL_MOUSEBUTTONDOWN) {
		if (e->button.button == SDL_BUTTON_LEFT) {
			ctx->act = 1;
		} else if (e->button.button == SDL_BUTTON_RIGHT) {
			ctx->use = 1;
		}
	} else if (e->type == SDL_MOUSEBUTTONUP) {
		if (e->button.button == SDL_BUTTON_LEFT) {
			ctx->act = 0;
		} else if (e->button.button == SDL_BUTTON_RIGHT) {
			ctx->use = 0;
		}
	} else if (e->type == SDL_MOUSEWHEEL) {
		if (ctx->move.y0) {
			int mat = ctx->inv[ctx->tool].mat;
			if (e->wheel.y > 0) {
				do {
					if (mat == 0)
						mat = MAT_COUNT;
					--mat;
				} while (texcoord_from_mat[mat][0] == v2f(0, 0));
			} else if (e->wheel.y < 0) {
				do {
					++mat;
					if (mat == MAT_COUNT)
						mat = 0;
				} while (texcoord_from_mat[mat][0] == v2f(0, 0));
			}
			ctx->inv[ctx->tool].mat = mat;
		} else {
			if (e->wheel.y > 0) {
				if (ctx->tool == 0)
					ctx->tool = ctx->inv.size();
				--ctx->tool;
			} else if (e->wheel.y < 0) {
				++ctx->tool;
				if (ctx->tool == ctx->inv.size())
					ctx->tool = 0;
			}
		}
	}
	return;
}

