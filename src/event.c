
#include "event.h"

#include "context.h"
#include "lighting.h"
#include "drop_entity.h"

void event(const SDL_Event *e, void *data)
{
	struct context *ctx = data;

	if (ctx->mode == MODE_COMMAND) {
		if (e->type == SDL_KEYDOWN) {
			if (e->key.keysym.sym == SDLK_RETURN) {
				Tcl_Eval(ctx->tcl, ctx->cli->visible);
				const char *ret = Tcl_GetStringResult(ctx->tcl);
				if (ret && *ret)
					log_info("%s", ret);
				cli_push(ctx->cli);
				ctx->mode = MODE_ROAM;
				SDL_StopTextInput();
			} else if (e->key.keysym.sym == SDLK_ESCAPE) {
				ctx->mode = MODE_ROAM;
				SDL_StopTextInput();
			} else if (e->key.keysym.sym == SDLK_DELETE) {
				cli_delete_forward(ctx->cli);
			} else if (e->key.keysym.sym == SDLK_BACKSPACE) {
				cli_delete_backward(ctx->cli);
			} else  if (e->key.keysym.sym == SDLK_HOME) {
				cli_first_char(ctx->cli);
			} else  if (e->key.keysym.sym == SDLK_END) {
				cli_last_char(ctx->cli);
			} else  if (e->key.keysym.sym == SDLK_UP) {
				cli_prev_line(ctx->cli);
			} else  if (e->key.keysym.sym == SDLK_DOWN) {
				cli_next_line(ctx->cli);
			} else  if (e->key.keysym.sym == SDLK_LEFT) {
				cli_prev_char(ctx->cli);
			} else  if (e->key.keysym.sym == SDLK_RIGHT) {
				cli_next_char(ctx->cli);
			}
		} else if (e->type == SDL_TEXTINPUT) {
			cli_append(ctx->cli, e->text.text);
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
			struct item s = inventory_get(ctx->inv, ctx->tool);
			if (s.num > 0) {
				struct drop_entity *d = drop_entity(ctx, item(s.obj, s.mat, 1));
				body_set_position(d->roaming.body, ctx->cam->p);
				struct v3f v = v3f(0, 0, -.5);
				v = v3_rotx(v, ctx->cam->r.x);
				v = v3_roty(v, ctx->cam->r.y);
				body_set_velocity(d->roaming.body, v);
				inventory_set_num(ctx->inv, ctx->tool, s.num - 1);
			}
		} else if (e->key.keysym.sym == SDLK_ESCAPE) {
			main_loop_kill(ctx->ml);
		} else if (e->key.keysym.sym == SDLK_o) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		} else if (e->key.keysym.sym == SDLK_p) {
			fprintf(stdout, "=== PROFILE START ===\n");
			profile_manager_reset(ctx->prof_mgr);
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
		} else if (e->key.keysym.sym == SDLK_e) {
			if (ctx->bench != NULL) {
				array_destroy(ctx->bench);
				ctx->bench = NULL;
			} else if (ctx->cur.face != -1) {
				ctx->bench = inventory(9);
				ctx->bench_p = ctx->cur.p;
			}
		} else if (e->key.keysym.sym == SDLK_o) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		} else if (e->key.keysym.sym == SDLK_p) {
			fprintf(stdout, "=== PROFILE DUMP ===\n");
			profile_manager_dump(ctx->prof_mgr);
		} else if (e->key.keysym.sym == SDLK_p) {
			printf("at %g,%g,%g; rot %g,%g; facing %d,%s\n",
					ctx->player->p.x, ctx->player->p.y, ctx->player->p.x,
					ctx->player->r.x, ctx->player->r.y,
					ctx->rotx, face_names[ctx->roty]);
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
			int mat = inventory_get_mat(ctx->inv, ctx->tool);
			if (e->wheel.y > 0) {
				do {
					if (mat == 0)
						mat = MAT_COUNT;
					--mat;
				} while (v2_eql(texcoord_from_mat[mat][0], v2f(0, 0)));
			} else if (e->wheel.y < 0) {
				do {
					++mat;
					if (mat == MAT_COUNT)
						mat = 0;
				} while (v2_eql(texcoord_from_mat[mat][0], v2f(0, 0)));
			}
			inventory_set_mat(ctx->inv, ctx->tool, mat);
		} else {
			if (e->wheel.y > 0) {
				if (ctx->tool == 0)
					ctx->tool = ctx->inv->size;
				--ctx->tool;
			} else if (e->wheel.y < 0) {
				++ctx->tool;
				if (ctx->tool == ctx->inv->size)
					ctx->tool = 0;
			}
		}
	}
	return;
}

