/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "renderer.h"

#include "context.h"
#include "roaming_entity.h"
#include "block_entity.h"
#include "board_entity.h"

void populate_obj_vertex_buffer(VertexBuffer *vbo)
{
	std::vector<Vertex> a;
	for (int i = 0; i < MAT_COUNT; ++i) {
		v2f lt(1, 1);
		v2f mt[6];
		int tilted[6];
		v3f p;
		texcoord_up(i, mt, tilted);
		p = v3f(0, 0, 0);
		vertices_add(&a, vertices_face_dn, 6, p, lt, mt, tilted, nullptr);
		vertices_add(&a, vertices_face_lf, 6, p, lt, mt, tilted, nullptr);
		vertices_add(&a, vertices_face_bk, 6, p, lt, mt, tilted, nullptr);
		vertices_add(&a, vertices_face_up, 6, p, lt, mt, tilted, nullptr);
		vertices_add(&a, vertices_face_rt, 6, p, lt, mt, tilted, nullptr);
		vertices_add(&a, vertices_face_ft, 6, p, lt, mt, tilted, nullptr);

		vertices_add(&a, vertices_face_dn, 6, p, lt, mt, tilted, nullptr);
		vertices_add(&a, vertices_slab_dn, 30, p, lt, mt, tilted, nullptr);

		vertices_add(&a, vertices_face_dn, 6, p, lt, mt, tilted, nullptr);
		vertices_add(&a, vertices_face_lf, 6, p, lt, mt, tilted, nullptr);
		vertices_add(&a, vertices_stairs_dl, 42, p, lt, mt, tilted, nullptr);

		vertices_add(&a, vertices_pane_z, 36, p, lt, mt, tilted, nullptr);

		vertices_add(&a, vertices_token_lf, 30, p, lt, mt, tilted, nullptr);

		vertices_add(&a, vertices_token_bk, 30, p, lt, mt, tilted, nullptr);

		vertices_add(&a, vertices_token_rt, 30, p, lt, mt, tilted, nullptr);

		vertices_add(&a, vertices_token_ft, 30, p, lt, mt, tilted, nullptr);
	}
	vbo->update(0, a.data(), a.size());
}

void populate_text_vertex_buffer(VertexBuffer *vbo)
{
	std::vector<Vertex> a;
	int i = 0;
	for (auto p : box2s(0, 0, 15, 15)) {
		GLfloat u0 = p.y / 16.;
		GLfloat v0 = p.x / 16.;
		GLfloat u1 = u0 + 1. / 16.;
		GLfloat v1 = v0 + 1. / 16.;
		u0 += .25 / 16.;
		u1 -= .25 / 16.;
		a.push_back(Vertex(.5, 1, 0, u1, v0, 1, 1, 1, 1));
		a.push_back(Vertex(0, 1, 0, u0, v0, 1, 1, 1, 1));
		a.push_back(Vertex(.5, 0, 0, u1, v1, 1, 1, 1, 1));
		a.push_back(Vertex(.5, 0, 0, u1, v1, 1, 1, 1, 1));
		a.push_back(Vertex(0, 1, 0, u0, v0, 1, 1, 1, 1));
		a.push_back(Vertex(0, 0, 0, u0, v1, 1, 1, 1, 1));
		++i;
	}
	vbo->update(0, a.data(), a.size());
}

Renderer::Renderer(Context *ctx)
	: ctx(ctx)
{
	/* Load textures */
	tex_terrain = texture("data/materials.png");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	tex_font = texture("data/font.png");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	/* Setup camera */
	cam.reset(new Camera());
	cam->set_max_distance(1024);
	cam->set_aspect_ratio(1920.0 / 1080.0);

	/* Create vertex_buffers */
	shard_vertex_buffer.reset(new VertexBuffer(SHARDS_PER_WORLD));
	obj_vertex_buffer.reset(new VertexBuffer(1));
	text_vertex_buffer.reset(new VertexBuffer(1));
	populate_obj_vertex_buffer(obj_vertex_buffer.get());
	populate_text_vertex_buffer(text_vertex_buffer.get());

	tone_mapper.reset(new ToneMapper(1. / 30., 16));
	shader.reset(new Shader("data/shader.vert", "data/shader.frag"));
}

