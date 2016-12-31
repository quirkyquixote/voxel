

#include "sz.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

int write_data(int fd, const void *data, size_t len)
{
	int rval = write(fd, data, len);
	if (rval < 0) {
		perror("write_data");
		return -1;
	}
	if (rval < len) {
		fprintf(stderr, "write_data: %d of %d bytes written\n", rval, len);
		return -1;
	}
	return 0;
}

int write_i8(int fd, int8_t data)
{
	return write_data(fd, &data, sizeof(data));
}

int write_i16(int fd, int16_t data)
{
	return write_data(fd, &data, sizeof(data));
}

int write_i32(int fd, int32_t data)
{
	return write_data(fd, &data, sizeof(data));
}

int write_i64(int fd, int32_t data)
{
	return write_data(fd, &data, sizeof(data));
}

int write_f32(int fd, float data)
{
	return write_data(fd, &data, sizeof(data));
}

int write_f64(int fd, double data)
{
	return write_data(fd, &data, sizeof(data));
}

int write_i8v(int fd, const int8_t* data, size_t len)
{
	if (write_i16(fd, len) != 0)
		return -1;
	if (write_data(fd, data, sizeof(*data) * len) != 0)
		return -1;
	return 0;
}

int write_i16v(int fd, const int16_t* data, size_t len)
{
	if (write_i16(fd, len) != 0)
		return -1;
	if (write_data(fd, data, sizeof(*data) * len) != 0)
		return -1;
	return 0;
}

int write_i32v(int fd, const int32_t* data, size_t len)
{
	if (write_i16(fd, len) != 0)
		return -1;
	if (write_data(fd, data, sizeof(*data) * len) != 0)
		return -1;
	return 0;
}

int write_i64v(int fd, const int64_t* data, size_t len)
{
	if (write_i16(fd, len) != 0)
		return -1;
	if (write_data(fd, data, sizeof(*data) * len) != 0)
		return -1;
	return 0;
}

int write_f32v(int fd, const float* data, size_t len)
{
	if (write_i16(fd, len) != 0)
		return -1;
	if (write_data(fd, data, sizeof(*data) * len) != 0)
		return -1;
	return 0;
}

int write_f64v(int fd, const double* data, size_t len)
{
	if (write_i16(fd, len) != 0)
		return -1;
	if (write_data(fd, data, sizeof(*data) * len) != 0)
		return -1;
	return 0;
}

int sz_write_null(int fd, const char *name)
{
	if (write_i8(fd, SZ_NULL) != 0)
		return -1;
	if (write_i8v(fd, name, strlen(name)) != 0)
		return -1;
	return 0;
}

int sz_write_i8(int fd, const char *name, int8_t data)
{
	if (write_i8(fd, SZ_I8) != 0)
		return -1;
	if (write_i8v(fd, name, strlen(name)) != 0)
		return -1;
	if (write_i8(fd, data) != 0)
		return -1;
	return 0;
}

int sz_write_i16(int fd, const char *name, int16_t data)
{
	if (write_i8(fd, SZ_I16) != 0)
		return -1;
	if (write_i8v(fd, name, strlen(name)) != 0)
		return -1;
	if (write_i16(fd, data) != 0)
		return -1;
	return 0;
}

int sz_write_i32(int fd, const char *name, int32_t data)
{
	if (write_i8(fd, SZ_I32) != 0)
		return -1;
	if (write_i8v(fd, name, strlen(name)) != 0)
		return -1;
	if (write_i32(fd, data) != 0)
		return -1;
	return 0;
}

int sz_write_i64(int fd, const char *name, int64_t data)
{
	if (write_i8(fd, SZ_I64) != 0)
		return -1;
	if (write_i8v(fd, name, strlen(name)) != 0)
		return -1;
	if (write_i64(fd, data) != 0)
		return -1;
	return 0;
}

