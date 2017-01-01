

#include "sz.h"

#include <string.h>
#include <unistd.h>
#include <stdio.h>

#ifdef NDEBUG
#define PROPAGATE_ERROR() return -1
#else
#define PROPAGATE_ERROR() abort()
#endif

void sz_destroy(union sz_tag *tag)
{
	if (tag->tag == SZ_STR) {
		free(tag->str.data);
	} else if (tag->tag == SZ_RAW) {
		free(tag->raw.data);
	} else if (tag->tag == SZ_LIST) {
		union sz_tag *val;
		sz_list_foreach(val, tag)
			sz_destroy(val);
		free(tag->list.vals);
	} else if (tag->tag == SZ_DICT) {
		char *key;
		union sz_tag *val;
		sz_dict_foreach(key, val, tag) {
			free(key);
			sz_destroy(val);
		}
		free(tag->list.vals);
	}
	free(tag);
}

union sz_tag *sz_dup(union sz_tag *other)
{
	union sz_tag *tag = calloc(1, sizeof(*tag));
	memcpy(tag, other, sizeof(*tag));
	return tag;
}

union sz_tag *sz_null(void)
{
	union sz_tag *tag = calloc(1, sizeof(*tag));
	tag->tag = SZ_NULL;
	return tag;
}

union sz_tag *sz_i8(int8_t data)
{
	union sz_tag *tag = calloc(1, sizeof(*tag));
	tag->tag = SZ_I8;
	tag->i8.data = data;
	return tag;
}

union sz_tag *sz_i16(int16_t data)
{
	union sz_tag *tag = calloc(1, sizeof(*tag));
	tag->tag = SZ_I16;
	tag->i16.data = data;
	return tag;
}

union sz_tag *sz_i32(int32_t data)
{
	union sz_tag *tag = calloc(1, sizeof(*tag));
	tag->tag = SZ_I32;
	tag->i32.data = data;
	return tag;
}

union sz_tag *sz_i64(int64_t data)
{
	union sz_tag *tag = calloc(1, sizeof(*tag));
	tag->tag = SZ_I64;
	tag->i64.data = data;
	return tag;
}

union sz_tag *sz_f32(float data)
{
	union sz_tag *tag = calloc(1, sizeof(*tag));
	tag->tag = SZ_F32;
	tag->f32.data = data;
	return tag;
}

union sz_tag *sz_f64(double data)
{
	union sz_tag *tag = calloc(1, sizeof(*tag));
	tag->tag = SZ_F64;
	tag->f64.data = data;
	return tag;
}

union sz_tag *sz_str(const char *data)
{
	union sz_tag *tag = calloc(1, sizeof(*tag));
	tag->tag = SZ_STR;
	tag->str.size = strlen(data);
	tag->str.data = strdup(data);
	return tag;
}

union sz_tag *sz_raw(const void *data, size_t size)
{
	union sz_tag *tag = calloc(1, sizeof(*tag));
	tag->tag = SZ_RAW;
	tag->raw.size = size;
	tag->raw.data = calloc(1, size);
	memcpy(tag->raw.data, data, size);
	return tag;
}

union sz_tag *sz_list(void)
{
	union sz_tag *tag = calloc(1, sizeof(*tag));
	tag->tag = SZ_LIST;
	return tag;
}

union sz_tag *sz_dict(void)
{
	union sz_tag *tag = calloc(1, sizeof(*tag));
	tag->tag = SZ_DICT;
	return tag;
}


int sz_list_add(union sz_tag *tag, union sz_tag *elem)
{
	if (tag->list.size == tag->list.alloc) {
		if (tag->list.alloc == 0)
			tag->list.alloc = 2;
		else
			tag->list.alloc *= 2;
		tag->list.vals = realloc(tag->list.vals,
				tag->list.alloc * sizeof(*tag->list.vals));
	}
	tag->list.vals[tag->list.size] = elem;
	++tag->list.size;
	return 0;
}

