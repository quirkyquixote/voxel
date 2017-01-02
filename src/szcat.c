
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "sz.h"

int sz_print(union sz_tag *tag, int depth)
{
	int i;
	char *key;
	union sz_tag *val;
	if (tag->tag == SZ_NULL) {
		/* do nothing */
	} else if (tag->tag == SZ_I8) {
		printf("%d", tag->i8.data);
	} else if (tag->tag == SZ_I16) {
		printf("%d", tag->i16.data);
	} else if (tag->tag == SZ_I32) {
		printf("%d", tag->i32.data);
	} else if (tag->tag == SZ_I64) {
		printf("%ld", tag->i64.data);
	} else if (tag->tag == SZ_F32) {
		printf("%g", tag->f32.data);
	} else if (tag->tag == SZ_F64) {
		printf("%lg", tag->i64.data);
	} else if (tag->tag == SZ_STR) {
		printf("%s", tag->str.data);
	} else if (tag->tag == SZ_RAW) {
		printf("<%d bytes>", tag->raw.size);
	} else if (tag->tag == SZ_LIST) {
		putchar('[');
		putchar('\n');
		sz_list_foreach(val, tag) {
			for (i = 0; i < depth + 1; ++i)
				putchar('\t');
			sz_print(val, depth + 1);
			putchar('\n');
		}
		for (i = 0; i < depth; ++i)
			putchar('\t');
		putchar(']');
	} else if (tag->tag == SZ_DICT) {
		putchar('{');
		putchar('\n');
		sz_dict_foreach(key, val, tag) {
			for (i = 0; i < depth + 1; ++i)
				putchar('\t');
			printf("%s:", key);
			sz_print(val, depth + 1);
			putchar('\n');
		}
		for (i = 0; i < depth; ++i)
			putchar('\t');
		putchar('}');
	} else {
		fprintf(stderr, "%s: bad tag %d\n", __func__, tag->tag);
		return -1;
	}
	return 0;
}

int main(int argc, char *argv[])
{
	union sz_tag tag;
	int i;
	int fd;

	if (argc < 2) {
		if (sz_read(STDIN_FILENO, &tag) != 0)
			return -1;
		sz_print(&tag, 0);
		return 0;
	}

	for (i = 1; i < argc; ++i) {
		fd = open(argv[i], O_RDONLY | O_NONBLOCK, 0400);
		if (fd < 0) {
			perror(argv[i]);
			return -1;
		}
		if (sz_read(fd, &tag) != 0)
			return -1;
		sz_print(&tag, 0);
	}

	return 0;
}