Renderer::~Renderer()
{
}

void Renderer::operator()()
{
	glEnable(GL_DEPTH_TEST);
	render_shards();
	// render_flowsim(this);
	for (auto f : callback_list)
		(*f)();
	ctx->player->render();
	glDisable(GL_DEPTH_TEST);
	render_commandline();
}

void Renderer::render_string(const char *str)
{
	shader->enable();

	glActiveTexture(GL_TEXTURE0);
	int tl0 = shader->get_uniform_location("Texture0");
	glUniform1i(tl0, 0);
	glBindTexture(GL_TEXTURE_2D, tone_mapper->get_texture());

	glActiveTexture(GL_TEXTURE1);
	int tl1 = shader->get_uniform_location("Texture1");
	glUniform1i(tl1, 1);
	glBindTexture(GL_TEXTURE_2D, tex_font);

	glActiveTexture(GL_TEXTURE0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	text_vertex_buffer->enable();
	while (*str) {
		text_vertex_buffer->draw_slice(GL_TRIANGLES, 0, *str * 6, 6);
		glTranslatef(.5, 0, 0);
		++str;
	}
	text_vertex_buffer->disable();
	glPopMatrix();
	shader->disable();
}

#if 0
void render_flowsim(Renderer *ctx)
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
#endif

void Renderer::render_item(int obj, int mat, GLfloat alpha)
{
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
		obj_vertex_buffer->draw_slice(GL_TRIANGLES, 0, 282 * mat + 0, 36);
		obj_vertex_buffer->disable();
	} else if (obj == OBJ_SLAB) {
		obj_vertex_buffer->enable();
		obj_vertex_buffer->draw_slice(GL_TRIANGLES, 0, 282 * mat + 36, 36);
		obj_vertex_buffer->disable();
	} else if (obj == OBJ_STAIRS) {
		obj_vertex_buffer->enable();
		obj_vertex_buffer->draw_slice(GL_TRIANGLES, 0, 282 * mat + 72, 54);
		obj_vertex_buffer->disable();
	} else if (obj == OBJ_PANE) {
		obj_vertex_buffer->enable();
		obj_vertex_buffer->draw_slice(GL_TRIANGLES, 0, 282 * mat + 126, 36);
		obj_vertex_buffer->disable();
	} else if (obj == OBJ_TOKEN_LF) {
		obj_vertex_buffer->enable();
		obj_vertex_buffer->draw_slice(GL_TRIANGLES, 0, 282 * mat + 162, 36);
		obj_vertex_buffer->disable();
	} else if (obj == OBJ_TOKEN_BK) {
		obj_vertex_buffer->enable();
		obj_vertex_buffer->draw_slice(GL_TRIANGLES, 0, 282 * mat + 192, 36);
		obj_vertex_buffer->disable();
	} else if (obj == OBJ_TOKEN_RT) {
		obj_vertex_buffer->enable();
		obj_vertex_buffer->draw_slice(GL_TRIANGLES, 0, 282 * mat + 222, 36);
		obj_vertex_buffer->disable();
	} else if (obj == OBJ_TOKEN_FT) {
		obj_vertex_buffer->enable();
		obj_vertex_buffer->draw_slice(GL_TRIANGLES, 0, 282 * mat + 252, 36);
		obj_vertex_buffer->disable();
	}
#if 0
	if (s.obj == OBJ_TOKEN) {
		v3f r = ctx->renderer->get_cam()->get_r();
		glTranslatef(.25 / side, .25 / side, .25 / side);
		glRotatef(180.0 * r.y / M_PI, 0, -1, 0);
		glRotatef(180.0 * r.x / M_PI, 1, 0, 0);
		glTranslatef(-.25 / side, -.25 / side, -.25 / side);
	}
#endif
	shader->disable();
}

