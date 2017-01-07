

#include "roam.h"

#include <stdio.h>

void render_cursor(struct context *ctx)
{
	if (ctx->cur.face == -1)
		return;
	GLfloat x = ctx->cur.p.x;
	GLfloat y = ctx->cur.p.y;
	GLfloat z = ctx->cur.p.z;

	glColor3f(0, 0, 0);
	glLineWidth(2);

	glBegin(GL_LINE_LOOP);
	glVertex3f(x, y, z);
	glVertex3f(x, y, z + 1);
	glVertex3f(x, y + 1, z + 1);
	glVertex3f(x, y + 1, z);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(x + 1, y, z);
	glVertex3f(x + 1, y, z + 1);
	glVertex3f(x + 1, y + 1, z + 1);
	glVertex3f(x + 1, y + 1, z);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(x, y, z);
	glVertex3f(x, y, z + 1);
	glVertex3f(x + 1, y, z + 1);
	glVertex3f(x + 1, y, z);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(x, y + 1, z);
	glVertex3f(x, y + 1, z + 1);
	glVertex3f(x + 1, y + 1, z + 1);
	glVertex3f(x + 1, y + 1, z);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(x, y, z);
	glVertex3f(x + 1, y, z);
	glVertex3f(x + 1, y + 1, z);
	glVertex3f(x, y + 1, z);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(x, y, z + 1);
	glVertex3f(x + 1, y, z + 1);
	glVertex3f(x + 1, y + 1, z + 1);
	glVertex3f(x, y + 1, z + 1);
	glEnd();
}


void render_obj(struct v3f p, GLfloat r)
{
	glTexCoord2f(1, 1);
	glColor3b(192, 192, 192);
	glBegin(GL_TRIANGLES);

	glVertex3f(p.x, p.y, p.z);
	glVertex3f(p.x, p.y, p.z + r);
	glVertex3f(p.x, p.y + r, p.z);
	glVertex3f(p.x, p.y + r, p.z);
	glVertex3f(p.x, p.y, p.z + r);
	glVertex3f(p.x, p.y + r, p.z + r);

	glVertex3f(p.x + r, p.y + r, p.z + r);
	glVertex3f(p.x + r, p.y, p.z + r);
	glVertex3f(p.x + r, p.y + r, p.z);
	glVertex3f(p.x + r, p.y + r, p.z);
	glVertex3f(p.x + r, p.y, p.z + r);
	glVertex3f(p.x + r, p.y, p.z);

	glColor3b(64, 64, 64);
	glVertex3f(p.x + r, p.y, p.z + r);
	glVertex3f(p.x, p.y, p.z + r);
	glVertex3f(p.x + r, p.y, p.z);
	glVertex3f(p.x + r, p.y, p.z);
	glVertex3f(p.x, p.y, p.z + r);
	glVertex3f(p.x, p.y, p.z);

	glColor3b(255, 255, 255);
	glVertex3f(p.x, p.y + r, p.z);
	glVertex3f(p.x, p.y + r, p.z + r);
	glVertex3f(p.x + r, p.y + r, p.z);
	glVertex3f(p.x + r, p.y + r, p.z);
	glVertex3f(p.x, p.y + r, p.z + r);
	glVertex3f(p.x + r, p.y + r, p.z + r);

	glColor3b(128, 128, 128);
	glVertex3f(p.x, p.y, p.z);
	glVertex3f(p.x, p.y + r, p.z);
	glVertex3f(p.x + r, p.y, p.z);
	glVertex3f(p.x + r, p.y, p.z);
	glVertex3f(p.x, p.y + r, p.z);
	glVertex3f(p.x + r, p.y + r, p.z);

	glVertex3f(p.x + r, p.y + r, p.z + r);
	glVertex3f(p.x, p.y + r, p.z + r);
	glVertex3f(p.x + r, p.y, p.z + r);
	glVertex3f(p.x + r, p.y, p.z + r);
	glVertex3f(p.x, p.y + r, p.z + r);
	glVertex3f(p.x, p.y, p.z + r);

	glEnd();
}