int sz_dict_add(union sz_tag *tag, const char *name, union sz_tag *elem)
{
	if (tag->dict.size == tag->dict.alloc) {
		if (tag->dict.alloc == 0)
			tag->dict.alloc = 2;
		else
			tag->dict.alloc *= 2;
		tag->dict.keys = realloc(tag->dict.keys,
				tag->dict.alloc * sizeof(*tag->dict.keys));
		tag->dict.vals = realloc(tag->dict.vals,
				tag->dict.alloc * sizeof(*tag->dict.vals));
	}
	tag->dict.keys[tag->dict.size] = strdup(name);
	tag->dict.vals[tag->dict.size] = elem;
	++tag->dict.size;
	return 0;
}

int read_bytes(int fd, void *data, size_t nbytes)
{
	int ret;
	ret = read(fd, data, nbytes);
	if (ret < 0) {
		perror(__func__);
		PROPAGATE_ERROR();
	}
	if (ret < nbytes) {
		fprintf(stderr, "%s: read %d of %d bytes", __func__, ret, nbytes);
		PROPAGATE_ERROR();
	}
	return 0;
}

#define read_atom(fd,data) read_bytes(fd, data, sizeof(*(data)))

int read_str(int fd, char **data, int16_t *rlen)
{
	int16_t len;
	if (read_atom(fd, &len) != 0)
		PROPAGATE_ERROR();
	if (rlen != NULL)
		*rlen = len;
	if (len == 0) {
		*data = NULL;
		return 0;
	}
	*data = calloc(len + 1, 1);
	if (read_bytes(fd, *data, len) != 0)
		PROPAGATE_ERROR();
	return 0;
}

int read_raw(int fd, void **data, int32_t *len)
{
	if (read_atom(fd, len) != 0)
		PROPAGATE_ERROR();
	*data = calloc(*len, 1);
	if (read_bytes(fd, *data, *len) != 0)
		PROPAGATE_ERROR();
	return 0;
}

int next_power_of_2(int x)
{
	int y = 2;
	while (y < x)
		y *= 2;
	return y;
}

int read_list(int fd, union sz_tag *root)
{
	int i;
	union sz_tag tag;

	if (read_atom(fd, &root->list.size) != 0)
		PROPAGATE_ERROR();
	root->dict.alloc = next_power_of_2(root->dict.size);
	root->list.vals = calloc(root->list.alloc, sizeof(*root->list.vals));
	for (i = 0; i < root->list.size; ++i) {
		if (sz_read(fd, &tag) != 0)
			PROPAGATE_ERROR();
		root->list.vals[i] = sz_dup(&tag);
	}
	return 0;
}

int read_dict(int fd, union sz_tag *root)
{
	int i;
	char *key;
	union sz_tag tag;

	if (read_atom(fd, &root->dict.size) != 0)
		PROPAGATE_ERROR();
	root->dict.alloc = next_power_of_2(root->dict.size);
	root->dict.keys = calloc(root->dict.alloc, sizeof(*root->dict.keys));
	root->dict.vals = calloc(root->dict.alloc, sizeof(*root->dict.vals));
	for (i = 0; i < root->dict.size; ++i) {
		if (read_str(fd, &key, NULL) != 0)
			PROPAGATE_ERROR();
		if (sz_read(fd, &tag) != 0)
			PROPAGATE_ERROR();
		root->dict.keys[i] = key;
		root->dict.vals[i] = sz_dup(&tag);
	}
	return 0;
}