int sz_write_f32(int fd, const char *name, float data)
{
	if (write_i8(fd, SZ_F32) != 0)
		return -1;
	if (write_i8v(fd, name, strlen(name)) != 0)
		return -1;
	if (write_i32(fd, data) != 0)
		return -1;
	return 0;
}

int sz_write_f64(int fd, const char *name, double data)
{
	if (write_i8(fd, SZ_F64) != 0)
		return -1;
	if (write_i8v(fd, name, strlen(name)) != 0)
		return -1;
	if (write_f64(fd, data) != 0)
		return -1;
	return 0;
}

int sz_write_i8v(int fd, const char *name, int8_t *data, size_t len)
{
	if (write_i8(fd, SZ_I8) != 0)
		return -1;
	if (write_i8v(fd, name, strlen(name)) != 0)
		return -1;
	if (write_i8v(fd, data, len) != 0)
		return -1;
	return 0;
}

int sz_write_i16v(int fd, const char *name, int16_t *data, size_t len)
{
	if (write_i8(fd, SZ_I16) != 0)
		return -1;
	if (write_i8v(fd, name, strlen(name)) != 0)
		return -1;
	if (write_i16v(fd, data, len) != 0)
		return -1;
	return 0;
}

int sz_write_i32v(int fd, const char *name, int32_t *data, size_t len)
{
	if (write_i8(fd, SZ_I32) != 0)
		return -1;
	if (write_i8v(fd, name, strlen(name)) != 0)
		return -1;
	if (write_i32v(fd, data, len) != 0)
		return -1;
	return 0;
}

int sz_write_i64v(int fd, const char *name, int64_t *data, size_t len)
{
	if (write_i8(fd, SZ_I64) != 0)
		return -1;
	if (write_i8v(fd, name, strlen(name)) != 0)
		return -1;
	if (write_i64v(fd, data, len) != 0)
		return -1;
	return 0;
}

int sz_write_f32v(int fd, const char *name, float *data, size_t len)
{
	if (write_i8(fd, SZ_F32) != 0)
		return -1;
	if (write_i8v(fd, name, strlen(name)) != 0)
		return -1;
	if (write_f32v(fd, data, len) != 0)
		return -1;
	return 0;
}

int sz_write_f64v(int fd, const char *name, double *data, size_t len)
{
	if (write_i8(fd, SZ_F64) != 0)
		return -1;
	if (write_i8v(fd, name, strlen(name)) != 0)
		return -1;
	if (write_f64v(fd, data, len) != 0)
		return -1;
	return 0;
}

int read_data(int fd, void *data, size_t len)
{
	int rval = read(fd, data, len);
	if (rval < 0) {
		perror("read_data");
		return -1;
	}
	if (rval < len) {
		fprintf(stderr, "read_data: %d of %d bytes read\n", rval, len);
		return -1;
	}
	return 0;
}

int read_i8(int fd, int8_t *data)
{
	return read_data(fd, data, sizeof(*data));
}

int read_i16(int fd, int16_t *data)
{
	return read_data(fd, data, sizeof(*data));
}

int read_i32(int fd, int32_t *data)
{
	return read_data(fd, data, sizeof(*data));
}

int read_i64(int fd, int64_t *data)
{
	return read_data(fd, data, sizeof(*data));
}

int read_f32(int fd, float *data)
{
	return read_data(fd, data, sizeof(*data));
}

int read_f64(int fd, double *data)
{
	return read_data(fd, data, sizeof(*data));
}

int read_i8v(int fd, int8_t **data, size_t *size)
{
	int16_t tmp;
	if (read_data(fd, &tmp, sizeof(tmp)) != 0)
		return -1;
	if (size != NULL)
		*size = tmp;
	if (*data == NULL)
		*data = calloc(tmp, sizeof(**data));
	if (read_data(fd, *data, sizeof(*data) * tmp) != 0)
		return -1;
	return 0;
}