void render_inventory(struct context *ctx, struct inventory *inv, struct v3ll p)
{
	int i, x, z;
	int side = sqrt(inv->size);
	struct v3f q;

	i = 0;
	for (x = 0; x < side; ++x) {
		for (z = 0; z < side; ++z) {
			q.x = p.x + (x + .125) / side;
			q.y = p.y + 1;
			q.z = p.z + (z + .125) / side;
			glBegin(GL_LINE_LOOP);
			glVertex3f(q.x, q.y + .0001, q.z);
			glVertex3f(q.x, q.y + .0001, q.z + .75 / side);
			glVertex3f(q.x + .75 / side, q.y + .0001, q.z + .75 / side);
			glVertex3f(q.x + .75 / side, q.y + .0001, q.z);
			glEnd();
			if (inv->slots[i].num > 0) {
				render_obj(v3f(p.x + (x + .25) / side, p.y + 1, p.z + (z + .25) / side), .5 / side);
			}
			++i;
		}
	}
}

void roam_render(struct context *ctx)
{
	struct aab3ll bb;
	struct v3ll p;

	render_cursor(ctx);
	bb.x0 = ctx->player->p.x - 4;
	bb.y0 = ctx->player->p.y - 4;
	bb.z0 = ctx->player->p.z - 4;
	bb.x1 = ctx->player->p.x + 4;
	bb.y1 = ctx->player->p.y + 4;
	bb.z1 = ctx->player->p.z + 4;

	aab3_foreach(p, bb) {
		struct inventory *inv = WORLD_AT(ctx->w, data, p.x, p.y, p.z);
		if (inv != NULL)
			render_inventory(ctx, inv, p);
	}
}

void update_player(struct context *ctx)
{
	struct v3ll p = ctx->cur.p;
	struct v3f q = ctx->cur.q;
	int f = ctx->cur.face;
	int obj = ctx->inv->slots[ctx->tool].obj;
	int mat = ctx->inv->slots[ctx->tool].mat;
	int num = ctx->inv->slots[ctx->tool].num;

	if (f == FACE_UP) {
		struct inventory *inv = WORLD_AT(ctx->w, data, p.x, p.y, p.z);
		if (inv != NULL) {
			int side = sqrt(inv->size);
			int slot = side * floor(q.x * side) + floor(q.z * side);
			int obj2 = inv->slots[slot].obj;
			int mat2 = inv->slots[slot].mat;
			int num2 = inv->slots[slot].num;
			if (ctx->act == 1) {
				if (num > 0) {
					if (num2 == 0) {
						inv->slots[slot].obj = obj;
						inv->slots[slot].mat = mat;
						inv->slots[slot].num = 1;
						--ctx->inv->slots[ctx->tool].num;
					} else if (obj == obj2 && mat == mat2) {
						if (inventory_add_to_slot(inv, slot, obj, mat, 1))
							--ctx->inv->slots[ctx->tool].num;
					}
				} else if (num2 > 0) {
					ctx->inv->slots[ctx->tool].obj = obj2;
					ctx->inv->slots[ctx->tool].mat = mat2;
					ctx->inv->slots[ctx->tool].num = 1;
					--inv->slots[slot].num;
				}
			}
			return;
		}
	}
	if (ctx->act == 1) {
		if (ctx->cur.face != -1) {
			if (WORLD_AT(ctx->w, shape, p.x, p.y, p.z) != 0)
				world_set(ctx->w, p, 0, 0, NULL);
		}
	}
	if (ctx->use == 1) {
		if (f != -1) {
			if (f == FACE_LF)
				p = v3_add(p, v3c(-1, 0, 0));
			else if (f == FACE_RT)
				p = v3_add(p, v3c(1, 0, 0));
			else if (f == FACE_DN)
				p = v3_add(p, v3c(0, -1, 0));
			else if (f == FACE_UP)
				p = v3_add(p, v3c(0, 1, 0));
			else if (f == FACE_BK)
				p = v3_add(p, v3c(0, 0, -1));
			else if (f == FACE_FT)
				p = v3_add(p, v3c(0, 0, 1));
			if (WORLD_AT(ctx->w, shape, p.x, p.y, p.z) == 0) {
				if (obj == OBJ_BLOCK) {
					world_set(ctx->w, p, SHAPE_BLOCK_DN, 255, NULL);
				} else if (obj == OBJ_SLAB) {
					if (f == FACE_UP) {
						world_set(ctx->w, p, SHAPE_SLAB_DN, 255, NULL);
					} else if (f == FACE_DN) {
						world_set(ctx->w, p, SHAPE_SLAB_UP, 255, NULL);
					} else if (q.y > 0.5) {
						world_set(ctx->w, p, SHAPE_SLAB_UP, 255, NULL);
					} else {
						world_set(ctx->w, p, SHAPE_SLAB_DN, 255, NULL);
					}
				} else if (obj == OBJ_STAIRS) {
					if (f == FACE_UP) {
						world_set(ctx->w, p, SHAPE_STAIRS_DF + (ctx->roty + 2) % 4, 255, NULL);
					} else if (f == FACE_DN) {
						world_set(ctx->w, p, SHAPE_STAIRS_UF + (ctx->roty + 2) % 4, 255, NULL);
					} else if (q.y > 0.5) {
						world_set(ctx->w, p, SHAPE_STAIRS_UF + (ctx->roty + 2) % 4, 255, NULL);
					} else {
						world_set(ctx->w, p, SHAPE_STAIRS_DF + (ctx->roty + 2) % 4, 255, NULL);
					}
				} else if (obj == OBJ_WORKBENCH) {
					world_set(ctx->w, p, SHAPE_WORKBENCH, 255, inventory(4));
				} else if (obj == OBJ_CRATE) {
					world_set(ctx->w, p, SHAPE_CRATE, 255, inventory(16));
				}
			}
		}
	}
	if (ctx->pick == 1) {
	}
	if (ctx->act > 0) {
		if (++ctx->act >= 8)
			ctx->act = 1;
	}
	if (ctx->use > 0) {
		if (++ctx->use >= 8)
			ctx->use = 1;
	}
	if (ctx->pick > 0) {
		if (++ctx->pick >= 8)
			ctx->pick = 1;
	}
}