int sz_read(int fd, union sz_tag *tag)
{
	if (read_atom(fd, &tag->tag) != 0)
		PROPAGATE_ERROR();
	if (tag->tag == SZ_NULL) {
		return 0;
	} else if (tag->tag == SZ_I8) {
		return read_atom(fd, &tag->i8.data);
	} else if (tag->tag == SZ_I16) {
		return read_atom(fd, &tag->i16.data);
	} else if (tag->tag == SZ_I32) {
		return read_atom(fd, &tag->i32.data);
	} else if (tag->tag == SZ_I64) {
		return read_atom(fd, &tag->i64.data);
	} else if (tag->tag == SZ_F32) {
		return read_atom(fd, &tag->f32.data);
	} else if (tag->tag == SZ_F64) {
		return read_atom(fd, &tag->f64.data);
	} else if (tag->tag == SZ_STR) {
		return read_str(fd, &tag->str.data, &tag->str.size);
	} else if (tag->tag == SZ_RAW) {
		return read_raw(fd, &tag->raw.data, &tag->raw.size);
	} else if (tag->tag == SZ_LIST) {
		return read_list(fd, tag);
	} else if (tag->tag == SZ_DICT) {
		return read_dict(fd, tag);
	}
	fprintf(stderr, "%s: bad tag %02x\n", __func__, tag->tag);
	PROPAGATE_ERROR();
}

int write_bytes(int fd, void *data, size_t nbytes)
{
	int ret;
	ret = write(fd, data, nbytes);
	if (ret < 0) {
		perror(__func__);
		PROPAGATE_ERROR();
	}
	if (ret < nbytes) {
		fprintf(stderr, "%s: write %d of %d bytes", __func__, ret, nbytes);
		PROPAGATE_ERROR();
	}
	return 0;
}

#define write_atom(fd,data) write_bytes(fd, &data, sizeof(data))

int write_str(int fd, char *data, int16_t len)
{
	if (write_atom(fd, len) != 0)
		PROPAGATE_ERROR();
	if (write_bytes(fd, data, len) != 0)
		PROPAGATE_ERROR();
	return 0;
}

int write_raw(int fd, void *data, int32_t len)
{
	if (write_atom(fd, len) != 0)
		PROPAGATE_ERROR();
	if (write_bytes(fd, data, len) != 0)
		PROPAGATE_ERROR();
	return 0;
}

int write_list(int fd, union sz_tag *root)
{
	union sz_tag *iter;
	if (write_atom(fd, root->list.size) != 0)
		PROPAGATE_ERROR();
	sz_list_foreach(iter, root) {
		if (sz_write(fd, iter) != 0)
			PROPAGATE_ERROR();
	}
	return 0;
}

int write_dict(int fd, union sz_tag *root)
{
	char *key;
	union sz_tag *val;
	if (write_atom(fd, root->dict.size) != 0)
		PROPAGATE_ERROR();
	sz_dict_foreach(key, val, root) {
		if (write_str(fd, key, strlen(key)) != 0)
			PROPAGATE_ERROR();
		if (sz_write(fd, val) != 0)
			PROPAGATE_ERROR();
	}
	return 0;
}

int sz_write(int fd, union sz_tag *tag)
{
	if (write_atom(fd, tag->tag) != 0)
		PROPAGATE_ERROR();
	if (tag->tag == SZ_NULL) {
		return 0;
	} else if (tag->tag == SZ_I8) {
		return write_atom(fd, tag->i8.data);
	} else if (tag->tag == SZ_I16) {
		return write_atom(fd, tag->i16.data);
	} else if (tag->tag == SZ_I32) {
		return write_atom(fd, tag->i32.data);
	} else if (tag->tag == SZ_I64) {
		return write_atom(fd, tag->i64.data);
	} else if (tag->tag == SZ_F32) {
		return write_atom(fd, tag->f32.data);
	} else if (tag->tag == SZ_F64) {
		return write_atom(fd, tag->f64.data);
	} else if (tag->tag == SZ_STR) {
		return write_str(fd, tag->str.data, tag->str.size);
	} else if (tag->tag == SZ_RAW) {
		return write_raw(fd, tag->raw.data, tag->raw.size);
	} else if (tag->tag == SZ_LIST) {
		return write_list(fd, tag);
	} else if (tag->tag == SZ_DICT) {
		return write_dict(fd, tag);
	}
	fprintf(stderr, "%s: bad tag %02x\n", __func__, tag->tag);
	PROPAGATE_ERROR();
}
