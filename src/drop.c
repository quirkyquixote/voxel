
#include "drop.h"

#include "render.h"

void drop_destroy(void *data);
void drop_update(void *data);
void drop_render(void *data);

static const struct entity_traits drop_traits = {
	drop_update,
	drop_render,
	drop_destroy,
};

void drop_callback(struct body *b, void *udata, struct v3ll p, int face)
{
	struct drop *d = udata;
	if (face != FACE_UP)
		return;
	if (d->num) {
		int s = world_get_shape(d->ctx->w, p);
		int m = world_get_mat(d->ctx->w, p);
		if (block_traits[m][s].capacity > 0) {
			struct array *inv = world_get_data(d->ctx->w, p);
			if (inv == NULL)
				log_warning("expected inventory");
			else
				d->num -= inventory_add(inv, slot(d->obj, d->mat, d->num));
		}
	}
	b->v.x *= .8;
	b->v.z *= .8;
}

struct drop *drop(struct context *ctx, uint8_t obj, uint8_t mat, uint8_t num)
{
	struct drop *d = calloc(1, sizeof(*d));
	d->entity.traits = &drop_traits;
	list_link(&ctx->entities, &d->entity.entities);
	d->entity.body = body(ctx->space);
	body_set_size(d->entity.body, v2f(.0625, .0625));
	body_set_callback(d->entity.body, drop_callback, d);
	d->ctx = ctx;
	d->obj = obj;
	d->mat = mat;
	d->num = num;
	return d;
}

void drop_destroy(void *data)
{
	struct drop *d = data;
	list_unlink(&d->entity.entities);
	body_destroy(d->entity.body);
	free(d);
}

void drop_update(void *data)
{
	struct drop *d = data;
	++d->ticks;
	if (d->ticks > 10 && box3_overlap(box3_grow(d->entity.body->bb, 1), d->ctx->player->bb))
		d->num -= inventory_add(d->ctx->inv, slot(d->obj, d->mat, d->num));
	if (d->ticks > 1800 || d->num == 0)
		d->entity.die = 1;
}

void drop_render(void *data)
{
	struct drop *d = data;
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(d->entity.body->p.x, d->entity.body->p.y, d->entity.body->p.z);
	glScalef(.25, .25, .25);
	render_obj(d->ctx, d->obj, d->mat, 255);
	glPopMatrix();
}
