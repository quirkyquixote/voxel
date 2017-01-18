
#include "pipe_entity.h"

#include "render.h"
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
	render_inventory(ctx, items, p);
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
	if (ctx->cur.face == FACE_UP) {
		use_inventory(ctx, &items);
		return true;
	}
	return false;
}
