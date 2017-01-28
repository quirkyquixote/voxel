/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "serializer.h"

int serializer_print(serializer::Tag *tag, int depth)
{
	int i;
	char *key;
	serializer::Tag *val;
	if (tag->get_tag() == serializer::NUL) {
		/* do nothing */
	} else if (tag->get_tag() == serializer::I8) {
		printf("%d", tag->get_i8());
	} else if (tag->get_tag() == serializer::I16) {
		printf("%d", tag->get_i16());
	} else if (tag->get_tag() == serializer::I32) {
		printf("%d", tag->get_i32());
	} else if (tag->get_tag() == serializer::I64) {
		printf("%ld", tag->get_i64());
	} else if (tag->get_tag() == serializer::F32) {
		printf("%g", tag->get_f32());
	} else if (tag->get_tag() == serializer::F64) {
		printf("%lg", tag->get_f64());
	} else if (tag->get_tag() == serializer::STR) {
		printf("%s", tag->get_str());
	} else if (tag->get_tag() == serializer::RAW) {
		printf("<%zd bytes>", tag->get_raw().size());
	} else if (tag->get_tag() == serializer::LIST) {
		putchar('[');
		putchar('\n');
		for (auto &iter : tag->get_list()) {
			for (i = 0; i < depth + 1; ++i)
				putchar('\t');
			serializer_print(iter, depth + 1);
			putchar('\n');
		}
		for (i = 0; i < depth; ++i)
			putchar('\t');
		putchar(']');
	} else if (tag->get_tag() == serializer::DICT) {
		putchar('{');
		putchar('\n');
		for (auto &iter : tag->get_dict()) {
			for (i = 0; i < depth + 1; ++i)
				putchar('\t');
			printf("%s:", iter.first.c_str());
			serializer_print(iter.second, depth + 1);
			putchar('\n');
		}
		for (i = 0; i < depth; ++i)
			putchar('\t');
		putchar('}');
	} else {
		fprintf(stderr, "%s: bad tag %d\n", __func__, tag->get_tag());
		return -1;
	}
	return 0;
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		serializer::Tag *tag = serializer::read(STDIN_FILENO);
		serializer_print(tag, 0);
		delete tag;
		return 0;
	}

	for (int i = 1; i < argc; ++i) {
		int fd = open(argv[i], O_RDONLY | O_NONBLOCK, 0400);
		if (fd < 0) {
			perror(argv[i]);
			return -1;
		}
		serializer::Tag *tag = serializer::read(fd);
		serializer_print(tag, 0);
		delete tag;
	}

	return 0;
}
