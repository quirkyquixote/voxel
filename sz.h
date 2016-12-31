

#ifndef SZ_H_
#define SZ_H_

#include <stdint.h>
#include <stdlib.h>

enum sz_typeid {
	SZ_NULL = 0,
	SZ_I8 = 1,
	SZ_I16 = 2,
	SZ_I32 = 3,
	SZ_I64 = 4,
	SZ_F32 = 5,
	SZ_F64 = 6,
	SZ_STR = 7,
	SZ_LIST = 8,
	SZ_DICT = 9,
	SZ_END = -1,
	SZ_ERROR = -2,
};

struct sz_list {
	int8_t tag;
	size_t size;
	void *data;
};

union sz_data {
	int8_t i8;
	int16_t i16;
	int32_t i32;
	int64_t i64;
	float f32;
	double f64;
	struct sz_list list;
};

struct sz_tag {
	int8_t tag;
	char *name;
	union sz_data data;
};

int sz_write_null(int fd, const char *name);
int sz_write_i8(int fd, const char *name, int8_t data);
int sz_write_i16(int fd, const char *name, int16_t data);
int sz_write_i32(int fd, const char *name, int32_t data);
int sz_write_i64(int fd, const char *name, int64_t data);
int sz_write_f32(int fd, const char *name, float data);
int sz_write_f64(int fd, const char *name, double data);
int sz_write_i8v(int fd, const char *name, int8_t *data, size_t len);
int sz_write_i16v(int fd, const char *name, int16_t *data, size_t len);
int sz_write_i32v(int fd, const char *name, int32_t *data, size_t len);
int sz_write_i64v(int fd, const char *name, int64_t *data, size_t len);
int sz_write_f32v(int fd, const char *name, float *data, size_t len);
int sz_write_f64v(int fd, const char *name, double *data, size_t len);

int sz_read(int fd, struct sz_tag *it);
int sz_to_null(struct sz_tag *t);
int sz_to_i8(struct sz_tag *t, int8_t *data);
int sz_to_i16(struct sz_tag *t, int16_t *data);
int sz_to_i32(struct sz_tag *t, int32_t *data);
int sz_to_i64(struct sz_tag *t, int64_t *data);
int sz_to_f32(struct sz_tag *t, float *data);
int sz_to_f64(struct sz_tag *t, double *data);
int sz_to_i8v(struct sz_tag *t, int8_t **data, size_t *len);
int sz_to_i16v(struct sz_tag *t, int16_t **data, size_t *len);
int sz_to_i32v(struct sz_tag *t, int32_t **data, size_t *len);
int sz_to_i64v(struct sz_tag *t, int64_t **data, size_t *len);
int sz_to_f32v(struct sz_tag *t, float **data, size_t *len);
int sz_to_f64v(struct sz_tag *t, double **data, size_t *len);

int sz_copy_i8v(struct sz_tag *t, int8_t *data, size_t len);
int sz_copy_i16v(struct sz_tag *t, int16_t *data, size_t len);
int sz_copy_i32v(struct sz_tag *t, int32_t *data, size_t len);
int sz_copy_i64v(struct sz_tag *t, int64_t *data, size_t len);
int sz_copy_f32v(struct sz_tag *t, float *data, size_t len);
int sz_copy_f64v(struct sz_tag *t, double *data, size_t len);

#endif