int read_i16v(int fd, int16_t **data, size_t *size)
{
	int16_t tmp;
	if (read_data(fd, &tmp, sizeof(tmp)) != 0)
		return -1;
	if (size != NULL)
		*size = tmp;
	if (*data == NULL)
		*data = calloc(tmp, sizeof(**data));
	if (read_data(fd, *data, sizeof(*data) * tmp) != 0)
		return -1;
	return 0;
}

int read_i32v(int fd, int32_t **data, size_t *size)
{
	int16_t tmp;
	if (read_data(fd, &tmp, sizeof(tmp)) != 0)
		return -1;
	if (size != NULL)
		*size = tmp;
	if (*data == NULL)
		*data = calloc(tmp, sizeof(**data));
	if (read_data(fd, *data, sizeof(*data) * tmp) != 0)
		return -1;
	return 0;
}

int read_i64v(int fd, int64_t **data, size_t *size)
{
	int16_t tmp;
	if (read_data(fd, &tmp, sizeof(tmp)) != 0)
		return -1;
	if (size != NULL)
		*size = tmp;
	if (*data == NULL)
		*data = calloc(tmp, sizeof(**data));
	if (read_data(fd, *data, sizeof(*data) * tmp) != 0)
		return -1;
	return 0;
}

int read_f32v(int fd, float **data, size_t *size)
{
	int16_t tmp;
	if (read_data(fd, &tmp, sizeof(tmp)) != 0)
		return -1;
	if (size != NULL)
		*size = tmp;
	if (*data == NULL)
		*data = calloc(tmp, sizeof(**data));
	if (read_data(fd, *data, sizeof(*data) * tmp) != 0)
		return -1;
	return 0;
}

int read_f64v(int fd, double **data, size_t *size)
{
	int16_t tmp;
	if (read_data(fd, &tmp, sizeof(tmp)) != 0)
		return -1;
	if (size != NULL)
		*size = tmp;
	if (*data == NULL)
		*data = calloc(tmp, sizeof(**data));
	if (read_data(fd, *data, sizeof(*data) * tmp) != 0)
		return -1;
	return 0;
}

int read_list(int fd, struct sz_list *list)
{
	if (read_i8(fd, &list->tag) != 0)
		return -1;
	list->data = NULL;
	if (list->tag == SZ_I8)
		return read_i8v(fd, (int8_t**)&list->data, &list->size);
	if (list->tag == SZ_I16)
		return read_i16v(fd, (int16_t**)&list->data, &list->size);
	if (list->tag == SZ_I32)
		return read_i32v(fd, (int32_t**)&list->data, &list->size);
	if (list->tag == SZ_I64)
		return read_i64v(fd, (int64_t**)&list->data, &list->size);
	if (list->tag == SZ_F32)
		return read_f32v(fd, (float **)&list->data, &list->size);
	if (list->tag == SZ_F64)
		return read_f64v(fd, (double **)&list->data, &list->size);
	fprintf(stderr, "read_list: bad tag: %d\n", list->tag);
	return -1;
}

int sz_read(int fd, struct sz_tag *it)
{
	if (it->tag == SZ_LIST)
		free(it->data.list.data);
	if (read_i8(fd, &it->tag) != 0)
		return -1;
	if (it->tag == SZ_END)
		return SZ_END;
	if (read_i8v(fd, (int8_t **)&it->name, NULL) != 0)
		return -1;
	if (it->tag == SZ_NULL)
		return 0;
	if (it->tag == SZ_I8)
		return read_i8(fd, &it->data.i8);
	if (it->tag == SZ_I16)
		return read_i16(fd, &it->data.i16);
	if (it->tag == SZ_I32)
		return read_i32(fd, &it->data.i32);
	if (it->tag == SZ_I64)
		return read_i64(fd, &it->data.i64);
	if (it->tag == SZ_F32)
		return read_f32(fd, &it->data.f32);
	if (it->tag == SZ_F64)
		return read_f64(fd, &it->data.f64);
	if (it->tag == SZ_LIST)
		return read_list(fd, &it->data.list);
	fprintf(stderr, "sz_read: bad tag: %d\n", it->tag);
	return SZ_ERROR;
}