void Renderer::render_inventory(const std::vector<Item> &inv, const v3ll &p)
{
	GLfloat side = sqrt(inv.size());
	GLfloat d = dist(ctx->player->get_body()->get_p(), v3f(p));
	GLubyte alpha = d > 4 ? 0 : d < 2 ? 255 : 255 * (2 - d / 2);
	int i = 0;
	for (auto q : box2s(0, 0, side - 1, side - 1)) {
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glTranslatef(p.x + q.x / side, p.y + 1.0001, p.z + q.y / side);
		{
			glPushMatrix();
			glTranslatef(.125 / side, 0, .125 / side);
			glScalef(.75 / side, .75 / side, .75 / side);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glColor4ub(0, 0, 0, alpha / 2);
			glBegin(GL_TRIANGLES);
			glVertex3f(0, 0, 0);
			glVertex3f(0, 0, 1);
			glVertex3f(1, 0, 0);
			glVertex3f(1, 0, 0);
			glVertex3f(0, 0, 1);
			glVertex3f(1, 0, 1);
			glEnd();
			glDisable(GL_BLEND);
			glPopMatrix();
		}
		Item s = inv[i];
		if (s.num > 0) {
			glPushMatrix();
			glTranslatef(.5 / side - .0625, 0, .5 / side - .0625);
			glScalef(.125, .125, .125);
			render_item(s.obj, s.mat, alpha);
			glPopMatrix();
		}
		if (s.num > 1) {
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glTranslatef(.5 / side, .125, .5 / side);
			glRotatef(180.0 * cam->get_r().y / M_PI, 0, -1, 0);
			glRotatef(180.0 * cam->get_r().x / M_PI, 1, 0, 0);
			glScalef(.05, .05, .05);
			glTranslatef(-.5, 0, 0);
			char buf[3];
			snprintf(buf, sizeof(buf), "%02d", s.num);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glColor4ub(255, 255, 255, alpha);
			render_string(buf);
			glDisable(GL_BLEND);
			glPopMatrix();
		}
		glPopMatrix();
		++i;
	}
}

void Renderer::render_board(const std::vector<Item> &inv, const v3ll &p)
{
	GLfloat side = sqrt(inv.size());
	GLfloat d = dist(ctx->player->get_body()->get_p(), v3f(p));
	GLubyte alpha = d > 4 ? 0 : d < 2 ? 255 : 255 * (2 - d / 2);
	int i = 0;
	for (auto q : box2s(0, 0, side - 1, side - 1)) {
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glTranslatef(p.x + q.x / side, p.y + 1.0001, p.z + q.y / side);
		{
			glPushMatrix();
			glTranslatef(.125 / side, 0, .125 / side);
			glScalef(.75 / side, .75 / side, .75 / side);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glColor4ub(0, 0, 0, alpha / 2);
			glBegin(GL_TRIANGLES);
			glVertex3f(0, 0, 0);
			glVertex3f(0, 0, 1);
			glVertex3f(1, 0, 0);
			glVertex3f(1, 0, 0);
			glVertex3f(0, 0, 1);
			glVertex3f(1, 0, 1);
			glEnd();
			glDisable(GL_BLEND);
			glPopMatrix();
		}
		Item s = inv[i];
		if (s.num > 0) {
			glColor4ub(0, 0, 0, alpha);
			glPushMatrix();
			glTranslatef(.5 / side - .06, 0, .5 / side - .06);
			glScalef(.12, .12, .12);
			render_item(s.obj, s.mat, alpha);
			glPopMatrix();
		}
		if ((s.num & 2) != 0) {
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glTranslatef(.5 / side, .125, .5 / side);
			glRotatef(180.0 * cam->get_r().y / M_PI, 0, -1, 0);
			glRotatef(180.0 * cam->get_r().x / M_PI, 1, 0, 0);
			glScalef(.05, .05, .05);
			glTranslatef(-.5, 0, 0);
			glColor4ub(255, 255, 255, alpha);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			render_string("ON");
			glDisable(GL_BLEND);
			glPopMatrix();
		}
		glPopMatrix();
		++i;
	}
}

