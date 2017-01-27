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

void Shard::load(sz::Tag *root)
{
	memset(data, 0, sizeof(data));
	sz::Tag *tag;

	tag = sz::dict_lookup(root, "y");
	y = tag->get_i64();
	tag = sz::dict_lookup(root, "mat");
	memcpy(mat, tag->get_raw().data(), sizeof(mat));
	tag = sz::dict_lookup(root, "shape");
	memcpy(shape, tag->get_raw().data(), sizeof(shape));
	tag = sz::dict_lookup(root, "light");
	memcpy(light, tag->get_raw().data(), sizeof(light));
}

sz::Tag *Shard::save()
{
	sz::Tag *root = new sz::Dict();
	sz::dict_add(root, "y", new sz::i64(y));
	sz::dict_add(root, "mat", new sz::Raw(mat, VOLUME));
	sz::dict_add(root, "shape", new sz::Raw(shape, VOLUME));
	sz::dict_add(root, "light", new sz::Raw(light, VOLUME));
	return root;
}

