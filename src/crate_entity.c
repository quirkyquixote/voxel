
#include "crate_entity.h"

#include "render.h"

void *crate_entity_create(struct context *ctx);
void crate_entity_destroy(void *data);
void crate_entity_update(void *data);
void crate_entity_render(void *data);
int crate_entity_save(void *data, union sz_tag *root);
int crate_entity_load(void *data, char *key, union sz_tag *val);
void crate_entity_use(void *raw, struct context *ctx);

static const struct entity_traits crate_entity_traits = {
	.name = "crate",
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

void crate_entity_use(void *raw, struct context *ctx)
{
	struct crate_entity *e = raw;
	struct v3ll p = ctx->cur.p;
	struct v3f q = ctx->cur.q;
	int side = sqrt(e->items->size);
	int i = side * floor(q.x * side) + floor(q.z * side);
	struct item s1 = inventory_get(ctx->inv, ctx->tool);
	struct item s2 = inventory_get(e->items, i);
	if (ctx->act == 1) {
		if (ctx->move.y0) {
			if (s2.num == 0) {
				log_info("nothing to take");
				return;
			}
			int acc = inventory_add(ctx->inv, s2);
			inventory_set_num(e->items, i, s2.num - acc);
			if (acc == 0)
				log_info("no space to take");
			else
				log_info("take %s %s %d", mat_names[s1.mat],
						obj_names[s1.obj], acc);
		} else {
			if (s2.num)
				log_info("take %s %s %d", mat_names[s2.mat],
						obj_names[s2.obj], s2.num);
			if (s1.num)
				log_info("left %s %s %d", mat_names[s1.mat],
						obj_names[s1.obj], s1.num);
			inventory_set(e->items, i, s1);
			inventory_set(ctx->inv, ctx->tool, s2);
		}
	} else if (ctx->use == 1) {
		if (ctx->move.y0) {
			if (s2.num == 0) {
				log_info("nothing to take");
				return;
			}
			int acc =
				inventory_add(ctx->inv, item(s2.obj, s2.mat, 1));
			inventory_set_num(e->items, i, s2.num - acc);
			if (acc == 0)
				log_info("no space to take");
			else
				log_info("take %s %s 1", mat_names[s1.mat],
						obj_names[s1.obj], acc);
		} else {
			if (s1.num == 0) {
				log_info("nothing to leave");
				return;
			}
			if (s2.num >= 64) {
				log_info("no space to leave");
				return;
			}
			if (s2.num == 0) {
				inventory_set_obj(e->items, i, s1.obj);
				inventory_set_mat(e->items, i, s1.mat);
			} else if (s1.obj != s2.obj || s1.mat != s2.mat) {
				log_info("not the same object");
				return;
			}
			inventory_set_num(e->items, i, s2.num + 1);
			inventory_set_num(ctx->inv, ctx->tool, s1.num - 1);
			log_info("left %s %s 1", mat_names[s1.mat],
					obj_names[s1.obj]);
		}
	}
}