void Renderer::render_commandline()
{
	if (ctx->mode == MODE_COMMAND) {
		v3f p = cam->get_p();
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glTranslatef(p.x, p.y, p.z);
		glRotatef(180.0 * cam->get_r().y / M_PI, 0, -1, 0);
		glRotatef(180.0 * cam->get_r().x / M_PI, 1, 0, 0);
		glTranslatef(-15, 0, -30);
		if (ctx->cli->get_visible()) {
			glColor3f(1, 1, 1);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			render_string(ctx->cli->get_visible());
			glDisable(GL_BLEND);
		}
		glTranslatef(.5 * ctx->cli->get_cur_char(), 0, 0);
		glColor3f(1, 1, 1);
		glEnable(GL_BLEND);
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
		glPopMatrix();
	}
}

void Renderer::render_shards()
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
			c = ctx->world->get_chunk(v2ll(x, z));
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

static const char has_left_side[256] = {
	0,
	1, 1, 1, 1, 1, 1,
	0, 0, 1, 0, 0, 0,
	0, 0, 0,
	0, 1, 0, 0, 0, 1, 0, 0,
	0, 0, 1, 0, 0, 0,
};

static const char has_right_side[256] = {
	0,
	1, 1, 1, 1, 1, 1,
	0, 0, 0, 1, 0, 0,
	0, 0, 0,
	0, 0, 0, 1, 0, 0, 0, 1,
	0, 0, 0, 1, 0, 0,
};

static const char has_down_side[256] = {
	0,
	1, 1, 1, 1, 1, 1,
	1, 0, 0, 0, 0, 0,
	0, 0, 0,
	1, 1, 1, 1, 0, 0, 0, 0,
	1, 0, 0, 0, 0, 0,
};

static const char has_up_side[256] = {
	0,
	1, 1, 1, 1, 1, 1,
	0, 1, 0, 0, 0, 0,
	0, 0, 0,
	0, 0, 0, 0, 1, 1, 1, 1,
	0, 1, 0, 0, 0, 0,
};

static const char has_back_side[256] = {
	0,
	1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 1, 0,
	0, 0, 0,
	0, 0, 1, 0, 0, 0, 1, 0,
	0, 0, 0, 0, 1, 0,
};

static const char has_front_side[256] = {
	0,
	1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 1,
	0, 0, 0,
	1, 0, 0, 0, 1, 0, 0, 0,
	0, 0, 0, 0, 0, 1,
};

void Renderer::update_face_lf(std::vector<Vertex> *buf,
		int64_t x, int64_t y, int64_t z, const v2f *mt, const int *tilted)
{
	v2f lt;
	if (has_right_side[ctx->world->get_shape(v3ll(x - 1, y, z))])
		return;
	lt = texcoord_from_light(ctx->world->get_light(v3ll(x - 1, y, z)));
	vertices_add(buf, vertices_face_lf, 6, v3f(x, y, z), lt, mt, tilted, ctx->world.get());
}

void Renderer::update_face_rt(std::vector<Vertex> *buf,
		int64_t x, int64_t y, int64_t z, const v2f *mt, const int *tilted)
{
	v2f lt;
	if (has_left_side[ctx->world->get_shape(v3ll(x + 1, y, z))])
		return;
	lt = texcoord_from_light(ctx->world->get_light(v3ll(x + 1, y, z)));
	vertices_add(buf, vertices_face_rt, 6, v3f(x, y, z), lt, mt, tilted, ctx->world.get());
}

