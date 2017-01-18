
#include "drop_entity.h"

#include "render.h"
#include "block_entity.h"

void DropEntity::callback(Body *b, const v3ll &p, int face)
{
	if (face != FACE_UP)
		return;
/*
	if (d->get_items()[0].num) {
		Entity *e = 
		struct block_entity *e = world_get_data(entity.ctx->w, p);
		if (e != NULL && e->entity.items != NULL)
			item.num -= inventory_add(e->entity.items, item);
	}*/
	b->set_v(b->get_v() * .8f);
}

DropEntity::DropEntity(Context *ctx, Item item)
	: RoamingEntity(ctx, 1)
{
	body->set_size(v2f(.0625, .0625));
	body->set_callback([this](Body *b, const v3ll &p, int face){this->callback(b, p, face);});
	items[0] = item;
}

DropEntity::~DropEntity()
{
}

void DropEntity::update()
{
	++ticks;
	if (ticks > 10 && overlap(grow(body->get_box(), 1.f), ctx->player->get_box()))
		items[0].num -= inventory_add(&ctx->inv, items[0]);
	if (ticks > 1800 || items[0].num == 0)
		die = true;
}

void DropEntity::render()
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(body->get_p().x, body->get_p().y, body->get_p().z);
	glScalef(.25, .25, .25);
	render_obj(ctx, items[0].obj, items[0].mat, 255);
	glPopMatrix();
}

sz_Tag *DropEntity::save()
{
	return RoamingEntity::save();
}

void DropEntity::load(sz_Tag *root)
{
	RoamingEntity::load(root);
}
