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

bool BenchEntity::use(PlayerEntity *player)
{
	if (player->get_cur().face == FACE_UP) {
		player->use_inventory(&items);
		return true;
	} else {
		player->use_workbench(&items);
		return true;
	}
	return false;
}

