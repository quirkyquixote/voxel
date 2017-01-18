/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "bench_entity.h"

#include "render.h"
#include "update.h"

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
	render_inventory(ctx, items, p);
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
		use_inventory(ctx, &items);
		return true;
	} else {
		use_workbench(ctx, &items);
		return true;
	}
	return false;
}