int sz_to_null(struct sz_tag *t)
{
	if (t->tag != SZ_NULL) {
		fprintf(stderr, "%s: not null\n", __func__);
		return -1;
	}
	return 0;
}

int sz_to_i8(struct sz_tag *t, int8_t *data)
{
	if (t->tag != SZ_I8) {
		fprintf(stderr, "%s: not i8\n", __func__);
		return -1;
	}
	*data = t->data.i8;
	return 0;
}

int sz_to_i16(struct sz_tag *t, int16_t *data)
{
	if (t->tag != SZ_I16) {
		fprintf(stderr, "%s: not i16\n", __func__);
		return -1;
	}
	*data = t->data.i16;
	return 0;
}

int sz_to_i32(struct sz_tag *t, int32_t *data)
{
	if (t->tag != SZ_I32) {
		fprintf(stderr, "%s: not i32\n", __func__);
		return -1;
	}
	*data = t->data.i32;
	return 0;
}

int sz_to_i64(struct sz_tag *t, int64_t *data)
{
	if (t->tag != SZ_I64) {
		fprintf(stderr, "%s: not i64\n", __func__);
		return -1;
	}
	*data = t->data.i64;
	return 0;
}

int sz_to_f32(struct sz_tag *t, float *data)
{
	if (t->tag != SZ_F32) {
		fprintf(stderr, "%s: not f32\n", __func__);
		return -1;
	}
	*data = t->data.f32;
	return 0;
}

int sz_to_f64(struct sz_tag *t, double *data)
{
	if (t->tag != SZ_F64) {
		fprintf(stderr, "%s: not f64\n", __func__);
		return -1;
	}
	*data = t->data.f64;
	return 0;
}

int sz_to_i8v(struct sz_tag *t, int8_t **data, size_t *len)
{
	if (t->tag != SZ_LIST) {
		fprintf(stderr, "%s: not a list\n", __func__);
		return -1;
	}
	if (t->data.list.tag != SZ_I8) {
		fprintf(stderr, "%s: not i8\n", __func__);
		return -1;
	}
	*data = t->data.list.data;
	*len = t->data.list.size;
	return 0;
}

int sz_to_i16v(struct sz_tag *t, int16_t **data, size_t *len)
{
	if (t->tag != SZ_LIST) {
		fprintf(stderr, "%s: not a list\n", __func__);
		return -1;
	}
	if (t->data.list.tag != SZ_I16) {
		fprintf(stderr, "%s: not i16\n", __func__);
		return -1;
	}
	*data = t->data.list.data;
	*len = t->data.list.size;
	return 0;
}

int sz_to_i32v(struct sz_tag *t, int32_t **data, size_t *len)
{
	if (t->tag != SZ_LIST) {
		fprintf(stderr, "%s: not a list\n", __func__);
		return -1;
	}
	if (t->data.list.tag != SZ_I32) {
		fprintf(stderr, "%s: not i32\n", __func__);
		return -1;
	}
	*data = t->data.list.data;
	*len = t->data.list.size;
	return 0;
}

int sz_to_i64v(struct sz_tag *t, int64_t **data, size_t *len)
{
	if (t->tag != SZ_LIST) {
		fprintf(stderr, "%s: not a list\n", __func__);
		return -1;
	}
	if (t->data.list.tag != SZ_I64) {
		fprintf(stderr, "%s: not i64\n", __func__);
		return -1;
	}
	*data = t->data.list.data;
	*len = t->data.list.size;
	return 0;
}

int sz_to_f32v(struct sz_tag *t, float **data, size_t *len)
{
	if (t->tag != SZ_LIST) {
		fprintf(stderr, "%s: not a list\n", __func__);
		return -1;
	}
	if (t->data.list.tag != SZ_F32) {
		fprintf(stderr, "%s: not f32\n", __func__);
		return -1;
	}
	*data = t->data.list.data;
	*len = t->data.list.size;
	return 0;
}

