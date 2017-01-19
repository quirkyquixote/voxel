/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "shard.h"

#include <string.h>
#include <stdio.h>

#include "sz.h"
#include "lighting.h"
#include "inventory.h"
#include "context.h"
#include "block_entity.h"

Shard::Shard(int id, int y)
	: id(id), y(y)
{
	memset(data, 0, sizeof(data));
}

Shard::~Shard()
{
}

void Shard::load(sz_Tag *root)
{
	memset(data, 0, sizeof(data));
	for (auto &it : root->get_dict()) {
		if (strcmp(it.first, "y") == 0) {
			y = it.second->get_i64();
		} else if (strcmp(it.first, "mat") == 0) {
			memcpy(mat, it.second->get_raw().data(), sizeof(mat));
		} else if (strcmp(it.first, "shape") == 0) {
			memcpy(shape, it.second->get_raw().data(), sizeof(shape));
		} else if (strcmp(it.first, "light") == 0) {
			memcpy(light, it.second->get_raw().data(), sizeof(light));
		} else {
			log_error("bad tag: %s", it.first);
		}
	}
}

sz_Tag *Shard::save()
{
	sz_Tag *root = new sz_Dict();
	sz_dict_add(root, "y", new sz_i64(y));
	sz_dict_add(root, "mat", new sz_Raw(mat, SHARD_VOLUME));
	sz_dict_add(root, "shape", new sz_Raw(shape, SHARD_VOLUME));
	sz_dict_add(root, "light", new sz_Raw(light, SHARD_VOLUME));
	return root;
}

