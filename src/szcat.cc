/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "sz.h"

int sz_print(sz_Tag *tag, int depth)
{
	int i;
	char *key;
	union sz_tag *val;
	if (tag->get_tag() == SZ_NULL) {
		/* do nothing */
	} else if (tag->get_tag() == SZ_I8) {
		printf("%d", tag->get_i8());
	} else if (tag->get_tag() == SZ_I16) {
		printf("%d", tag->get_i16());
	} else if (tag->get_tag() == SZ_I32) {
		printf("%d", tag->get_i32());
	} else if (tag->get_tag() == SZ_I64) {
		printf("%ld", tag->get_i64());
	} else if (tag->get_tag() == SZ_F32) {
		printf("%g", tag->get_f32());
	} else if (tag->get_tag() == SZ_F64) {
		printf("%lg", tag->get_f64());
	} else if (tag->get_tag() == SZ_STR) {
		printf("%s", tag->get_str());
	} else if (tag->get_tag() == SZ_RAW) {
		printf("<%zd bytes>", tag->get_raw().size());
	} else if (tag->get_tag() == SZ_LIST) {
		putchar('[');
		putchar('\n');
		for (auto &iter : tag->get_list()) {
			for (i = 0; i < depth + 1; ++i)
				putchar('\t');
			sz_print(iter, depth + 1);
			putchar('\n');
		}
		for (i = 0; i < depth; ++i)
			putchar('\t');
		putchar(']');
	} else if (tag->get_tag() == SZ_DICT) {
		putchar('{');
		putchar('\n');
		for (auto &iter : tag->get_dict()) {
			for (i = 0; i < depth + 1; ++i)
				putchar('\t');
			printf("%s:", iter.first);
			sz_print(iter.second, depth + 1);
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
		sz_Tag *tag = sz_read(STDIN_FILENO);
		sz_print(tag, 0);
		delete tag;
		return 0;
	}

	for (int i = 1; i < argc; ++i) {
		int fd = open(argv[i], O_RDONLY | O_NONBLOCK, 0400);
		if (fd < 0) {
			perror(argv[i]);
			return -1;
		}
		sz_Tag *tag = sz_read(fd);
		sz_print(tag, 0);
		delete tag;
	}

	return 0;
}
