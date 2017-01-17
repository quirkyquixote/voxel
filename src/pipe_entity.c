
#include "pipe_entity.h"

#include "render.h"
#include "update.h"

void *pipe_entity_create(struct context *ctx);
void pipe_entity_destroy(void *data);
void pipe_entity_update(void *data);
void pipe_entity_render(void *data);
int pipe_entity_save(void *data, union sz_tag *root);
int pipe_entity_load(void *data, char *key, union sz_tag *val);
int pipe_entity_use(void *data);

struct entity_traits pipe_traits = {
	.name = "pipe",
	.create_func = pipe_entity_create,
	.update_func = pipe_entity_update,
	.render_func = pipe_entity_render,
	.destroy_func = pipe_entity_destroy,
	.save_func = pipe_entity_save,
	.load_func = pipe_entity_load,
	.use_func = pipe_entity_use,
};

struct pipe_entity *pipe_entity(struct context *ctx)
{
	struct pipe_entity *d = pipe_entity_create(ctx);
	return d;
}

void *pipe_entity_create(struct context *ctx)
{
	struct pipe_entity *d = calloc(1, sizeof(*d));
	d->block.entity.traits = &pipe_traits;
	d->block.entity.ctx = ctx;
	d->items = inventory(9);
	return d;
}

void pipe_entity_destroy(void *data)
{
	struct pipe_entity *d = data;
	array_destroy(d->items);
	free(d);
}

void pipe_entity_update(void *data)
{
}

void pipe_entity_render(void *data)
{
}

int pipe_entity_save(void *data, union sz_tag *root)
{
	struct pipe_entity *e = data;
	block_entity_save(e, root);
	sz_dict_add(root, "items", sz_raw(e->items->data, e->items->elem_size * e->items->size));
	return 0;
}

int pipe_entity_load(void *data, char *key, union sz_tag *val)
{
	struct pipe_entity *e = data;
	if (block_entity_load(e, key, val) == 0)
		return 0;
	if (strcmp(key, "items") == 0) {
		memcpy(e->items->data, val->raw.data, val->raw.size);
		return 0;
	}
	return -1;
}

int pipe_entity_use(void *raw)
{
	return 0;
}
