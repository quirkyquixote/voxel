/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "player_entity.h"

#include "context.h"
#include "block_entity.h"
#include "drop_entity.h"
#include "recipes.h"

PlayerEntity::PlayerEntity(Context *ctx)
	: RoamingEntity(ctx, 9), act(0), use(0), pick(0), run(0), tool(0)
{
	body->set_p(v3f(p.x, WORLD_H, p.y));
	body->set_size(v2f(0.325, 0.825));
	body->set_step_size(1);
}

PlayerEntity::~PlayerEntity()
{
}

void PlayerEntity::update()
{
	int w, h;
	int x, y;
	int buttons;
	v3f r, v;

	SDL_GetWindowSize(ctx->ml->get_window()->get_sdl_window(), &w, &h);
	buttons = SDL_GetMouseState(&x, &y);
	SDL_WarpMouseInWindow(ctx->ml->get_window()->get_sdl_window(), w / 2, h / 2);

	r = body->get_r();
	r.y += (x - w / 2) * .005;
	r.x -= (y - h / 2) * .005;
	if (r.x < -M_PI_2 * .99)
		r.x = -M_PI_2 * .99;
	else if (r.x > M_PI_2 * .99)
		r.x = M_PI_2 * .99;
	body->set_r(r);

	v = body->get_v();
	if (move.x0 == 0 && move.x1 == 0 && move.z0 == 0 && move.z1 == 0)
		run = 0;
	v.x = (move.x1 - move.x0) * (run ? .18 : 0.15);
	v.y += (move.y1 - move.y0) * (run ? .18 : 0.15);
	v.z = (move.z1 - move.z0) * (run ? .18 : 0.15);
	v = roty(v, r.y);
	body->set_v(v);
	body->set_step_size(1 + run);

	rot.x = (unsigned int)floor(0.5 + r.x / M_PI_2) & 3;
	rot.y = (unsigned int)floor(0.5 + r.y / M_PI_2) & 3;

	v = v3f(0, 0, -5);
	v = rotx(v, r.x);
	v = roty(v, r.y);
	ctx->space->query(ctx->renderer->get_cam()->get_p(), v, &cur);

	v3ll p = cur.p;
	int s = ctx->world->get_shape(p);
	int m = ctx->world->get_mat(p);
	Entity *e = ctx->world->get_data(p);

	if (e == nullptr || !e->use(this)) {
		if (act == 1) {
			if (cur.face != -1) {
				if (s != SHAPE_NONE) {
					ctx->spill_inventory(p);
					ctx->drop_block(p);
					ctx->world->set_block(p, 0, 0);
				}
			}
		}
		if (use == 1) {
			if (cur.face != -1) {
				use_tool();
			}
		}
		if (pick == 1) {
			if (cur.face != -1) {
				int s = ctx->world->get_shape(cur.p);
				int m = ctx->world->get_mat(cur.p);
				int o;
				if (s >= SHAPE_BLOCK_DN && s <= SHAPE_BLOCK_FT)
					o = OBJ_BLOCK;
				else if (s >= SHAPE_SLAB_DN && s <= SHAPE_SLAB_FT)
					o = OBJ_SLAB;
				else if (s >= SHAPE_PANE_X && s <= SHAPE_PANE_Z)
					o = OBJ_PANE;
				else if (s >= SHAPE_STAIRS_DF && s <= SHAPE_STAIRS_UR)
					o = OBJ_STAIRS;
				items[tool] = Item(o, m, 1);
			}
		}
	}
	if (act > 0) {
		if (++act >= 8)
			act = 1;
	}
	if (use > 0) {
		if (++use >= 8)
			use = 1;
	}
	if (pick > 0) {
		if (++pick >= 8)
			pick = 1;
	}
}

