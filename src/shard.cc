/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "shard.h"

#include <string.h>
#include <stdio.h>

#include "sz.h"

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
	sz_Tag *tag;

	tag = sz_dict_lookup(root, "y");
	y = tag->get_i64();
	tag = sz_dict_lookup(root, "mat");
	memcpy(mat, tag->get_raw().data(), sizeof(mat));
	tag = sz_dict_lookup(root, "shape");
	memcpy(shape, tag->get_raw().data(), sizeof(shape));
	tag = sz_dict_lookup(root, "light");
	memcpy(light, tag->get_raw().data(), sizeof(light));
}

sz_Tag *Shard::save()
{
	sz_Tag *root = new sz_Dict();
	sz_dict_add(root, "y", new sz_i64(y));
	sz_dict_add(root, "mat", new sz_Raw(mat, VOLUME));
	sz_dict_add(root, "shape", new sz_Raw(shape, VOLUME));
	sz_dict_add(root, "light", new sz_Raw(light, VOLUME));
	return root;
}

