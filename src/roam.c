

#include "roam.h"

#include <stdio.h>

#include "recipes.h"
#include "drop.h"

void render_cursor(struct context *ctx)
{
	if (ctx->cur.face == -1)
		return;

	GLfloat x = ctx->cur.p.x;
	GLfloat y = ctx->cur.p.y;
	GLfloat z = ctx->cur.p.z;

	if (ctx->cur.face == FACE_UP &&
		WORLD_AT(ctx->w, data, ctx->cur.p.x, ctx->cur.p.y, ctx->cur.p.z) != NULL)
		return;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(x, y, z);
	glColor3f(0, 0, 0);
	glLineWidth(2);

	glBegin(GL_LINE_LOOP);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 1);
	glVertex3f(0, 1, 1);
	glVertex3f(0, 1, 0);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(1, 0, 0);
	glVertex3f(1, 0, 1);
	glVertex3f(1, 1, 1);
	glVertex3f(1, 1, 0);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 1);
	glVertex3f(1, 0, 1);
	glVertex3f(1, 0, 0);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(0, 1, 0);
	glVertex3f(0, 1, 1);
	glVertex3f(1, 1, 1);
	glVertex3f(1, 1, 0);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(0, 0, 0);
	glVertex3f(1, 0, 0);
	glVertex3f(1, 1, 0);
	glVertex3f(0, 1, 0);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(0, 0, 1);
	glVertex3f(1, 0, 1);
	glVertex3f(1, 1, 1);
	glVertex3f(0, 1, 1);
	glEnd();
	glPopMatrix();
}