void PlayerEntity::use_inventory(std::vector<Item> *inv)
{
	v3ll p = cur.p;
	v3f q = cur.q;
	int side = sqrt(inv->size());
	int i = side * floor(q.x * side) + floor(q.z * side);
	Item &s1 = items[tool];
	Item &s2 = (*inv)[i];
	if (act == 1) {
		if (move.y0) {
			if (s2.num == 0) {
				log_info("nothing to take");
				return;
			}
			int acc = inventory_add(&items, s2);
			s2.num -= acc;
			if (acc == 0)
				log_info("no space to take");
			else
				log_info("take %s %s %d", mat_names[s1.mat],
						obj_names[s1.obj], acc);
		} else {
			if (s2.num)
				log_info("take %s %s %d", mat_names[s2.mat],
						obj_names[s2.obj], s2.num);
			if (s1.num)
				log_info("left %s %s %d", mat_names[s1.mat],
						obj_names[s1.obj], s1.num);
			std::swap(s1, s2);
		}
	} else if (use == 1) {
		if (move.y0) {
			if (s2.num == 0) {
				log_info("nothing to take");
				return;
			}
			int acc = inventory_add(&items, Item(s2.obj, s2.mat, 1));
			s2.num -= acc;
			if (acc == 0)
				log_info("no space to take");
			else
				log_info("take %s %s 1", mat_names[s1.mat],
						obj_names[s1.obj]);
		} else {
			if (s1.num == 0) {
				log_info("nothing to leave");
				return;
			}
			if (s2.num >= 64) {
				log_info("no space to leave");
				return;
			}
			if (s2.num == 0) {
				s2 = Item(s1.obj, s1.mat, 0);
			} else if (s1.obj != s2.obj || s1.mat != s2.mat) {
				log_info("not the same object");
				return;
			}
			++s2.num;
			--s1.num;
			log_info("left %s %s 1", mat_names[s1.mat],
					obj_names[s1.obj]);
		}
	}
}

void PlayerEntity::use_workbench(std::vector<Item> *inv)
{
	if (act == 1) {
		Item s;
		if (recipe_match(inv, &s)) {
			int i = 0;
			do {
				inventory_add(&items, s);
				++i;
			} while (recipe_match(inv, &s));
			log_info("take %s %s %d", mat_names[s.mat],
					obj_names[s.obj], s.num * i);
		} else {
			log_info("not a recipe");
		}
	} else if (use == 1) {
		Item s;
		if (recipe_match(inv, &s)) {
			inventory_add(&items, s);
			log_info("take %s %s %d", mat_names[s.mat],
					obj_names[s.obj], s.num);
		} else {
			log_info("not a recipe");
		}
	}
}

void PlayerEntity::use_tool()
{
	v3ll p = cur.p;
	v3f q = cur.q;
	int f = cur.face;
	Item &s = items[tool];

	if (s.num == 0)
		return;
	if (f == FACE_LF)
		p = p + v3ll(-1, 0, 0);
	else if (f == FACE_RT)
		p = p + v3ll(1, 0, 0);
	else if (f == FACE_DN)
		p = p + v3ll(0, -1, 0);
	else if (f == FACE_UP)
		p = p + v3ll(0, 1, 0);
	else if (f == FACE_BK)
		p = p + v3ll(0, 0, -1);
	else if (f == FACE_FT)
		p = p + v3ll(0, 0, 1);
	if (ctx->world->get_shape(p) != SHAPE_NONE)
		return;
	if (s.obj == OBJ_BLOCK) {
		if (move.y0) {
			if (f == FACE_UP)
				ctx->world->set_block(p, SHAPE_BLOCK_DN, s.mat);
			else if (f == FACE_DN)
				ctx->world->set_block(p, SHAPE_BLOCK_UP, s.mat);
			else if (f == FACE_LF)
				ctx->world->set_block(p, SHAPE_BLOCK_RT, s.mat);
			else if (f == FACE_RT)
				ctx->world->set_block(p, SHAPE_BLOCK_LF, s.mat);
			else if (f == FACE_BK)
				ctx->world->set_block(p, SHAPE_BLOCK_FT, s.mat);
			else if (f == FACE_FT)
				ctx->world->set_block(p, SHAPE_BLOCK_BK, s.mat);
		} else {
			ctx->world->set_block(p, SHAPE_BLOCK_DN, s.mat);
		}
	} else if (s.obj == OBJ_SLAB) {
		if (move.y0) {
			if (f == FACE_UP)
				ctx->world->set_block(p, SHAPE_SLAB_DN, s.mat);
			else if (f == FACE_DN)
				ctx->world->set_block(p, SHAPE_SLAB_UP, s.mat);
			else if (f == FACE_LF)
				ctx->world->set_block(p, SHAPE_SLAB_RT, s.mat);
			else if (f == FACE_RT)
				ctx->world->set_block(p, SHAPE_SLAB_LF, s.mat);
			else if (f == FACE_BK)
				ctx->world->set_block(p, SHAPE_SLAB_FT, s.mat);
			else if (f == FACE_FT)
				ctx->world->set_block(p, SHAPE_SLAB_BK, s.mat);
		} else {
			if (f == FACE_UP)
				ctx->world->set_block(p, SHAPE_SLAB_DN, s.mat);
			else if (f == FACE_DN)
				ctx->world->set_block(p, SHAPE_SLAB_UP, s.mat);
			else if (q.y > 0.5)
				ctx->world->set_block(p, SHAPE_SLAB_UP, s.mat);
			else
				ctx->world->set_block(p, SHAPE_SLAB_DN, s.mat);
		}
	} else if (s.obj == OBJ_STAIRS) {
		if (f == FACE_UP) {
			ctx->world->set_block(p,
					SHAPE_STAIRS_DF + (rot.y + 2) % 4, s.mat);
		} else if (f == FACE_DN) {
			ctx->world->set_block(p,
					SHAPE_STAIRS_UF + (rot.y + 2) % 4, s.mat);
		} else if (q.y > 0.5) {
			ctx->world->set_block(p,
					SHAPE_STAIRS_UF + (rot.y + 2) % 4, s.mat);
		} else {
			ctx->world->set_block(p,
					SHAPE_STAIRS_DF + (rot.y + 2) % 4, s.mat);
		}
	} else if (s.obj == OBJ_PANE) {
		if (move.y0) {
			if (f == FACE_UP)
				ctx->world->set_block(p, SHAPE_PANE_DN, s.mat);
			else if (f == FACE_DN)
				ctx->world->set_block(p, SHAPE_PANE_UP, s.mat);
			else if (f == FACE_LF)
				ctx->world->set_block(p, SHAPE_PANE_RT, s.mat);
			else if (f == FACE_RT)
				ctx->world->set_block(p, SHAPE_PANE_LF, s.mat);
			else if (f == FACE_BK)
				ctx->world->set_block(p, SHAPE_PANE_FT, s.mat);
			else if (f == FACE_FT)
				ctx->world->set_block(p, SHAPE_PANE_BK, s.mat);
		} else {
			if (rot.y & 1)
				ctx->world->set_block(p, SHAPE_PANE_X, s.mat);
			else
				ctx->world->set_block(p, SHAPE_PANE_Z, s.mat);
		}
/*	} else if (s.obj == OBJ_FLUID) {
		flowsim_add(flowsim, p, 1);*/
	}
	--s.num;
}

