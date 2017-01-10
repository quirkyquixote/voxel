

#include "render.h"

#include "context.h"
#include "drop.h"

void render_flowsim(struct context *ctx)
{
	struct fs_volume *v;
	struct fs_layer *l;
	struct v3ll p;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0, 0, 0, .5);
	glBegin(GL_TRIANGLES);
	list_foreach(v, &ctx->flowsim->volumes, volumes) {
		list_foreach(l, &v->top_layers, top_layers) {
			stack_foreach(p, l->cells) {
				glVertex3f(p.x + 1, l->top, p.z + 1);
				glVertex3f(p.x + 1, l->top, p.z);
				glVertex3f(p.x, l->top, p.z + 1);
				glVertex3f(p.x, l->top, p.z + 1);
				glVertex3f(p.x + 1, l->top, p.z);
				glVertex3f(p.x, l->top, p.z);
			}
		}
	}
	glEnd();
	glDisable(GL_BLEND);
}

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
			obj == OBJ_FLUID || obj == OBJ_PIPE) {
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
	int s, i;

	render_cursor(ctx);
	bb.x0 = floor(ctx->player->p.x - 4);
	bb.y0 = floor(ctx->player->p.y - 4);
	bb.z0 = floor(ctx->player->p.z - 4);
	bb.x1 = ceil(ctx->player->p.x + 4);
	bb.y1 = ceil(ctx->player->p.y + 4);
	bb.z1 = ceil(ctx->player->p.z + 4);

	aab3_foreach(p, bb) {
		s = WORLD_AT(ctx->w, shape, p.x, p.y, p.z);
		if (s == SHAPE_WORKBENCH || s == SHAPE_CRATE)
			render_inventory(ctx, WORLD_AT(ctx->w, data, p.x, p.y, p.z), p);
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

void render(void *data)
{
	struct context *ctx = data;
	int64_t x, y, z;
	struct v3f p;
	struct chunk *c;
	struct shard *s;
	int shards_rendered;

	shards_rendered = 0;
	camera_load_gl_matrices(ctx->cam);
	shader_enable(ctx->shader);

	glActiveTexture(GL_TEXTURE0);
	int tl0 = glGetUniformLocation(ctx->shader->pobj, "Texture0");
	glUniform1i(tl0, 0);
	glBindTexture(GL_TEXTURE_2D, ctx->tone_mapper->texture);

	glActiveTexture(GL_TEXTURE1);
	int tl1 = glGetUniformLocation(ctx->shader->pobj, "Texture1");
	glUniform1i(tl1, 1);
	glBindTexture(GL_TEXTURE_2D, ctx->tex_terrain);

	glActiveTexture(GL_TEXTURE0);
	vertex_buffer_enable(ctx->shard_vertex_buffer);
	for (x = 0; x < CHUNKS_PER_WORLD; ++x) {
		for (z = 0; z < CHUNKS_PER_WORLD; ++z) {
			c = ctx->w->chunks[x][z];
			p.x = c->x + CHUNK_W / 2;
			p.z = c->z + CHUNK_W / 2;
			for (y = 0; y < SHARDS_PER_CHUNK; ++y) {
				s = c->shards[y];
				if (ctx->shard_vertex_buffer->vbo_sizes[s->id] == 0)
					continue;
				p.y = (s->y + 0.5) * SHARD_W;
				if (camera_visible(ctx->cam, p, SHARD_W) == 0)
					continue;
				vertex_buffer_draw(ctx->shard_vertex_buffer, GL_TRIANGLES, s->id);
				++shards_rendered;
			}
		}
	}
	vertex_buffer_disable(ctx->shard_vertex_buffer);
	shader_enable(NULL);
	render_flowsim(ctx);
	roam_render(ctx);
}


