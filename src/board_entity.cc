/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "board_entity.h"

#include "context.h"

enum {
	FLAG_EXISTS = 1 << 0,
	FLAG_ON = 1 << 1,
	FLAG_LF = 1 << 2,
	FLAG_RT = 1 << 3,
	FLAG_BK = 1 << 4,
	FLAG_FT = 1 << 5,
};

BoardEntity::BoardEntity(Context *ctx) : BlockEntity(ctx, 64)
{
	update_func.reset(new Callback([ctx,this](){
		this->update();
	}));
	render_func.reset(new Callback([ctx,this](){
		ctx->renderer->render_inventory(this->items, this->p);
		ctx->renderer->render_inventory_state(this->items, this->p);
	}));
	ctx->add_callback(update_func.get());
	ctx->renderer->add_callback(render_func.get());
}

BoardEntity::~BoardEntity()
{
}

void BoardEntity::test(int i, int min, int max)
{
	int acc = 0;
	if (i >= 8) {
		int j = i - 8;
		if ((items[j].num & FLAG_ON) != 0
				&& (items[j].num & FLAG_RT) == 0) {
			tmp[i] |= FLAG_LF;
			++acc;
		}
	}
	if (i < 64 - 8) {
		int j = i + 8;
		if ((items[j].num & FLAG_ON) != 0
				&& (items[j].num & FLAG_LF) == 0) {
			tmp[i] |= FLAG_RT;
			++acc;
		}
	}
	if (i > 0) {
		int j = i - 1;
		if ((items[j].num & FLAG_ON) != 0
				&& (items[j].num & FLAG_FT) == 0) {
			tmp[i] |= FLAG_BK;
			++acc;
		}
	}
	if (i < 64 - 1) {
		int j = i + 1;
		if ((items[j].num & FLAG_ON) != 0
				&& (items[j].num & FLAG_BK) == 0) {
			tmp[i] |= FLAG_FT;
			++acc;
		}
	}
	if (acc >= min && acc <= max) {
		tmp[i] |= FLAG_ON;
	}
}

void BoardEntity::update()
{
	for (int i = 0; i < 64; ++i) {
		if (items[i].num != 0) {
			tmp[i] = FLAG_EXISTS;
			if (items[i].mat == MAT_RUNE_O)
				tmp[i] |= FLAG_ON;
			else if (items[i].mat == MAT_RUNE_K)
				test(i, 2, 4);
			else if (items[i].mat == MAT_RUNE_A)
				test(i, 1, 4);
			else if (items[i].mat == MAT_RUNE_N)
				test(i, 0, 0);
			else if (items[i].mat == MAT_RUNE_J)
				test(i, 1, 1);
		} else {
			tmp[i] = 0;
		}
	}
	for (int i = 0; i < 64; ++i)
		items[i].num = tmp[i];
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

