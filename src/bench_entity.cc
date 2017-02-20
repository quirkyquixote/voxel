/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "bench_entity.h"

#include "context.h"

BenchEntity::BenchEntity(Context *ctx) : BlockEntity(ctx, 9)
{
	render_callback.reset(new Callback([ctx, this](){
		ctx->renderer->render_inventory(this->items, this->p);
	}));
	ctx->renderer->add_callback(render_callback.get());
}

BenchEntity::~BenchEntity()
{
}

void BenchEntity::load(serializer::Tag *tag)
{
	BlockEntity::load(tag);
}

serializer::Tag *BenchEntity::save()
{
	return BlockEntity::save();
}

bool BenchEntity::use(PlayerEntity *player)
{
	if (player->get_cur().face == FACE_UP) {
		player->use_workbench(&items);
		return true;
	}
	return false;
}

