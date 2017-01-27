/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "shard.h"

#include <string.h>
#include <stdio.h>

#include "serializer.h"

Shard::Shard(int id, int y)
	: id(id), y(y)
{
	memset(data, 0, sizeof(data));
}

Shard::~Shard()
{
}

void Shard::load(serializer::Tag *root)
{
	memset(data, 0, sizeof(data));
	serializer::Tag *tag;

	tag = serializer::dict_lookup(root, "y");
	y = tag->get_i64();
	tag = serializer::dict_lookup(root, "mat");
	memcpy(mat, tag->get_raw().data(), sizeof(mat));
	tag = serializer::dict_lookup(root, "shape");
	memcpy(shape, tag->get_raw().data(), sizeof(shape));
	tag = serializer::dict_lookup(root, "light");
	memcpy(light, tag->get_raw().data(), sizeof(light));
}

serializer::Tag *Shard::save()
{
	serializer::Tag *root = new serializer::Dict();
	serializer::dict_add(root, "y", new serializer::i64(y));
	serializer::dict_add(root, "mat", new serializer::Raw(mat, VOLUME));
	serializer::dict_add(root, "shape", new serializer::Raw(shape, VOLUME));
	serializer::dict_add(root, "light", new serializer::Raw(light, VOLUME));
	return root;
}

