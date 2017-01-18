/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "board_entity.h"

#include "context.h"

BoardEntity::BoardEntity(Context *ctx) : BlockEntity(ctx, 64)
{
}

BoardEntity::~BoardEntity()
{
}

void BoardEntity::update()
{
}

void BoardEntity::render()
{
	ctx->renderer->render_inventory(items, p);
}

void BoardEntity::load(sz_Tag *tag)
{
	BlockEntity::load(tag);
}

sz_Tag *BoardEntity::save()
{
	return BlockEntity::save();
}

bool BoardEntity::use(PlayerEntity *player)
{
	if (player->get_cur().face == FACE_UP) {
		player->use_inventory(&items);
		return true;
	}
	return false;
}