void PlayerEntity::render()
{
	render_cursor();
	glDisable(GL_DEPTH_TEST);
	render_held_item();
	render_hotbar();
	glEnable(GL_DEPTH_TEST);

}

void PlayerEntity::render_cursor()
{
	if (cur.face == -1)
		return;

	GLfloat x = cur.p.x;
	GLfloat y = cur.p.y;
	GLfloat z = cur.p.z;

	if (cur.face == FACE_UP && ctx->world->get_data(cur.p) != nullptr)
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

void PlayerEntity::render_held_item()
{
	Item s = items[tool];
	if (s.num > 0) {
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		v3f p = ctx->renderer->get_cam()->get_p();
		v3f r = ctx->renderer->get_cam()->get_r();
		glTranslatef(p.x, p.y, p.z);
		glRotatef(180.0 * r.y / M_PI, 0, -1, 0);
		glRotatef(180.0 * r.x / M_PI, 1, 0, 0);
		glTranslatef(.4, -.4, -.8);
		glScalef(.125, .125, .125);
		ctx->renderer->render_item(s.obj, s.mat, 255);
		glPopMatrix();
	}
}

void PlayerEntity::render_hotbar()
{
	for (int i = 0; i < 9; ++i) {
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		v3f p = ctx->renderer->get_cam()->get_p();
		v3f r = ctx->renderer->get_cam()->get_r();
		glTranslatef(p.x, p.y, p.z);
		glRotatef(180.0 * r.y / M_PI, 0, -1, 0);
		glRotatef(180.0 * r.x / M_PI, 1, 0, 0);
		glTranslatef(i * .06 - .30, -.4, -.8);
		glScalef(.03125, .03125, .03125);
		if (i == tool)
			glColor3ub(255, 255, 255);
		else
			glColor3ub(64, 64, 64);
		glBegin(GL_LINE_LOOP);
		glVertex3f(-.25, 0, -.25);
		glVertex3f(-.25, 0, 1.25);
		glVertex3f(1.25, 0, 1.25);
		glVertex3f(1.25, 0, -.25);
		glEnd();
		Item s = items[i];
		if (s.num > 0)
			ctx->renderer->render_item(s.obj, s.mat, 255);
		if (s.num > 1) {
			glEnable(GL_BLEND);
			glColor3f(1, 1, 1);
			glTranslatef(0, -1.5, 0);
			char buf[3];
			snprintf(buf, sizeof(buf), "%02d", s.num);
			ctx->renderer->render_string(buf);
			glDisable(GL_BLEND);
		}
		glPopMatrix();
	}
}

sz_Tag *PlayerEntity::save()
{
	return RoamingEntity::save();
}

void PlayerEntity::load(sz_Tag *root)
{
	RoamingEntity::load(root);
}

void PlayerEntity::handle_event(const SDL_Event &e)
{
	if (e.type == SDL_KEYDOWN) {
		if (e.key.repeat) {
			return;
		} else if (e.key.keysym.sym == SDLK_w) {
			move.z0 = 1;
		} else if (e.key.keysym.sym == SDLK_a) {
			move.x0 = 1;
		} else if (e.key.keysym.sym == SDLK_s) {
			move.z1 = 1;
		} else if (e.key.keysym.sym == SDLK_d) {
			move.x1 = 1;
		} else if (e.key.keysym.sym == SDLK_LSHIFT) {
			move.y0 = 1;
		} else if (e.key.keysym.sym == SDLK_SPACE) {
			move.y1 = 1;
		} else if (e.key.keysym.sym == SDLK_r) {
			pick = 1;
		} else if (e.key.keysym.sym == SDLK_LCTRL) {
			run = 1;
		} else if (e.key.keysym.sym == SDLK_1) {
			tool = 0;
		} else if (e.key.keysym.sym == SDLK_2) {
			tool = 1;
		} else if (e.key.keysym.sym == SDLK_3) {
			tool = 2;
		} else if (e.key.keysym.sym == SDLK_4) {
			tool = 3;
		} else if (e.key.keysym.sym == SDLK_5) {
			tool = 4;
		} else if (e.key.keysym.sym == SDLK_6) {
			tool = 5;
		} else if (e.key.keysym.sym == SDLK_7) {
			tool = 6;
		} else if (e.key.keysym.sym == SDLK_8) {
			tool = 7;
		} else if (e.key.keysym.sym == SDLK_9) {
			tool = 8;
		} else if (e.key.keysym.sym == SDLK_q) {
			Item &s = items[tool];
			if (s.num > 0) {
				DropEntity *d = new DropEntity(ctx, Item(s.obj, s.mat, 1));
				d->get_body()->set_p(ctx->renderer->get_cam()->get_p());
				v3f v(0, 0, -.5);
				v = rotx(v, ctx->renderer->get_cam()->get_r().x);
				v = roty(v, ctx->renderer->get_cam()->get_r().y);
				d->get_body()->set_v(v);
				ctx->entities.push_back(d);
				--s.num;
			}
		}
	} else if (e.type == SDL_KEYUP) {
		if (e.key.repeat) {
			return;
		} else if (e.key.keysym.sym == SDLK_w) {
			move.z0 = 0;
		} else if (e.key.keysym.sym == SDLK_a) {
			move.x0 = 0;
		} else if (e.key.keysym.sym == SDLK_s) {
			move.z1 = 0;
		} else if (e.key.keysym.sym == SDLK_d) {
			move.x1 = 0;
		} else if (e.key.keysym.sym == SDLK_LSHIFT) {
			move.y0 = 0;
		} else if (e.key.keysym.sym == SDLK_SPACE) {
			move.y1 = 0;
		} else if (e.key.keysym.sym == SDLK_r) {
			pick = 0;
		} else if (e.key.keysym.sym == SDLK_l) {
			v3f p = body->get_p();
			v3f v = body->get_v();
			v3f r = body->get_r();
			log_info("pos %g,%g,%g", p.x, p.y, p.z);
			log_info("vel %g,%g,%g", v.x, v.y, v.z);
			log_info("rot %g,%g,%g", r.x, r.y, r.z);
			log_info("facing %d,%s", rot.x, face_names[rot.y]);
		}
	} else if (e.type == SDL_MOUSEBUTTONDOWN) {
		if (e.button.button == SDL_BUTTON_LEFT) {
			act = 1;
		} else if (e.button.button == SDL_BUTTON_RIGHT) {
			use = 1;
		}
	} else if (e.type == SDL_MOUSEBUTTONUP) {
		if (e.button.button == SDL_BUTTON_LEFT) {
			act = 0;
		} else if (e.button.button == SDL_BUTTON_RIGHT) {
			use = 0;
		}
	} else if (e.type == SDL_MOUSEWHEEL) {
		if (move.y0) {
			auto &mat = items[tool].mat;
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
			if (e.wheel.y > 0) {
				if (tool == 0)
					tool = items.size();
				--tool;
			} else if (e.wheel.y < 0) {
				++tool;
				if (tool == items.size())
					tool = 0;
			}
		}
	}
}