void Renderer::update_face_dn(std::vector<Vertex> *buf,
		int64_t x, int64_t y, int64_t z, const v2f *mt, const int *tilted)
{
	v2f lt;
	if (has_up_side[ctx->world->get_shape(v3ll(x, y - 1, z))])
		return;
	lt = texcoord_from_light(ctx->world->get_light(v3ll(x, y - 1, z)));
	vertices_add(buf, vertices_face_dn, 6, v3f(x, y, z), lt, mt, tilted, ctx->world.get());
}

void Renderer::update_face_up(std::vector<Vertex> *buf,
		int64_t x, int64_t y, int64_t z, const v2f *mt, const int *tilted)
{
	v2f lt;
	if (has_down_side[ctx->world->get_shape(v3ll(x, y + 1, z))])
		return;
	lt = texcoord_from_light(ctx->world->get_light(v3ll(x, y + 1, z)));
	vertices_add(buf, vertices_face_up, 6, v3f(x, y, z), lt, mt, tilted, ctx->world.get());
}

void Renderer::update_face_bk(std::vector<Vertex> *buf,
		int64_t x, int64_t y, int64_t z, const v2f *mt, const int *tilted)
{
	v2f lt;
	if (has_front_side[ctx->world->get_shape(v3ll(x, y, z - 1))])
		return;
	lt = texcoord_from_light(ctx->world->get_light(v3ll(x, y, z - 1)));
	vertices_add(buf, vertices_face_bk, 6, v3f(x, y, z), lt, mt, tilted, ctx->world.get());
}

void Renderer::update_face_ft(std::vector<Vertex> *buf,
		int64_t x, int64_t y, int64_t z, const v2f *mt, const int *tilted)
{
	v2f lt;
	if (has_back_side[ctx->world->get_shape(v3ll(x, y, z + 1))])
		return;
	lt = texcoord_from_light(ctx->world->get_light(v3ll(x, y, z + 1)));
	vertices_add(buf, vertices_face_ft, 6, v3f(x, y, z), lt, mt, tilted, ctx->world.get());
}

