
#include "drop.h"

void drop_callback(struct body *b, void *udata, struct v3ll p, int face)
{
	struct drop *d = udata;
	if (face != FACE_UP)
		return;
	if (d->num) {
		int s = world_get_shape(d->ctx->w, p);
		int m = world_get_mat(d->ctx->w, p);
		if (block_capacity[m][s] > 0) {
			struct array *inv = world_get_data(d->ctx->w, p);
			d->num -= inventory_add(inv, slot(d->obj, d->mat, d->num));
		}
	}
	b->v.x *= .8;
	b->v.z *= .8;
}

struct drop *drop(struct context *ctx, uint8_t obj, uint8_t mat, uint8_t num)
{
	struct drop *d = calloc(1, sizeof(*d));
	list_link(&ctx->drops, &d->list);
	d->body = body(ctx->space);
	body_set_size(d->body, v2f(.0625, .0625));
	body_set_callback(d->body, drop_callback, d);
	d->ctx = ctx;
	d->obj = obj;
	d->mat = mat;
	d->num = num;
	return d;
}

void drop_destroy(struct drop *d)
{
	list_unlink(&d->list);
	body_destroy(d->body);
	free(d);
}


