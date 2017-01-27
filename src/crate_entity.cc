/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "crate_entity.h"

#include "context.h"

CrateEntity::CrateEntity(Context *ctx) : BlockEntity(ctx, 16)
{
	render_callback.reset(new Callback([ctx, this](){
		ctx->renderer->render_inventory(this->items, this->p);
	}));
	ctx->renderer->add_callback(render_callback.get());
}

CrateEntity::~CrateEntity()
{
}


void CrateEntity::load(sz::Tag *tag)
{
	BlockEntity::load(tag);
}

sz::Tag *CrateEntity::save()
{
	return BlockEntity::save();
}

bool CrateEntity::use(PlayerEntity *player)
{
	if (player->get_cur().face == FACE_UP) {
		player->use_inventory(&items);
		return true;
	}
	return false;
}

