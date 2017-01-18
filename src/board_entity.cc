/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "board_entity.h"

#include "context.h"
#include "update.h"

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
	ctx->render_inventory(items, p);
}

void BoardEntity::load(sz_Tag *tag)
{
	BlockEntity::load(tag);
}

sz_Tag *BoardEntity::save()
{
	return BlockEntity::save();
}

bool BoardEntity::use()
{
	if (ctx->cur.face == FACE_UP) {
		use_inventory(ctx, &items);
		return true;
	}
	return false;
}