void update_camera(struct context *ctx)
{
	int w, h;
	int x, y;
	int buttons;
	struct v3f r, v;

	SDL_GetWindowSize(ctx->ml->windows->sdl_window, &w, &h);
	buttons = SDL_GetMouseState(&x, &y);
	SDL_WarpMouseInWindow(ctx->ml->windows->sdl_window, w / 2, h / 2);

	r = body_get_rotation(ctx->player);
	r.y += (x - w / 2) * .005;
	r.x -= (y - h / 2) * .005;
	if (r.x < -M_PI_2 * .99)
		r.x = -M_PI_2 * .99;
	else if (r.x > M_PI_2 * .99)
		r.x = M_PI_2 *.99;
	body_set_rotation(ctx->player, r);

	v = body_get_velocity(ctx->player);
	if (ctx->move.x0 == 0 && ctx->move.x1 == 0 && ctx->move.z0 == 0 && ctx->move.z1 == 0)
		ctx->run = 0;
	v.x = (ctx->move.x1 - ctx->move.x0) * (ctx->run ? .18 : 0.15);
	v.y += (ctx->move.y1 - ctx->move.y0) * (ctx->run ? .18 : 0.15);
	v.z = (ctx->move.z1 - ctx->move.z0) * (ctx->run ? .18 : 0.15);
	v = v3_roty(v, r.y);
	body_set_velocity(ctx->player, v);

	camera_set_fovy(ctx->cam, (ctx->cam->fovy + (ctx->run ? 70.f : 60.f)) * 0.5);
	camera_set_position(ctx->cam, v3_add(ctx->player->p, v3f(0, .6, 0)));
	camera_set_rotation(ctx->cam, ctx->player->r);

	ctx->rotx = (unsigned int)floor(0.5 + r.x / M_PI_2) & 3;
	ctx->roty = (unsigned int)floor(0.5 + r.y / M_PI_2) & 3;

	v = v3f(0, 0, -5);
	v = v3_rotx(v, r.x);
	v = v3_roty(v, r.y);
	space_query(ctx->space, ctx->cam->p, v, &ctx->cur);
}

