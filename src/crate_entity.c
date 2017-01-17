
#include "crate_entity.h"

#include "render.h"
#include "update.h"

void *crate_entity_create(struct context *ctx);
void crate_entity_destroy(void *data);
void crate_entity_update(void *data);
void crate_entity_render(void *data);
int crate_entity_save(void *data, union sz_tag *root);
int crate_entity_load(void *data, char *key, union sz_tag *val);
int crate_entity_use(void *data);

struct entity_traits crate_traits = {
	.name = "crate",
	.create_func = crate_entity_create,
	.update_func = crate_entity_update,
	.render_func = crate_entity_render,
	.destroy_func = crate_entity_destroy,
	.save_func = crate_entity_save,
	.load_func = crate_entity_load,
	.use_func = crate_entity_use,
};

struct crate_entity *crate_entity(struct context *ctx)
{
	struct crate_entity *d = crate_entity_create(ctx);
	return d;
}

void *crate_entity_create(struct context *ctx)
{
	struct crate_entity *d = calloc(1, sizeof(*d));
	d->block.entity.traits = &crate_traits;
	d->block.entity.ctx = ctx;
	d->items = inventory(16);
	return d;
}

void crate_entity_destroy(void *data)
{
	struct crate_entity *d = data;
	array_destroy(d->items);
	free(d);
}

void crate_entity_update(void *data)
{
}

void crate_entity_render(void *data)
{
	struct crate_entity *e = data;
	render_inventory(e->block.entity.ctx, e->items, e->block.p);
}

int crate_entity_save(void *data, union sz_tag *root)
{
	struct crate_entity *e = data;
	block_entity_save(e, root);
	sz_dict_add(root, "items", sz_raw(e->items->data, e->items->elem_size * e->items->size));
	return 0;
}

int crate_entity_load(void *data, char *key, union sz_tag *val)
{
	struct crate_entity *e = data;
	if (block_entity_load(e, key, val) == 0)
		return 0;
	if (strcmp(key, "items") == 0) {
		memcpy(e->items->data, val->raw.data, val->raw.size);
		return 0;
	}
	return -1;
}

int crate_entity_use(void *raw)
{
	struct crate_entity *e = raw;
	if (e->block.entity.ctx->cur.face == FACE_UP) {
		update_inventory(e->block.entity.ctx, e->items);
		return 1;
	}
	return 0;
}
