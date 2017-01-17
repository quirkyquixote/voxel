
#include "drop.h"

#include "render.h"

void *drop_create(struct context *ctx);
void drop_destroy(void *data);
void drop_update(void *data);
void drop_render(void *data);
int drop_save(void *data, union sz_tag *root);
int drop_load(void *data, char *key, union sz_tag *val);

static const struct entity_traits drop_traits = {
	.name = "drop",
	.update_func = drop_update,
	.render_func = drop_render,
	.destroy_func = drop_destroy,
	.save_func = drop_save,
	.load_func = drop_load,
};

void drop_callback(struct body *b, void *udata, struct v3ll p, int face)
{
	struct drop *d = udata;
	if (face != FACE_UP)
		return;
	if (d->item.num) {
		int s = world_get_shape(d->ctx->w, p);
		int m = world_get_mat(d->ctx->w, p);
		if (block_traits[m][s].capacity > 0) {
			struct array *inv = world_get_data(d->ctx->w, p);
			if (inv == NULL)
				log_warning("expected inventory");
			else
				d->item.num -= inventory_add(inv, d->item);
		}
	}
	b->v.x *= .8;
	b->v.z *= .8;
}

struct drop *drop(struct context *ctx, struct item item)
{
	struct drop *d = drop_create(ctx);
	d->item = item;
	return d;
}

void *drop_create(struct context *ctx)
{
	struct drop *d = calloc(1, sizeof(*d));
	d->entity.traits = &drop_traits;
	list_link(&ctx->entities, &d->entity.entities);
	d->entity.body = body(ctx->space);
	body_set_size(d->entity.body, v2f(.0625, .0625));
	body_set_callback(d->entity.body, drop_callback, d);
	d->ctx = ctx;
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
		d->item.num -= inventory_add(d->ctx->inv, d->item);
	if (d->ticks > 1800 || d->item.num == 0)
		d->entity.die = 1;
}

void drop_render(void *data)
{
	struct drop *d = data;
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(d->entity.body->p.x, d->entity.body->p.y, d->entity.body->p.z);
	glScalef(.25, .25, .25);
	render_obj(d->ctx, d->item.obj, d->item.mat, 255);
	glPopMatrix();
}

int drop_save(void *data, union sz_tag *root)
{/*
	sz_dict_add(root, "y", sz_i64(s->y));
	sz_dict_add(root, "mat", sz_raw(s->mat, SHARD_VOLUME));
	sz_dict_add(root, "shape", sz_raw(s->shape, SHARD_VOLUME));
	sz_dict_add(root, "light", sz_raw(s->light, SHARD_VOLUME));*/
	return 0;
}

int drop_load(void *data, char *key, union sz_tag *val)
{/*
	char *key;
	union sz_tag *val;
	sz_dict_foreach(key, val, root) {
		if (strcmp(key, "y") == 0) {
			s->y = val->i64.data;
		} else if (strcmp(key, "mat") == 0) {
			memcpy(s->mat, val->raw.data, val->raw.size);
		} else if (strcmp(key, "shape") == 0) {
			memcpy(s->shape, val->raw.data, val->raw.size);
		} else if (strcmp(key, "light") == 0) {
			memcpy(s->light, val->raw.data, val->raw.size);
		} else {
			log_error("bad tag: %s", key);
		}
	}*/
	return 0;
}
