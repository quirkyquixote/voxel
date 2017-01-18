/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "pipe_entity.h"

#include "update.h"

PipeEntity::PipeEntity(Context *ctx) : BlockEntity(ctx, 9)
{
}

PipeEntity::~PipeEntity()
{
}

void PipeEntity::update()
{
}

void PipeEntity::render()
{
}

void PipeEntity::load(sz_Tag *tag)
{
	BlockEntity::load(tag);
}

sz_Tag *PipeEntity::save()
{
	return BlockEntity::save();
}

bool PipeEntity::use()
{
	return false;
}