int sz_to_f64v(struct sz_tag *t, double **data, size_t *len)
{
	if (t->tag != SZ_LIST) {
		fprintf(stderr, "%s: not a list\n", __func__);
		return -1;
	}
	if (t->data.list.tag != SZ_F64) {
		fprintf(stderr, "%s: not f64\n", __func__);
		return -1;
	}
	*data = t->data.list.data;
	*len = t->data.list.size;
	return 0;
}

int sz_copy_i8v(struct sz_tag *t, int8_t *data, size_t len)
{
	if (t->tag != SZ_LIST) {
		fprintf(stderr, "%s: not a list\n", __func__);
		return -1;
	}
	if (t->data.list.tag != SZ_I8) {
		fprintf(stderr, "%s: not i8\n", __func__);
		return -1;
	}
	if (len < t->data.list.size) {
		fprintf(stderr, "%s: not enough space", __func__);
		return -1;
	}
	memcpy(data, t->data.list.data, t->data.list.size * sizeof(*data));
	return 0;
}

int sz_copy_i16v(struct sz_tag *t, int16_t *data, size_t len)
{
	if (t->tag != SZ_LIST) {
		fprintf(stderr, "%s: not a list\n", __func__);
		return -1;
	}
	if (t->data.list.tag != SZ_I16) {
		fprintf(stderr, "%s: not i16\n", __func__);
		return -1;
	}
	if (len < t->data.list.size) {
		fprintf(stderr, "%s: not enough space", __func__);
		return -1;
	}
	memcpy(data, t->data.list.data, t->data.list.size * sizeof(*data));
	return 0;
}

int sz_copy_i32v(struct sz_tag *t, int32_t *data, size_t len)
{
	if (t->tag != SZ_LIST) {
		fprintf(stderr, "%s: not a list\n", __func__);
		return -1;
	}
	if (t->data.list.tag != SZ_I32) {
		fprintf(stderr, "%s: not i32\n", __func__);
		return -1;
	}
	if (len < t->data.list.size) {
		fprintf(stderr, "%s: not enough space", __func__);
		return -1;
	}
	memcpy(data, t->data.list.data, t->data.list.size * sizeof(*data));
	return 0;
}

int sz_copy_i64v(struct sz_tag *t, int64_t *data, size_t len)
{
	if (t->tag != SZ_LIST) {
		fprintf(stderr, "%s: not a list\n", __func__);
		return -1;
	}
	if (t->data.list.tag != SZ_I64) {
		fprintf(stderr, "%s: not i64\n", __func__);
		return -1;
	}
	if (len < t->data.list.size) {
		fprintf(stderr, "%s: not enough space", __func__);
		return -1;
	}
	memcpy(data, t->data.list.data, t->data.list.size * sizeof(*data));
	return 0;
}

int sz_copy_f32v(struct sz_tag *t, float *data, size_t len)
{
	if (t->tag != SZ_LIST) {
		fprintf(stderr, "%s: not a list\n", __func__);
		return -1;
	}
	if (t->data.list.tag != SZ_F32) {
		fprintf(stderr, "%s: not f32\n", __func__);
		return -1;
	}
	if (len < t->data.list.size) {
		fprintf(stderr, "%s: not enough space", __func__);
		return -1;
	}
	memcpy(data, t->data.list.data, t->data.list.size * sizeof(*data));
	return 0;
}

int sz_copy_f64v(struct sz_tag *t, double *data, size_t len)
{
	if (t->tag != SZ_LIST) {
		fprintf(stderr, "%s: not a list\n", __func__);
		return -1;
	}
	if (t->data.list.tag != SZ_F64) {
		fprintf(stderr, "%s: not f64\n", __func__);
		return -1;
	}
	if (len < t->data.list.size) {
		fprintf(stderr, "%s: not enough space", __func__);
		return -1;
	}
	memcpy(data, t->data.list.data, t->data.list.size * sizeof(*data));
	return 0;
}

