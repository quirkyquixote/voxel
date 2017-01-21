/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "board_entity.h"

#include "context.h"

BoardEntity::BoardEntity(Context *ctx) : BlockEntity(ctx, 64)
{
	update_func.reset(new Callback([ctx,this](){
		this->update();
	}));
	render_func.reset(new Callback([ctx,this](){
		ctx->renderer->render_board(this->items, this->p);
	}));
	ctx->add_callback(update_func.get());
	ctx->renderer->add_callback(render_func.get());
}

BoardEntity::~BoardEntity()
{
}

void BoardEntity::test(int i, int min, int max)
{
}

void BoardEntity::update()
{
	for (int i = 0; i < 64; ++i)
		tmp[i] = 0;

	for (int i = 0; i < 64; ++i) {
		if (items[i].num & BOARD_ON) {
			if (i >= 8 && items[i].obj == OBJ_TOKEN_LF)
				++tmp[i - 8];
			if (i < 64 - 8 && items[i].obj == OBJ_TOKEN_RT)
				++tmp[i + 8];
			if (i > 0 && items[i].obj == OBJ_TOKEN_BK)
				++tmp[i - 1];
			if (i < 64 - 1 && items[i].obj == OBJ_TOKEN_FT)
				++tmp[i + 1];
		}
	}

	for (int i = 0; i < 64; ++i) {
		if (items[i].num != 0) {
			if (items[i].mat == MAT_RUNE_O) {
				items[i].num |= BOARD_ON;
			} else if (items[i].mat == MAT_RUNE_K) {
				if (tmp[i] >= 2 && tmp[i] <= 4)
					items[i].num |= BOARD_ON;
				else
					items[i].num &= ~BOARD_ON;
			} else if (items[i].mat == MAT_RUNE_A) {
				if (tmp[i] >= 1 && tmp[i] <= 4)
					items[i].num |= BOARD_ON;
				else
					items[i].num &= ~BOARD_ON;
			} else if (items[i].mat == MAT_RUNE_N) {
				if (tmp[i] == 0)
					items[i].num |= BOARD_ON;
				else
					items[i].num &= ~BOARD_ON;
			} else if (items[i].mat == MAT_RUNE_J) {
				if (tmp[i] == 1)
					items[i].num |= BOARD_ON;
				else
					items[i].num &= ~BOARD_ON;
			}
		}
	}
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
		player->use_board(&items);
		return true;
	}
	return false;
}

