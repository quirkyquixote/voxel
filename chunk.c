
#include "chunk.h"

#include <string.h>
#include <stdio.h>

#include "sz.h"

struct chunk *chunk_new(void)
{
	struct chunk *c = calloc(1, sizeof(*c));
	c->mat = calloc(VX_CHUNK_SIZE, sizeof(*c->mat));
	c->shape = calloc(VX_CHUNK_SIZE, sizeof(*c->shape));
	c->light1 = calloc(VX_CHUNK_SIZE, sizeof(*c->light1));
	c->light2 = calloc(VX_CHUNK_SIZE, sizeof(*c->light2));
	return c;
}

void chunk_destroy(struct chunk *c)
{
	struct cell_entity *it, *next;
	for (it = c->entities; it != NULL; it = next) {
		next = it->next;
		free(it);
	}
	free(c->mat);
	free(c->shape);
	free(c->light1);
	free(c->light2);
	free(c);
}

int chunk_load(struct chunk *c, int fd)
{
	struct sz_tag it;
	int rval;
	for (;;) {
		rval = sz_read(fd, &it);
		if (rval == SZ_END) {
			return 0;
		} else if (rval == SZ_ERROR) {
			return -1;
		} else if (strcmp(it.name, "x") == 0) {
			if (sz_to_i64(&it, &c->x) != 0)
				return -1;
		} else if (strcmp(it.name, "y") == 0) {
			if (sz_to_i64(&it, &c->y) != 0)
				return -1;
		} else if (strcmp(it.name, "z") == 0) {
			if (sz_to_i64(&it, &c->z) != 0)
				return -1;
		} else if (strcmp(it.name, "mat") == 0) {
			if (sz_copy_i8v(&it, c->mat, VX_CHUNK_SIZE) != 0)
				return -1;
		} else if (strcmp(it.name, "shape") == 0) {
			if (sz_copy_i8v(&it, c->shape, VX_CHUNK_SIZE) != 0)
				return -1;
		} else if (strcmp(it.name, "light1") == 0) {
			if (sz_copy_i8v(&it, c->light1, VX_CHUNK_SIZE) != 0)
				return -1;
		} else if (strcmp(it.name, "light2") == 0) {
			if (sz_copy_i8v(&it, c->light2, VX_CHUNK_SIZE) != 0)
				return -1;
		} else {
			fprintf(stderr, "bad tag: %s\n", it.name);
			return -1;
		}
	}
	return 0;
}

int chunk_save(struct chunk *c, int fd)
{
	if (sz_write_i64(fd, "x", c->x))
		return -1;
	if (sz_write_i64(fd, "y", c->y))
		return -1;
	if (sz_write_i64(fd, "z", c->z))
		return -1;
	if (sz_write_i8v(fd, "mat", c->mat, VX_CHUNK_SIZE))
		return -1;
	if (sz_write_i8v(fd, "shape", c->shape, VX_CHUNK_SIZE))
		return -1;
	if (sz_write_i8v(fd, "light1", c->light1, VX_CHUNK_SIZE))
		return -1;
	if (sz_write_i8v(fd, "light2", c->light2, VX_CHUNK_SIZE))
		return -1;
	return 0;
}