void render_obj(struct context *ctx, int obj, int mat, GLfloat alpha)
{
	//glEnable(GL_TEXTURE_2D);
	//glTexCoord2f(0.5, 0.5);
	glBegin(GL_TRIANGLES);

	if (obj == OBJ_BLOCK || obj == OBJ_WORKBENCH || obj == OBJ_CRATE ||
			obj == OBJ_FLUID) {
		glColor4ub(192, 192, 192, alpha);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, 1);
		glVertex3f(0, 1, 0);
		glVertex3f(0, 1, 0);
		glVertex3f(0, 0, 1);
		glVertex3f(0, 1, 1);
		glVertex3f(1, 1, 1);
		glVertex3f(1, 0, 1);
		glVertex3f(1, 1, 0);
		glVertex3f(1, 1, 0);
		glVertex3f(1, 0, 1);
		glVertex3f(1, 0, 0);
		glColor4ub(64, 64, 64, alpha);
		glVertex3f(1, 0, 1);
		glVertex3f(0, 0, 1);
		glVertex3f(1, 0, 0);
		glVertex3f(1, 0, 0);
		glVertex3f(0, 0, 1);
		glVertex3f(0, 0, 0);
		glColor4ub(255, 255, 255, alpha);
		glVertex3f(0, 1, 0);
		glVertex3f(0, 1, 1);
		glVertex3f(1, 1, 0);
		glVertex3f(1, 1, 0);
		glVertex3f(0, 1, 1);
		glVertex3f(1, 1, 1);
		glColor4ub(128, 128, 128, alpha);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 1, 0);
		glVertex3f(1, 0, 0);
		glVertex3f(1, 0, 0);
		glVertex3f(0, 1, 0);
		glVertex3f(1, 1, 0);
		glVertex3f(1, 1, 1);
		glVertex3f(0, 1, 1);
		glVertex3f(1, 0, 1);
		glVertex3f(1, 0, 1);
		glVertex3f(0, 1, 1);
		glVertex3f(0, 0, 1);
	} else if (obj == OBJ_SLAB) {
		glColor4ub(192, 192, 192, alpha);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, 1);
		glVertex3f(0, .5, 0);
		glVertex3f(0, .5, 0);
		glVertex3f(0, 0, 1);
		glVertex3f(0, .5, 1);
		glVertex3f(1, .5, 1);
		glVertex3f(1, 0, 1);
		glVertex3f(1, .5, 0);
		glVertex3f(1, .5, 0);
		glVertex3f(1, 0, 1);
		glVertex3f(1, 0, 0);
		glColor4ub(64, 64, 64, alpha);
		glVertex3f(1, 0, 1);
		glVertex3f(0, 0, 1);
		glVertex3f(1, 0, 0);
		glVertex3f(1, 0, 0);
		glVertex3f(0, 0, 1);
		glVertex3f(0, 0, 0);
		glColor4ub(255, 255, 255, alpha);
		glVertex3f(0, .5, 0);
		glVertex3f(0, .5, 1);
		glVertex3f(1, .5, 0);
		glVertex3f(1, .5, 0);
		glVertex3f(0, .5, 1);
		glVertex3f(1, .5, 1);
		glColor4ub(128, 128, 128, alpha);
		glVertex3f(0, 0, 0);
		glVertex3f(0, .5, 0);
		glVertex3f(1, 0, 0);
		glVertex3f(1, 0, 0);
		glVertex3f(0, .5, 0);
		glVertex3f(1, .5, 0);
		glVertex3f(1, .5, 1);
		glVertex3f(0, .5, 1);
		glVertex3f(1, 0, 1);
		glVertex3f(1, 0, 1);
		glVertex3f(0, .5, 1);
		glVertex3f(0, 0, 1);
	} else if (obj == OBJ_STAIRS) {
		glColor4ub(192, 192, 192, alpha);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, 1);
		glVertex3f(0, .5, 0);
		glVertex3f(0, .5, 0);
		glVertex3f(0, 0, 1);
		glVertex3f(0, .5, 1);
		glVertex3f(1, .5, 1);
		glVertex3f(1, 0, 1);
		glVertex3f(1, .5, 0);
		glVertex3f(1, .5, 0);
		glVertex3f(1, 0, 1);
		glVertex3f(1, 0, 0);
		glColor4ub(64, 64, 64, alpha);
		glVertex3f(1, 0, 1);
		glVertex3f(0, 0, 1);
		glVertex3f(1, 0, 0);
		glVertex3f(1, 0, 0);
		glVertex3f(0, 0, 1);
		glVertex3f(0, 0, 0);
		glColor4ub(255, 255, 255, alpha);
		glVertex3f(0, .5, 0);
		glVertex3f(0, .5, 1);
		glVertex3f(1, .5, 0);
		glVertex3f(1, .5, 0);
		glVertex3f(0, .5, 1);
		glVertex3f(1, .5, 1);
		glColor4ub(128, 128, 128, alpha);
		glVertex3f(0, 0, 0);
		glVertex3f(0, .5, 0);
		glVertex3f(1, 0, 0);
		glVertex3f(1, 0, 0);
		glVertex3f(0, .5, 0);
		glVertex3f(1, .5, 0);
		glVertex3f(1, .5, 1);
		glVertex3f(0, .5, 1);
		glVertex3f(1, 0, 1);
		glVertex3f(1, 0, 1);
		glVertex3f(0, .5, 1);
		glVertex3f(0, 0, 1);
		glColor4ub(192, 192, 192, alpha);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, 1);
		glVertex3f(0, 1, 0);
		glVertex3f(0, 1, 0);
		glVertex3f(0, 0, 1);
		glVertex3f(0, 1, 1);
		glVertex3f(.5, 1, 1);
		glVertex3f(.5, 0, 1);
		glVertex3f(.5, 1, 0);
		glVertex3f(.5, 1, 0);
		glVertex3f(.5, 0, 1);
		glVertex3f(.5, 0, 0);
		glColor4ub(64, 64, 64, alpha);
		glVertex3f(.5, 0, 1);
		glVertex3f(0, 0, 1);
		glVertex3f(.5, 0, 0);
		glVertex3f(.5, 0, 0);
		glVertex3f(0, 0, 1);
		glVertex3f(0, 0, 0);
		glColor4ub(255, 255, 255, alpha);
		glVertex3f(0, 1, 0);
		glVertex3f(0, 1, 1);
		glVertex3f(.5, 1, 0);
		glVertex3f(.5, 1, 0);
		glVertex3f(0, 1, 1);
		glVertex3f(.5, 1, 1);
		glColor4ub(128, 128, 128, alpha);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 1, 0);
		glVertex3f(.5, 0, 0);
		glVertex3f(.5, 0, 0);
		glVertex3f(0, 1, 0);
		glVertex3f(.5, 1, 0);
		glVertex3f(.5, 1, 1);
		glVertex3f(0, 1, 1);
		glVertex3f(.5, 0, 1);
		glVertex3f(.5, 0, 1);
		glVertex3f(0, 1, 1);
		glVertex3f(0, 0, 1);
	} else if (obj == OBJ_PANE) {
		glColor4ub(192, 192, 192, alpha);
		glVertex3f(.46875, 0, 0);
		glVertex3f(.46875, 0, 1);
		glVertex3f(.46875, 1, 0);
		glVertex3f(.46875, 1, 0);
		glVertex3f(.46875, 0, 1);
		glVertex3f(.46875, 1, 1);
		glVertex3f(.53125, 1, 1);
		glVertex3f(.53125, 0, 1);
		glVertex3f(.53125, 1, 0);
		glVertex3f(.53125, 1, 0);
		glVertex3f(.53125, 0, 1);
		glVertex3f(.53125, 0, 0);
		glColor4ub(64, 64, 64, alpha);
		glVertex3f(.53125, 0, 1);
		glVertex3f(.46875, 0, 1);
		glVertex3f(.53125, 0, 0);
		glVertex3f(.53125, 0, 0);
		glVertex3f(.46875, 0, 1);
		glVertex3f(.46875, 0, 0);
		glColor4ub(255, 255, 255, alpha);
		glVertex3f(.46875, 1, 0);
		glVertex3f(.46875, 1, 1);
		glVertex3f(.53125, 1, 0);
		glVertex3f(.53125, 1, 0);
		glVertex3f(.46875, 1, 1);
		glVertex3f(.53125, 1, 1);
		glColor4ub(128, 128, 128, alpha);
		glVertex3f(.46875, 0, 0);
		glVertex3f(.46875, 1, 0);
		glVertex3f(.53125, 0, 0);
		glVertex3f(.53125, 0, 0);
		glVertex3f(.46875, 1, 0);
		glVertex3f(.53125, 1, 0);
		glVertex3f(.53125, 1, 1);
		glVertex3f(.46875, 1, 1);
		glVertex3f(.53125, 0, 1);
		glVertex3f(.53125, 0, 1);
		glVertex3f(.46875, 1, 1);
		glVertex3f(.46875, 0, 1);
	}
	glEnd();
	//glDisable(GL_TEXTURE_2D);
}

