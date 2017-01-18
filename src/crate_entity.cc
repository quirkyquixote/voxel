/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "crate_entity.h"

#include "context.h"

CrateEntity::CrateEntity(Context *ctx) : BlockEntity(ctx, 16)
{
}

CrateEntity::~CrateEntity()
{
}

void CrateEntity::update()
{
}

void CrateEntity::render()
{
	ctx->render_inventory(items, p);
}

void CrateEntity::load(sz_Tag *tag)
{
	BlockEntity::load(tag);
}

sz_Tag *CrateEntity::save()
{
	return BlockEntity::save();
}

bool CrateEntity::use()
{
	if (ctx->cur.face == FACE_UP) {
		ctx->use_inventory(&items);
		return true;
	}
	return false;
}