void Renderer::update_cell(std::vector<Vertex> *buf, int64_t x, int64_t y,
		int64_t z)
{
	int8_t s, sl, sd, sb, sr, su, sf;
	v2f mt[6];
	int tilted[6];
	v2f lt;

	s = ctx->world->get_shape(v3ll(x, y, z));
	if (s == SHAPE_NONE)
		return;

	if (s == SHAPE_BLOCK_DN) {
		texcoord_up(ctx->world->get_mat(v3ll(x, y, z)), mt, tilted);
		update_face_lf(buf, x, y, z, mt, tilted);
		update_face_rt(buf, x, y, z, mt, tilted);
		update_face_dn(buf, x, y, z, mt, tilted);
		update_face_up(buf, x, y, z, mt, tilted);
		update_face_bk(buf, x, y, z, mt, tilted);
		update_face_ft(buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_BLOCK_UP) {
		texcoord_dn(ctx->world->get_mat(v3ll(x, y, z)), mt, tilted);
		update_face_lf(buf, x, y, z, mt, tilted);
		update_face_rt(buf, x, y, z, mt, tilted);
		update_face_dn(buf, x, y, z, mt, tilted);
		update_face_up(buf, x, y, z, mt, tilted);
		update_face_bk(buf, x, y, z, mt, tilted);
		update_face_ft(buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_BLOCK_LF) {
		texcoord_lf(ctx->world->get_mat(v3ll(x, y, z)), mt, tilted);
		update_face_lf(buf, x, y, z, mt, tilted);
		update_face_rt(buf, x, y, z, mt, tilted);
		update_face_dn(buf, x, y, z, mt, tilted);
		update_face_up(buf, x, y, z, mt, tilted);
		update_face_bk(buf, x, y, z, mt, tilted);
		update_face_ft(buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_BLOCK_RT) {
		texcoord_rt(ctx->world->get_mat(v3ll(x, y, z)), mt, tilted);
		update_face_lf(buf, x, y, z, mt, tilted);
		update_face_rt(buf, x, y, z, mt, tilted);
		update_face_dn(buf, x, y, z, mt, tilted);
		update_face_up(buf, x, y, z, mt, tilted);
		update_face_bk(buf, x, y, z, mt, tilted);
		update_face_ft(buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_BLOCK_BK) {
		texcoord_bk(ctx->world->get_mat(v3ll(x, y, z)), mt, tilted);
		update_face_lf(buf, x, y, z, mt, tilted);
		update_face_rt(buf, x, y, z, mt, tilted);
		update_face_dn(buf, x, y, z, mt, tilted);
		update_face_up(buf, x, y, z, mt, tilted);
		update_face_bk(buf, x, y, z, mt, tilted);
		update_face_ft(buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_BLOCK_FT) {
		texcoord_ft(ctx->world->get_mat(v3ll(x, y, z)), mt, tilted);
		update_face_lf(buf, x, y, z, mt, tilted);
		update_face_rt(buf, x, y, z, mt, tilted);
		update_face_dn(buf, x, y, z, mt, tilted);
		update_face_up(buf, x, y, z, mt, tilted);
		update_face_bk(buf, x, y, z, mt, tilted);
		update_face_ft(buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_SLAB_DN) {
		texcoord_up(ctx->world->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->world->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_slab_dn, 30, v3f(x, y, z), lt, mt, tilted, ctx->world.get());
		update_face_dn(buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_SLAB_UP) {
		texcoord_up(ctx->world->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->world->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_slab_up, 30, v3f(x, y, z), lt, mt, tilted, ctx->world.get());
		update_face_up(buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_SLAB_LF) {
		texcoord_up(ctx->world->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->world->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_slab_lf, 30, v3f(x, y, z), lt, mt, tilted, ctx->world.get());
		update_face_lf(buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_SLAB_RT) {
		texcoord_up(ctx->world->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->world->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_slab_rt, 30, v3f(x, y, z), lt, mt, tilted, ctx->world.get());
		update_face_rt(buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_SLAB_BK) {
		texcoord_up(ctx->world->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->world->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_slab_bk, 30, v3f(x, y, z), lt, mt, tilted, ctx->world.get());
		update_face_bk(buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_SLAB_FT) {
		texcoord_up(ctx->world->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->world->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_slab_ft, 30, v3f(x, y, z), lt, mt, tilted, ctx->world.get());
		update_face_ft(buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_STAIRS_DF) {
		texcoord_up(ctx->world->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->world->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_stairs_df, 42, v3f(x, y, z), lt, mt, tilted, ctx->world.get());
		update_face_dn(buf, x, y, z, mt, tilted);
		update_face_ft(buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_STAIRS_DL) {
		texcoord_up(ctx->world->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->world->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_stairs_dl, 42, v3f(x, y, z), lt, mt, tilted, ctx->world.get());
		update_face_dn(buf, x, y, z, mt, tilted);
		update_face_lf(buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_STAIRS_DB) {
		texcoord_up(ctx->world->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->world->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_stairs_db, 42, v3f(x, y, z), lt, mt, tilted, ctx->world.get());
		update_face_dn(buf, x, y, z, mt, tilted);
		update_face_bk(buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_STAIRS_DR) {
		texcoord_up(ctx->world->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->world->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_stairs_dr, 42, v3f(x, y, z), lt, mt, tilted, ctx->world.get());
		update_face_dn(buf, x, y, z, mt, tilted);
		update_face_rt(buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_STAIRS_UF) {
		texcoord_up(ctx->world->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->world->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_stairs_uf, 42, v3f(x, y, z), lt, mt, tilted, ctx->world.get());
		update_face_up(buf, x, y, z, mt, tilted);
		update_face_ft(buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_STAIRS_UL) {
		texcoord_up(ctx->world->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->world->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_stairs_ul, 42, v3f(x, y, z), lt, mt, tilted, ctx->world.get());
		update_face_up(buf, x, y, z, mt, tilted);
		update_face_lf(buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_STAIRS_UB) {
		texcoord_up(ctx->world->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->world->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_stairs_ub, 42, v3f(x, y, z), lt, mt, tilted, ctx->world.get());
		update_face_up(buf, x, y, z, mt, tilted);
		update_face_bk(buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_STAIRS_UR) {
		texcoord_up(ctx->world->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->world->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_stairs_ur, 42, v3f(x, y, z), lt, mt, tilted, ctx->world.get());
		update_face_up(buf, x, y, z, mt, tilted);
		update_face_rt(buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_PANE_Y) {
		texcoord_up(ctx->world->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->world->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_pane_y, 36, v3f(x, y, z), lt, mt, tilted, ctx->world.get());
	} else if (s == SHAPE_PANE_X) {
		texcoord_up(ctx->world->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->world->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_pane_x, 36, v3f(x, y, z), lt, mt, tilted, ctx->world.get());
	} else if (s == SHAPE_PANE_Z) {
		texcoord_up(ctx->world->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->world->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_pane_z, 36, v3f(x, y, z), lt, mt, tilted, ctx->world.get());
	} else if (s == SHAPE_PANE_DN) {
		texcoord_up(ctx->world->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->world->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_pane_dn, 30, v3f(x, y, z), lt, mt, tilted, ctx->world.get());
		update_face_dn(buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_PANE_UP) {
		texcoord_up(ctx->world->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->world->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_pane_up, 30, v3f(x, y, z), lt, mt, tilted, ctx->world.get());
		update_face_up(buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_PANE_LF) {
		texcoord_up(ctx->world->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->world->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_pane_lf, 30, v3f(x, y, z), lt, mt, tilted, ctx->world.get());
		update_face_lf(buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_PANE_RT) {
		texcoord_up(ctx->world->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->world->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_pane_rt, 30, v3f(x, y, z), lt, mt, tilted, ctx->world.get());
		update_face_rt(buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_PANE_BK) {
		texcoord_up(ctx->world->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->world->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_pane_bk, 30, v3f(x, y, z), lt, mt, tilted, ctx->world.get());
		update_face_bk(buf, x, y, z, mt, tilted);
	} else if (s == SHAPE_PANE_FT) {
		texcoord_up(ctx->world->get_mat(v3ll(x, y, z)), mt, tilted);
		lt = texcoord_from_light(ctx->world->get_light(v3ll(x, y, z)));
		vertices_add(buf, vertices_pane_ft, 30, v3f(x, y, z), lt, mt, tilted, ctx->world.get());
		update_face_ft(buf, x, y, z, mt, tilted);
	}
}

void Renderer::update_shard(int id, int64_t x0, int64_t y0, int64_t z0)
{
	int64_t x1, y1, z1;
	int64_t x, y, z;
	std::vector<Vertex> buf;

	x1 = x0 + SHARD_W;
	y1 = y0 + SHARD_H;
	z1 = z0 + SHARD_D;

	for (x = x0; x < x1; ++x) {
		for (y = y0; y < y1; ++y) {
			for (z = z0; z < z1; ++z) {
				update_cell(&buf, x, y, z);
			}
		}
	}

	shard_vertex_buffer->update(id, buf.data(), buf.size());
}

void Renderer::update_camera()
{
	float fovy = ctx->player->get_run() ? 70.f : 60.f;
	cam->set_fovy((cam->get_fovy() + fovy) * 0.5);
	cam->set_p(ctx->player->get_body()->get_p() + v3f(0, .8, 0));
	cam->set_r(ctx->player->get_body()->get_r());
	v3ll p(cam->get_p());
	tone_mapper->update((ctx->world->get_light(p) << 4) / 255., 0);
}