void render_inventory(struct context *ctx, struct inventory *inv, struct v3ll p)
{
	int i, x, z;
	int side = sqrt(inv->size);

	GLfloat d = v3_dist(ctx->player->p, p);
	GLubyte alpha = d > 4 ? 0 : d < 2 ? 255 : 255 * (2 - d / 2);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	i = 0;
	for (x = 0; x < side; ++x) {
		for (z = 0; z < side; ++z) {
			glColor4ub(0, 0, 0, alpha);
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glTranslatef(p.x + (x + .125) / side, p.y + 1.0001, p.z + (z +.125) / side);
			glScalef(.75 / side, .75 / side, .75 / side);
			glBegin(GL_LINE_LOOP);
			glVertex3f(0, 0, 0);
			glVertex3f(0, 0, 1);
			glVertex3f(1, 0, 1);
			glVertex3f(1, 0, 0);
			glEnd();
			glPopMatrix();
			if (inv->slots[i].num > 0) {
				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();
				glTranslatef(p.x + (x + .25) / side, p.y + 1, p.z + (z +.25) / side);
				glScalef(.5 / side, .5 / side, .5 / side);
				render_obj(ctx, inv->slots[i].obj, inv->slots[i].mat, alpha);
				glPopMatrix();
			}
			++i;
		}
	}
	glDisable(GL_BLEND);
}

