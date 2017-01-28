/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_SERIALIZER_H_
#define SRC_SERIALIZER_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <vector>
#include <string>
#include <utility>
#include <unordered_map>

#include "exception.h"

namespace serializer {

enum {
	NUL = 0,
	I8 = 1,
	I16 = 2,
	I32 = 3,
	I64 = 4,
	F32 = 5,
	F64 = 6,
	STR = 7,
	RAW = 8,
	LIST = 9,
	DICT = 10,
};

class Tag {
 public:
	typedef std::string RawData;
	typedef std::vector<Tag *> ListData;
	typedef std::unordered_map<std::string, Tag *> DictData;

	Tag() { }
	virtual ~Tag() { }

	virtual inline uint8_t get_tag() const { return NUL; }

	virtual inline int8_t get_i8() const {
		throw Exception("expected i8");
	}
	virtual inline int16_t get_i16() const {
		throw Exception("expected i16");
	}
	virtual inline int32_t get_i32() const {
		throw Exception("expected i32");
	}
	virtual inline int64_t get_i64() const {
		throw Exception("expected i64");
	}
	virtual inline float get_f32() const {
		throw Exception("expected f32");
	}
	virtual inline double get_f64() const {
		throw Exception("expected f64");
	}
	virtual inline const char *get_str() const {
		throw Exception("expected string");
	}
	virtual inline RawData &get_raw() {
		throw Exception("expected raw");
	}
	virtual inline ListData &get_list() {
		throw Exception("expected list");
	}
	virtual inline DictData &get_dict() {
		throw Exception("expected dict");
	}
};

class i8 : public Tag {
 public:
	explicit i8(int8_t val) : val(val) {}
	~i8() { }
	inline uint8_t get_tag() const { return I8; }
	inline int8_t get_i8() const { return val; }
 private:
	int8_t val;
};

class i16 : public Tag {
 public:
	explicit i16(int16_t val) : val(val) {}
	~i16() { }
	inline uint8_t get_tag() const { return I16; }
	inline int16_t get_i16() const { return val; }
 private:
	int16_t val;
};

class i32 : public Tag {
 public:
	explicit i32(int32_t val) : val(val) {}
	~i32() { }
	inline uint8_t get_tag() const { return I32; }
	inline int32_t get_i32() const { return val; }
 private:
	int32_t val;
};

class i64 : public Tag {
 public:
	explicit i64(int64_t val) : val(val) {}
	~i64() { }
	inline uint8_t get_tag() const { return I64; }
	inline int64_t get_i64() const { return val; }
 private:
	int64_t val;
};

class f32 : public Tag {
 public:
	explicit f32(float val) : val(val) {}
	~f32() { }
	inline uint8_t get_tag() const { return F32; }
	inline float get_f32() const { return val; }
 private:
	float val;
};

class f64 : public Tag {
 public:
	explicit f64(double val) : val(val) {}
	~f64() { }
	inline uint8_t get_tag() const { return F64; }
	inline double get_f64() const { return val; }
 private:
	double val;
};

class Str : public Tag {
 public:
	explicit Str(const char *val) : val(strdup(val)) { }
	~Str() { free(val); }
	inline uint8_t get_tag() const { return STR; }
	inline const char *get_str() const { return val; }
 private:
	char *val;
};

class Raw : public Tag {
 public:
	Raw(const void *buf, size_t bytes)
		: data(reinterpret_cast<const char *>(buf), bytes) { }
	~Raw() { }
	inline uint8_t get_tag() const { return RAW; }
	inline RawData &get_raw() { return data; }
 private:
	RawData data;
};

class List : public Tag {
 public:
	List() { }
	~List()
	{
		for (auto &it : val)
			delete it;
	}
	inline uint8_t get_tag() const { return LIST; }
	inline ListData &get_list() { return val; }
 private:
	ListData val;
};

class Dict : public Tag {
 public:
	Dict() { }
	~Dict()
	{
		for (auto &it : val) {
			delete it.second;
		}
	}
	inline uint8_t get_tag() const { return DICT; }
	inline DictData &get_dict() { return val; }
 private:
	DictData val;
};

Tag *read(int fd);
void write(int fd, Tag *root);

static inline void list_add(Tag *d, Tag *v)
{
	d->get_list().push_back(v);
}

static inline void dict_add(Tag *d, const char *k, Tag *v)
{
	d->get_dict().insert(std::make_pair(std::string(k), v));
}

static inline Tag *dict_lookup(Tag *d, const char *k)
{
	auto v = d->get_dict().find(k);
	if (v == d->get_dict().end()) {
		throw Exception("expected \"%s\"", k);
	}
	return v->second;
}

};  // namespace serializer

#endif  // SRC_SERIALIZER_H_

