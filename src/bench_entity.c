
#include "bench_entity.h"

#include "render.h"
#include "update.h"

void *bench_entity_create(struct context *ctx);
void bench_entity_destroy(void *data);
void bench_entity_update(void *data);
void bench_entity_render(void *data);
int bench_entity_save(void *data, union sz_tag *root);
int bench_entity_load(void *data, char *key, union sz_tag *val);
int bench_entity_use(void *data);

struct entity_traits bench_traits = {
	.name = "bench",
	.create_func = bench_entity_create,
	.update_func = bench_entity_update,
	.render_func = bench_entity_render,
	.destroy_func = bench_entity_destroy,
	.save_func = bench_entity_save,
	.load_func = bench_entity_load,
	.use_func = bench_entity_use,
};

struct bench_entity *bench_entity(struct context *ctx)
{
	struct bench_entity *d = bench_entity_create(ctx);
	return d;
}

void *bench_entity_create(struct context *ctx)
{
	struct bench_entity *d = calloc(1, sizeof(*d));
	d->block.entity.traits = &bench_traits;
	d->block.entity.ctx = ctx;
	d->items = inventory(9);
	return d;
}

void bench_entity_destroy(void *data)
{
	struct bench_entity *d = data;
	array_destroy(d->items);
	free(d);
}

void bench_entity_update(void *data)
{
}

void bench_entity_render(void *data)
{
	struct bench_entity *e = data;
	render_inventory(e->block.entity.ctx, e->items, e->block.p);
}

int bench_entity_save(void *data, union sz_tag *root)
{
	struct bench_entity *e = data;
	block_entity_save(e, root);
	sz_dict_add(root, "items", sz_raw(e->items->data, e->items->elem_size * e->items->size));
	return 0;
}

int bench_entity_load(void *data, char *key, union sz_tag *val)
{
	struct bench_entity *e = data;
	if (block_entity_load(e, key, val) == 0)
		return 0;
	if (strcmp(key, "items") == 0) {
		memcpy(e->items->data, val->raw.data, val->raw.size);
		return 0;
	}
	return -1;
}

int bench_entity_use(void *raw)
{
	struct bench_entity *e = raw;
	if (e->block.entity.ctx->cur.face == FACE_UP) {
		use_inventory(e->block.entity.ctx, e->items);
		return 1;
	} else {
		use_workbench(e->block.entity.ctx, e->items);
		return 1;
	}
	return 0;
}
