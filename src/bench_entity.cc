/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "bench_entity.h"

#include "context.h"

BenchEntity::BenchEntity(Context *ctx) : BlockEntity(ctx, 9)
{
}

BenchEntity::~BenchEntity()
{
}

void BenchEntity::update()
{
}

void BenchEntity::render()
{
	ctx->renderer->render_inventory(items, p);
}

void BenchEntity::load(sz_Tag *tag)
{
	BlockEntity::load(tag);
}

sz_Tag *BenchEntity::save()
{
	return BlockEntity::save();
}

bool BenchEntity::use()
{
	if (ctx->cur.face == FACE_UP) {
		ctx->use_inventory(&items);
		return true;
	} else {
		ctx->use_workbench(&items);
		return true;
	}
	return false;
}

