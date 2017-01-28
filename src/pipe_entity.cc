/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "pipe_entity.h"

PipeEntity::PipeEntity(Context *ctx) : BlockEntity(ctx, 9)
{
}

PipeEntity::~PipeEntity()
{
}

void PipeEntity::load(serializer::Tag *tag)
{
	BlockEntity::load(tag);
}

serializer::Tag *PipeEntity::save()
{
	return BlockEntity::save();
}

bool PipeEntity::use(PlayerEntity *player)
{
	return false;
}
