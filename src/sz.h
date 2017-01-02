

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
	SZ_RAW = 8,
	SZ_LIST = 9,
	SZ_DICT = 10,
};

struct sz_i8 {
	uint8_t tag;
	int8_t data;
};

struct sz_i16 {
	uint8_t tag;
	int16_t data;
};

struct sz_i32 {
	uint8_t tag;
	int32_t data;
};

struct sz_i64 {
	uint8_t tag;
	int64_t data;
};

struct sz_f32 {
	uint8_t tag;
	float data;
};

struct sz_f64 {
	uint8_t tag;
	double data;
};

struct sz_str {
	uint8_t tag;
	int16_t size;
	char *data;
};

struct sz_raw {
	uint8_t tag;
	int32_t size;
	void *data;
};

struct sz_list {
	uint8_t tag;
	int32_t size;
	int32_t alloc;
	union sz_tag **vals;
};

struct sz_dict {
	uint8_t tag;
	int32_t size;
	int32_t alloc;
	char **keys;
	union sz_tag **vals;
};

union sz_tag {
	uint8_t tag;
	struct sz_i8 i8;
	struct sz_i16 i16;
	struct sz_i32 i32;
	struct sz_i64 i64;
	struct sz_f32 f32;
	struct sz_f64 f64;
	struct sz_str str;
	struct sz_raw raw;
	struct sz_list list;
	struct sz_dict dict;
};

union sz_tag *sz_null(void);
union sz_tag *sz_i8(int8_t data);
union sz_tag *sz_i16(int16_t data);
union sz_tag *sz_i32(int32_t data);
union sz_tag *sz_i64(int64_t data);
union sz_tag *sz_f32(float data);
union sz_tag *sz_f64(double data);
union sz_tag *sz_str(const char *data);
union sz_tag *sz_raw(const void *data, size_t size);
union sz_tag *sz_list(void);
union sz_tag *sz_dict(void);

void sz_destroy(union sz_tag *tag);

int sz_list_add(union sz_tag *list, union sz_tag *elem);
int sz_dict_add(union sz_tag *dict, const char *key, union sz_tag *elem);

#define sz_list_foreach(_iter,_list) \
for (int _i = 0; _iter = _list->list.vals[_i], _i < _list->list.size; ++_i)

#define sz_dict_foreach(_key,_val,_dict) \
for (int _i = 0; _key = _dict->dict.keys[_i], _val = _dict->dict.vals[_i], _i < _dict->dict.size; ++_i)

int sz_read(int fd, union sz_tag *root);
int sz_write(int fd, union sz_tag *root);

#endif

