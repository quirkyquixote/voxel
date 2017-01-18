/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

 
#include "context.h"
#include "roaming_entity.h"
#include "block_entity.h"

void Context::render_string(const char *str)
{
	GLfloat u0, v0, u1, v1;
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex_font);
	while (*str) {
		u0 = (*str % 16) / 16.;
		v0 = (*str / 16) / 16.;
		u1 = u0 + 1 / 16.;
		v1 = v0 + 1 / 16.;
		u0 += .25 / 16.;
		u1 -= .25 / 16.;
		glBegin(GL_TRIANGLES);
		glTexCoord2d(u1, v0);
		glVertex3f(.5, 1, 0);
		glTexCoord2d(u0, v0);
		glVertex3f(0, 1, 0);
		glTexCoord2d(u1, v1);
		glVertex3f(.5, 0, 0);
		glTexCoord2d(u1, v1);
		glVertex3f(.5, 0, 0);
		glTexCoord2d(u0, v0);
		glVertex3f(0, 1, 0);
		glTexCoord2d(u0, v1);
		glVertex3f(0, 0, 0);
		glEnd();
		glTranslatef(.5, 0, 0);
		++str;
	}
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}
/*
void render_flowsim(Context *ctx)
{
	struct fs_volume *v;
	struct fs_layer *l;
	v3ll p;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1, 0, 0, .5);
	glBegin(GL_TRIANGLES);
	list_foreach(v, &flowsim->volumes, volumes) {
		list_foreach(l, &v->layers, layers) {
			if (!l->is_top) {
				array_foreach(p, l->cells) {
					float y = l->y + 1;
					glVertex3f(p.x + 1, y, p.z + 1);
					glVertex3f(p.x + 1, y, p.z);
					glVertex3f(p.x, y, p.z + 1);
					glVertex3f(p.x, y, p.z + 1);
					glVertex3f(p.x + 1, y, p.z);
					glVertex3f(p.x, y, p.z);
				}
			}
		}
	}
	glEnd();
	glColor4f(0, 0, 1, .5);
	glBegin(GL_TRIANGLES);
	list_foreach(v, &flowsim->volumes, volumes) {
		list_foreach(l, &v->layers, layers) {
			if (l->is_top) {
				array_foreach(p, l->cells) {
					float y = v->top + l->y + .001;
					glVertex3f(p.x + 1, y, p.z + 1);
					glVertex3f(p.x + 1, y, p.z);
					glVertex3f(p.x, y, p.z + 1);
					glVertex3f(p.x, y, p.z + 1);
					glVertex3f(p.x + 1, y, p.z);
					glVertex3f(p.x, y, p.z);
				}
			}
		}
	}
	glEnd();
	glDisable(GL_BLEND);
}
*/
void Context::render_cursor()
{
	if (cur.face == -1)
		return;

	GLfloat x = cur.p.x;
	GLfloat y = cur.p.y;
	GLfloat z = cur.p.z;

	if (cur.face == FACE_UP && w->get_data(cur.p) != nullptr)
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


void Context::render_item(int obj, int mat, GLfloat alpha)
{
	//glEnable(GL_TEXTURE_2D);
	//glTexCoord2f(0.5, 0.5);

	if (mat >= MAT_COUNT)
		mat = 0;

	shader->enable();

	glActiveTexture(GL_TEXTURE0);
	int tl0 = shader->get_uniform_location("Texture0");
	glUniform1i(tl0, 0);
	glBindTexture(GL_TEXTURE_2D, tone_mapper->get_texture());

	glActiveTexture(GL_TEXTURE1);
	int tl1 = shader->get_uniform_location("Texture1");
	glUniform1i(tl1, 1);
	glBindTexture(GL_TEXTURE_2D, tex_terrain);

	glActiveTexture(GL_TEXTURE0);
	if (obj == OBJ_BLOCK) {
		obj_vertex_buffer->enable();
		obj_vertex_buffer->draw_slice(GL_TRIANGLES, 0, 162 * mat + 0, 36);
		obj_vertex_buffer->disable();
	} else if (obj == OBJ_SLAB) {
		obj_vertex_buffer->enable();
		obj_vertex_buffer->draw_slice(GL_TRIANGLES, 0, 162 * mat + 36, 36);
		obj_vertex_buffer->disable();
	} else if (obj == OBJ_STAIRS) {
		obj_vertex_buffer->enable();
		obj_vertex_buffer->draw_slice(GL_TRIANGLES, 0, 162 * mat + 72, 54);
		obj_vertex_buffer->disable();
	} else if (obj == OBJ_PANE) {
		obj_vertex_buffer->enable();
		obj_vertex_buffer->draw_slice(GL_TRIANGLES, 0, 162 * mat + 126, 36);
		obj_vertex_buffer->disable();
	}
	shader->disable();
	//glDisable(GL_TEXTURE_2D);
}

void Context::render_inventory(const std::vector<Item> &inv, const v3ll &p)
{
	int i, x, z;
	int side = sqrt(inv.size());
	Item s;

	GLfloat d = dist(player->get_p(), v3f(p));
	GLubyte alpha = d > 4 ? 0 : d < 2 ? 255 : 255 * (2 - d / 2);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	i = 0;
	for (x = 0; x < side; ++x) {
		for (z = 0; z < side; ++z) {
			glColor4ub(0, 0, 0, alpha / 2);
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glTranslatef(p.x + (GLfloat)x / side, p.y + 1.0001, p.z + (GLfloat)z / side);
			glPushMatrix();
			glTranslatef(.125 / side, 0, .125 / side);
			glScalef(.75 / side, .75 / side, .75 / side);
			glBegin(GL_TRIANGLES);
			glVertex3f(0, 0, 0);
			glVertex3f(0, 0, 1);
			glVertex3f(1, 0, 0);
			glVertex3f(1, 0, 0);
			glVertex3f(0, 0, 1);
			glVertex3f(1, 0, 1);
			glEnd();
			glPopMatrix();
			Item s = inv[i];
			if (s.num > 0) {
				glColor4ub(0, 0, 0, alpha);
				glPushMatrix();
				glTranslatef(.25 / side, 0, .25 / side);
				glScalef(.5 / side, .5 / side, .5 / side);
				render_item(s.obj, s.mat, alpha);
				glPopMatrix();
			}
			if (s.num > 1) {
				glColor4ub(255, 255, 255, alpha);
				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();
				glTranslatef(.5 / side, .5 / side, .5 / side);
				glRotatef(180.0 * cam->get_r().y / M_PI, 0, -1, 0);
				glRotatef(180.0 * cam->get_r().x / M_PI, 1, 0, 0);
				glScalef(.05, .05, .05);
				char buf[3];
				snprintf(buf, sizeof(buf), "%02d", s.num);
				render_string(buf);
				glPopMatrix();
			}
			glPopMatrix();
			++i;
		}
	}
	glDisable(GL_BLEND);
}

void Context::render_block_entities()
{
	v3f p = player->get_p();
	box3ll bb;
	bb.x0 = floor(p.x - 4);
	bb.y0 = floor(p.y - 4);
	bb.z0 = floor(p.z - 4);
	bb.x1 = ceil(p.x + 4);
	bb.y1 = ceil(p.y + 4);
	bb.z1 = ceil(p.z + 4);

	for (auto &p : bb) {
		Entity *e = w->get_data(p);
		if (e != NULL)
			e->render();
	}
}

void Context::render_roaming_entities()
{
	for (auto &e : entities)
		e->render();
}

void Context::render_held_item()
{
	glDisable(GL_DEPTH_TEST);

	Item s = inv[tool];
	if (s.num > 0) {
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		v3f p = cam->get_p();
		glTranslatef(p.x, p.y, p.z);
		glRotatef(180.0 * cam->get_r().y / M_PI, 0, -1, 0);
		glRotatef(180.0 * cam->get_r().x / M_PI, 1, 0, 0);
		glTranslatef(.4, -.4, -.8);
		glScalef(.125, .125, .125);
		render_item(s.obj, s.mat, 255);
		glPopMatrix();
	}
}

void Context::render_hotbar()
{
	for (int i = 0; i < 9; ++i) {
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		v3f p = cam->get_p();
		glTranslatef(p.x, p.y, p.z);
		glRotatef(180.0 * cam->get_r().y / M_PI, 0, -1, 0);
		glRotatef(180.0 * cam->get_r().x / M_PI, 1, 0, 0);
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
		Item s = inv[i];
		if (s.num > 0)
			render_item(s.obj, s.mat, 255);
		if (s.num > 1) {
			glEnable(GL_BLEND);
			glColor3f(1, 1, 1);
			glTranslatef(0, -1.5, 0);
			char buf[3];
			snprintf(buf, sizeof(buf), "%02d", s.num);
			render_string(buf);
			glDisable(GL_BLEND);
		}
		glPopMatrix();
	}
}

void Context::render_commandline()
{
	if (mode == MODE_COMMAND) {
		glEnable(GL_BLEND);
		glColor3f(1, 1, 1);
		v3f p = cam->get_p();
		glTranslatef(p.x, p.y, p.z);
		glRotatef(180.0 * cam->get_r().y / M_PI, 0, -1, 0);
		glRotatef(180.0 * cam->get_r().x / M_PI, 1, 0, 0);
		glTranslatef(-15, 0, -30);
		if (cli->get_visible())
			render_string(cli->get_visible());
		glTranslatef(.5 * cli->get_cur_char(), 0, 0);
		glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
		glBegin(GL_TRIANGLES);
		glVertex3f(.5, 1, 0);
		glVertex3f(0, 1, 0);
		glVertex3f(.5, 0, 0);
		glVertex3f(.5, 0, 0);
		glVertex3f(0, 1, 0);
		glVertex3f(0, 0, 0);
		glEnd();
		glDisable(GL_BLEND);
	}
	glEnable(GL_DEPTH_TEST);
}

void Context::render_shards()
{
	int64_t x, y, z;
	v3f p;
	Chunk *c;
	Shard *s;
	int shards_rendered;

	shards_rendered = 0;
	cam->load_gl_matrices();
	shader->enable();

	glActiveTexture(GL_TEXTURE0);
	int tl0 = shader->get_uniform_location("Texture0");
	glUniform1i(tl0, 0);
	glBindTexture(GL_TEXTURE_2D, tone_mapper->get_texture());

	glActiveTexture(GL_TEXTURE1);
	int tl1 = shader->get_uniform_location("Texture1");
	glUniform1i(tl1, 1);
	glBindTexture(GL_TEXTURE_2D, tex_terrain);

	glActiveTexture(GL_TEXTURE0);
	shard_vertex_buffer->enable();
	for (x = 0; x < CHUNKS_PER_WORLD; ++x) {
		for (z = 0; z < CHUNKS_PER_WORLD; ++z) {
			c = w->get_chunk(v2ll(x, z));
			p.x = c->get_p().x + CHUNK_W / 2;
			p.z = c->get_p().y + CHUNK_W / 2;
			for (y = 0; y < SHARDS_PER_CHUNK; ++y) {
				s = c->get_shard(y);
				if (shard_vertex_buffer->get_size(s->get_id()) == 0)
					continue;
				p.y = (s->get_y() + 0.5) * SHARD_W;
				if (!cam->is_visible(p, SHARD_W))
					continue;
				shard_vertex_buffer->draw(GL_TRIANGLES, s->get_id());
				++shards_rendered;
			}
		}
	}
	shard_vertex_buffer->disable();
	shader->disable();
}

void Context::render()
{
	render_shards();
	render_cursor();
//	render_flowsim(this);
	render_block_entities();
	render_roaming_entities();
	render_held_item();
	render_hotbar();
	render_commandline();
}