void roam_render(struct context *ctx)
{
	struct aab3ll bb;
	struct v3ll p;
	struct drop *d;
	int i;

	render_cursor(ctx);
	bb.x0 = floor(ctx->player->p.x - 4);
	bb.y0 = floor(ctx->player->p.y - 4);
	bb.z0 = floor(ctx->player->p.z - 4);
	bb.x1 = ceil(ctx->player->p.x + 4);
	bb.y1 = ceil(ctx->player->p.y + 4);
	bb.z1 = ceil(ctx->player->p.z + 4);

	aab3_foreach(p, bb) {
		struct inventory *inv = WORLD_AT(ctx->w, data, p.x, p.y, p.z);
		if (inv != NULL)
			render_inventory(ctx, inv, p);
	}

	list_foreach(d, &ctx->drops, list) {
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glTranslatef(d->body->p.x, d->body->p.y, d->body->p.z);
		glScalef(.25, .25, .25);
		render_obj(ctx, d->obj, d->mat, 255);
		glPopMatrix();
	}

	if (ctx->inv->slots[ctx->tool].num > 0) {
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glTranslatef(ctx->cam->p.x, ctx->cam->p.y, ctx->cam->p.z);
		glRotatef(180.0 * ctx->cam->r.y / M_PI, 0, -1, 0);
		glRotatef(180.0 * ctx->cam->r.x / M_PI, 1, 0, 0);
		glTranslatef(.4, -.4, -.8);
		glScalef(.125, .125, .125);
		render_obj(ctx, ctx->inv->slots[ctx->tool].obj, ctx->inv->slots[ctx->tool].mat, 255);
		glPopMatrix();
	}

	for (i = 0; i < 9; ++i) {
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glTranslatef(ctx->cam->p.x, ctx->cam->p.y, ctx->cam->p.z);
		glRotatef(180.0 * ctx->cam->r.y / M_PI, 0, -1, 0);
		glRotatef(180.0 * ctx->cam->r.x / M_PI, 1, 0, 0);
		glTranslatef(i * .06 - .30, -.4, -.8);
		glScalef(.03125, .03125, .03125);
		if (i == ctx->tool)
			glColor3ub(255, 255, 255);
		else
			glColor3ub(64, 64, 64);
		glBegin(GL_LINE_LOOP);
		glVertex3f(-.25, 0, -.25);
		glVertex3f(-.25, 0, 1.25);
		glVertex3f(1.25, 0, 1.25);
		glVertex3f(1.25, 0, -.25);
		glEnd();
		if (ctx->inv->slots[i].num > 0)
			render_obj(ctx, ctx->inv->slots[i].obj, ctx->inv->slots[i].mat, 255);
		glPopMatrix();
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
				if (ctx->move.y0) {
					if (num2 == 0) {
						printf("nothing to take\n");
						return;
					}
					int acc = inventory_add(ctx->inv, obj2, mat2, inv->slots[slot].num);
					inv->slots[slot].num -= acc;
					if (acc == 0)
						printf("no space to take\n");
					else
						printf("take %s %s %d\n", mat_names[mat], obj_names[obj], acc);
				} else {
					if (num2)
						printf("take %s %s %d\n", mat_names[mat2], obj_names[obj2], num2);
					if (num)
						printf("left %s %s %d\n", mat_names[mat], obj_names[obj], num);
					inv->slots[slot].obj = obj;
					inv->slots[slot].mat = mat;
					inv->slots[slot].num = num;
					ctx->inv->slots[ctx->tool].obj = obj2;
					ctx->inv->slots[ctx->tool].mat = mat2;
					ctx->inv->slots[ctx->tool].num = num2;
				}
			} else if (ctx->use == 1) {
				if (ctx->move.y0) {
					if (num2 == 0) {
						printf("nothing to take\n");
						return;
					}
					int acc = inventory_add(ctx->inv, obj2, mat2, 1);
					inv->slots[slot].num -= acc;
					if (acc == 0)
						printf("no space to take\n");
					else
						printf("take %s %s 1\n", mat_names[mat], obj_names[obj], acc);
				} else {
					if (num == 0) {
						printf("nothing to leave\n");
						return;
					}
					if (num2 >= 64) {
						printf("no space to leave\n");
						return;
					}
					if (num2 == 0) {
						inv->slots[slot].obj = obj;
						inv->slots[slot].mat = mat;
					} else if (obj != obj2 || mat != mat2) {
						printf("not the same object\n");
						return;
					}
					++inv->slots[slot].num;
					--ctx->inv->slots[ctx->tool].num;
					printf("left %s %s 1\n", mat_names[mat], obj_names[obj]);
				}
			}
			if (WORLD_AT(ctx->w, shape, p.x, p.y, p.z) == SHAPE_WORKBENCH) {
				const struct recipe *r;
				int i;
				for (r = recipes; r->num != 0; ++r) {
					if (recipe_match(r, inv)) {
						do {
							for (i = 0; i < 9; ++i) {
								if (inv->slots[i].num)
									--inv->slots[i].num;
							}
							inventory_add(ctx->inv, r->obj, 255, r->num);
							printf("take %s %s %d\n", mat_names[255], obj_names[r->obj], r->num);
						} while (recipe_match(r, inv));
						break;
					}
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
				} else if (obj == OBJ_PANE) {
					if (ctx->roty & 1)
						world_set(ctx->w, p, SHAPE_PANE_X, 255, NULL);
					else
						world_set(ctx->w, p, SHAPE_PANE_Z, 255, NULL);
				} else if (obj == OBJ_WORKBENCH) {
					world_set(ctx->w, p, SHAPE_WORKBENCH, 255, inventory(9));
				} else if (obj == OBJ_CRATE) {
					world_set(ctx->w, p, SHAPE_CRATE, 255, inventory(16));
				} else if (obj == OBJ_FLUID) {
					world_set(ctx->w, p, SHAPE_FLUID15, 255, NULL);
				}
			}
		}
	}
	if (ctx->pick == 1) {
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
	camera_set_position(ctx->cam, v3_add(ctx->player->p, v3f(0, .8, 0)));
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
		} else if (e->key.keysym.sym == SDLK_1) {
			ctx->tool = 0;
			return 0;
		} else if (e->key.keysym.sym == SDLK_2) {
			ctx->tool = 1;
			return 0;
		} else if (e->key.keysym.sym == SDLK_3) {
			ctx->tool = 2;
			return 0;
		} else if (e->key.keysym.sym == SDLK_4) {
			ctx->tool = 3;
			return 0;
		} else if (e->key.keysym.sym == SDLK_5) {
			ctx->tool = 4;
			return 0;
		} else if (e->key.keysym.sym == SDLK_6) {
			ctx->tool = 5;
			return 0;
		} else if (e->key.keysym.sym == SDLK_7) {
			ctx->tool = 6;
			return 0;
		} else if (e->key.keysym.sym == SDLK_8) {
			ctx->tool = 7;
			return 0;
		} else if (e->key.keysym.sym == SDLK_9) {
			ctx->tool = 8;
			return 0;
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
			return 0;
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
