
#include "board_entity.h"

#include "render.h"
#include "update.h"

void *board_entity_create(struct context *ctx);
void board_entity_destroy(void *data);
void board_entity_update(void *data);
void board_entity_render(void *data);
int board_entity_save(void *data, union sz_tag *root);
int board_entity_load(void *data, char *key, union sz_tag *val);
int board_entity_use(void *data);

struct entity_traits board_traits = {
	.name = "board",
	.create_func = board_entity_create,
	.update_func = board_entity_update,
	.render_func = board_entity_render,
	.destroy_func = board_entity_destroy,
	.save_func = board_entity_save,
	.load_func = board_entity_load,
	.use_func = board_entity_use,
};

struct board_entity *board_entity(struct context *ctx)
{
	struct board_entity *d = board_entity_create(ctx);
	return d;
}

void *board_entity_create(struct context *ctx)
{
	struct board_entity *d = calloc(1, sizeof(*d));
	d->block.entity.traits = &board_traits;
	d->block.entity.ctx = ctx;
	d->block.entity.items = inventory(64);
	return d;
}

void board_entity_destroy(void *data)
{
	struct board_entity *d = data;
	array_destroy(d->block.entity.items);
	free(d);
}

void board_entity_update(void *data)
{
}

void board_entity_render(void *data)
{
	struct board_entity *e = data;
	render_inventory(e->block.entity.ctx, e->block.entity.items, e->block.p);
}

int board_entity_save(void *data, union sz_tag *root)
{
	struct board_entity *e = data;
	block_entity_save(e, root);
	return 0;
}

int board_entity_load(void *data, char *key, union sz_tag *val)
{
	struct board_entity *e = data;
	if (block_entity_load(e, key, val) == 0)
		return 0;
	return -1;
}

int board_entity_use(void *raw)
{
	struct board_entity *e = raw;
	if (e->block.entity.ctx->cur.face == FACE_UP) {
		use_inventory(e->block.entity.ctx, e->block.entity.items);
		return 1;
	}
	return 0;
}