void roam_update(struct context *ctx)
{
	update_player(ctx);
	update_camera(ctx);
}

int roam_event(const SDL_Event *e, struct context *ctx)
{
	if (e->type == SDL_KEYDOWN) {
		if (e->key.repeat) {
			return 0;
		} else if (e->key.keysym.sym == SDLK_w) {
			ctx->move.z0 = 1;
			return 1;
		} else if (e->key.keysym.sym == SDLK_a) {
			ctx->move.x0 = 1;
			return 1;
		} else if (e->key.keysym.sym == SDLK_s) {
			ctx->move.z1 = 1;
			return 1;
		} else if (e->key.keysym.sym == SDLK_d) {
			ctx->move.x1 = 1;
			return 1;
		} else if (e->key.keysym.sym == SDLK_LSHIFT) {
			ctx->move.y0 = 1;
			return 1;
		} else if (e->key.keysym.sym == SDLK_SPACE) {
			ctx->move.y1 = 1;
			return 1;
		} else if (e->key.keysym.sym == SDLK_LCTRL) {
			ctx->run = 1;
			return 1;
		}
	} else if (e->type == SDL_KEYUP) {
		if (e->key.repeat) {
			return 0;
		} else if (e->key.keysym.sym == SDLK_w) {
			ctx->move.z0 = 0;
			return 1;
		} else if (e->key.keysym.sym == SDLK_a) {
			ctx->move.x0 = 0;
			return 1;
		} else if (e->key.keysym.sym == SDLK_s) {
			ctx->move.z1 = 0;
			return 1;
		} else if (e->key.keysym.sym == SDLK_d) {
			ctx->move.x1 = 0;
			return 1;
		} else if (e->key.keysym.sym == SDLK_LSHIFT) {
			ctx->move.y0 = 0;
			return 1;
		} else if (e->key.keysym.sym == SDLK_SPACE) {
			ctx->move.y1 = 0;
			return 1;
		}
	} else if (e->type == SDL_MOUSEBUTTONDOWN) {
		if (e->button.button == SDL_BUTTON_LEFT) {
			ctx->act = 1;
			return 1;
		} else if (e->button.button == SDL_BUTTON_RIGHT) {
			ctx->use = 1;
			return 1;
		} else if (e->button.button == SDL_BUTTON_MIDDLE) {
			ctx->pick = 1;
			return 1;
		}
	} else if (e->type == SDL_MOUSEBUTTONUP) {
		if (e->button.button == SDL_BUTTON_LEFT) {
			ctx->act = 0;
			return 1;
		} else if (e->button.button == SDL_BUTTON_RIGHT) {
			ctx->use = 0;
			return 1;
		} else if (e->button.button == SDL_BUTTON_MIDDLE) {
			ctx->pick = 0;
			return 1;
		}
	} else if (e->type == SDL_MOUSEWHEEL) {
		if (e->wheel.y > 0) {
			if (ctx->tool == 0)
				ctx->tool = ctx->inv->size;
			--ctx->tool;
			if (ctx->inv->slots[ctx->tool].num)
				printf("Holding %d: %s %s %d\n", ctx->tool,
					mat_names[ctx->inv->slots[ctx->tool].mat],
					obj_names[ctx->inv->slots[ctx->tool].obj],
					ctx->inv->slots[ctx->tool].num);
			else
				printf("Holding %d: nothing\n", ctx->tool);
			return 1;
		} else if (e->wheel.y < 0) {
			++ctx->tool;
			if (ctx->tool == ctx->inv->size)
				ctx->tool = 0;
			if (ctx->inv->slots[ctx->tool].num)
				printf("Holding %d: %s %s %d\n", ctx->tool,
					mat_names[ctx->inv->slots[ctx->tool].mat],
					obj_names[ctx->inv->slots[ctx->tool].obj],
					ctx->inv->slots[ctx->tool].num);
			else
				printf("Holding %d: nothing\n", ctx->tool);
			return 1;
		}
	}
	return 0;
}
