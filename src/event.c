
#include "event.h"

#include "context.h"
#include "drop.h"
#include "lighting.h"

void event(const SDL_Event *e, void *data)
{
	struct context *ctx = data;

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
		} else if (e->key.keysym.sym == SDLK_LCTRL) {
			ctx->run = 1;
		} else if (e->key.keysym.sym == SDLK_1) {
			ctx->inv->slots[ctx->tool].mat = 0;
		} else if (e->key.keysym.sym == SDLK_2) {
			ctx->inv->slots[ctx->tool].mat = 1;
		} else if (e->key.keysym.sym == SDLK_3) {
			ctx->inv->slots[ctx->tool].mat = 2;
		} else if (e->key.keysym.sym == SDLK_4) {
			ctx->inv->slots[ctx->tool].mat = 3;
		} else if (e->key.keysym.sym == SDLK_5) {
			ctx->inv->slots[ctx->tool].mat = 4;
		} else if (e->key.keysym.sym == SDLK_6) {
			ctx->inv->slots[ctx->tool].mat = 5;
		} else if (e->key.keysym.sym == SDLK_7) {
			ctx->inv->slots[ctx->tool].mat = 6;
		} else if (e->key.keysym.sym == SDLK_8) {
			ctx->inv->slots[ctx->tool].mat = 7;
		} else if (e->key.keysym.sym == SDLK_9) {
			ctx->inv->slots[ctx->tool].mat = 8;
		} else if (e->key.keysym.sym == SDLK_q) {
			if (ctx->inv->slots[ctx->tool].num > 0) {
				struct drop *d = drop(ctx,
						ctx->inv->slots[ctx->tool].obj,
						ctx->inv->slots[ctx->tool].obj,
						1);
				body_set_position(d->body, ctx->cam->p);
				struct v3f v = v3f(0, 0, -.5);
				v = v3_rotx(v, ctx->cam->r.x);
				v = v3_roty(v, ctx->cam->r.y);
				body_set_velocity(d->body, v);
				--ctx->inv->slots[ctx->tool].num;
			}
		} else if (e->key.keysym.sym == SDLK_ESCAPE) {
			main_loop_kill(ctx->ml);
		} else if (e->key.keysym.sym == SDLK_o) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		} else if (e->key.keysym.sym == SDLK_p) {
			fprintf(stdout, "=== PROFILE START ===\n");
			profile_manager_reset(ctx->prof_mgr);
		} else if (e->key.keysym.sym == SDLK_l) {
			struct aab3ll bb, bb2;
			bb.x0 = ctx->w->x;
			bb.y0 = WORLD_H - 1;
			bb.z0 = ctx->w->z;
			bb.x1 = bb.x0 + WORLD_W;
			bb.y1 = bb.y0 + 1;
			bb.z1 = bb.z0 + WORLD_W;
			update_lighting(ctx->w, bb, &bb2);
			world_set_flags(ctx->w, bb2, CHUNK_UNRENDERED);
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
		} else if (e->key.keysym.sym == SDLK_o) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		} else if (e->key.keysym.sym == SDLK_p) {
			fprintf(stdout, "=== PROFILE DUMP ===\n");
			profile_manager_dump(ctx->prof_mgr);
		} else if (e->key.keysym.sym == SDLK_q) {
			if (ctx->cur.face == -1)
				return;
			printf("looking at %d,%d,%d; %.02g,%.02g,%.02g; %s; %s %s; light %d\n",
					ctx->cur.p.x, ctx->cur.p.y, ctx->cur.p.z,
					ctx->cur.q.x, ctx->cur.q.y, ctx->cur.q.z,
					face_names[ctx->cur.face],
					mat_names[WORLD_AT(ctx->w, mat, ctx->cur.p.x, ctx->cur.p.y, ctx->cur.p.z)],
					shape_names[WORLD_AT(ctx->w, shape, ctx->cur.p.x, ctx->cur.p.y, ctx->cur.p.z)],
					WORLD_AT(ctx->w, light, ctx->cur.p.x, ctx->cur.p.y, ctx->cur.p.z));
			struct inventory *inv = WORLD_AT(ctx->w, data, ctx->cur.p.x, ctx->cur.p.y, ctx->cur.p.z);
			if (inv != NULL) {
				printf("inventory: [");
				for (int i = 0; i < inv->size; ++i) {
					if (i > 0)
						printf(",");
					if (inv->slots[i].num == 0)
						printf("");
					else
						printf("%s %s %d",
								mat_names[inv->slots[i].mat],
								obj_names[inv->slots[i].obj],
								inv->slots[i].num);
				}
				printf("]\n");
			}
		} else if (e->key.keysym.sym == SDLK_p) {
			printf("at %g,%g,%g; rot %g,%g; facing %d,%s\n",
					ctx->player->p.x, ctx->player->p.y, ctx->player->p.x,
					ctx->player->r.x, ctx->player->r.y,
					ctx->rotx, face_names[ctx->roty]);
		}
	} else if (e->type == SDL_MOUSEBUTTONDOWN) {
		if (e->button.button == SDL_BUTTON_LEFT) {
			ctx->act = 1;
		} else if (e->button.button == SDL_BUTTON_RIGHT) {
			ctx->use = 1;
		} else if (e->button.button == SDL_BUTTON_MIDDLE) {
			ctx->pick = 1;
		}
	} else if (e->type == SDL_MOUSEBUTTONUP) {
		if (e->button.button == SDL_BUTTON_LEFT) {
			ctx->act = 0;
		} else if (e->button.button == SDL_BUTTON_RIGHT) {
			ctx->use = 0;
		} else if (e->button.button == SDL_BUTTON_MIDDLE) {
			ctx->pick = 0;
		}
	} else if (e->type == SDL_MOUSEWHEEL) {
		if (ctx->move.y0) {
		if (e->wheel.y > 0) {
			if (ctx->inv->slots[ctx->tool].mat == 0)
				ctx->inv->slots[ctx->tool].mat = MAT_COUNT;
			--ctx->inv->slots[ctx->tool].mat;
		} else if (e->wheel.y < 0) {
			++ctx->inv->slots[ctx->tool].mat;
			if (ctx->inv->slots[ctx->tool].mat == MAT_COUNT)
				ctx->inv->slots[ctx->tool].mat = 0;
			if (ctx->tool == ctx->inv->size)
				ctx->tool = 0;
		}
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
			if (ctx->inv->slots[ctx->tool].num)
				printf("Holding %d: %s %s %d\n", ctx->tool,
						mat_names[ctx->inv->slots[ctx->tool].mat],
						obj_names[ctx->inv->slots[ctx->tool].obj],
						ctx->inv->slots[ctx->tool].num);
			else
				printf("Holding %d: nothing\n", ctx->tool);
	}
	return;
}

