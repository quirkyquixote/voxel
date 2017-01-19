/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_SZ_H_
#define SRC_SZ_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <vector>
#include <string>
#include <utility>
#include <unordered_map>

#include "log.h"

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

class sz_Exception {
 public:
	sz_Exception() { }
	~sz_Exception() { }
};

class sz_Tag {
 public:
	typedef std::string Raw;
	typedef std::vector<sz_Tag *> List;
	typedef std::unordered_map<std::string, sz_Tag *> Dict;

	sz_Tag() { }
	virtual ~sz_Tag() { }

	virtual inline uint8_t get_tag() const { return SZ_NULL; }

	virtual inline int8_t get_i8() const {
		log_error("expected i8");
		throw sz_Exception();
	}
	virtual inline int16_t get_i16() const {
		log_error("expected i16");
		throw sz_Exception();
	}
	virtual inline int32_t get_i32() const {
		log_error("expected i32");
		throw sz_Exception();
	}
	virtual inline int64_t get_i64() const {
		log_error("expected i64");
		throw sz_Exception();
	}
	virtual inline float get_f32() const {
		log_error("expected f32");
		throw sz_Exception();
	}
	virtual inline double get_f64() const {
		log_error("expected f64");
		throw sz_Exception();
	}
	virtual inline const char *get_str() const {
		log_error("expected str");
		throw sz_Exception();
	}
	virtual inline Raw &get_raw() {
		log_error("expected raw");
		throw sz_Exception();
	}
	virtual inline List &get_list() {
		log_error("expected list");
		throw sz_Exception();
	}
	virtual inline Dict &get_dict() {
		log_error("expected dict");
		throw sz_Exception();
	}
};

class sz_i8 : public sz_Tag {
 public:
	explicit sz_i8(int8_t val) : val(val) {}
	~sz_i8() { }
	inline uint8_t get_tag() const { return SZ_I8; }
	inline int8_t get_i8() const { return val; }
 private:
	int8_t val;
};

class sz_i16 : public sz_Tag {
 public:
	explicit sz_i16(int16_t val) : val(val) {}
	~sz_i16() { }
	inline uint8_t get_tag() const { return SZ_I16; }
	inline int16_t get_i16() const { return val; }
 private:
	int16_t val;
};

class sz_i32 : public sz_Tag {
 public:
	explicit sz_i32(int32_t val) : val(val) {}
	~sz_i32() { }
	inline uint8_t get_tag() const { return SZ_I32; }
	inline int32_t get_i32() const { return val; }
 private:
	int32_t val;
};

class sz_i64 : public sz_Tag {
 public:
	explicit sz_i64(int64_t val) : val(val) {}
	~sz_i64() { }
	inline uint8_t get_tag() const { return SZ_I64; }
	inline int64_t get_i64() const { return val; }
 private:
	int64_t val;
};

class sz_f32 : public sz_Tag {
 public:
	explicit sz_f32(float val) : val(val) {}
	~sz_f32() { }
	inline uint8_t get_tag() const { return SZ_F32; }
	inline float get_f32() const { return val; }
 private:
	float val;
};

class sz_f64 : public sz_Tag {
 public:
	explicit sz_f64(double val) : val(val) {}
	~sz_f64() { }
	inline uint8_t get_tag() const { return SZ_F64; }
	inline double get_f64() const { return val; }
 private:
	double val;
};

class sz_Str : public sz_Tag {
 public:
	explicit sz_Str(const char *val) : val(strdup(val)) { }
	~sz_Str() { free(val); }
	inline uint8_t get_tag() const { return SZ_STR; }
	inline const char *get_str() const { return val; }
 private:
	char *val;
};

class sz_Raw : public sz_Tag {
 public:
	sz_Raw(const void *buf, size_t bytes)
		: data(reinterpret_cast<const char *>(buf), bytes) { }
	~sz_Raw() { }
	inline uint8_t get_tag() const { return SZ_RAW; }
	inline Raw &get_raw() { return data; }
 private:
	Raw data;
};

class sz_List : public sz_Tag {
 public:
	sz_List() { }
	~sz_List()
	{
		for (auto &it : val)
			delete it;
	}
	inline uint8_t get_tag() const { return SZ_LIST; }
	inline List &get_list() { return val; }
 private:
	List val;
};

class sz_Dict : public sz_Tag {
 public:
	sz_Dict() { }
	~sz_Dict()
	{
		for (auto &it : val) {
			delete it.second;
		}
	}
	inline uint8_t get_tag() const { return SZ_DICT; }
	inline Dict &get_dict() { return val; }
 private:
	Dict val;
};

sz_Tag *sz_read(int fd);
void sz_write(int fd, sz_Tag *root);

static inline void sz_list_add(sz_Tag *d, sz_Tag *v)
{
	d->get_list().push_back(v);
}

static inline void sz_dict_add(sz_Tag *d, const char *k, sz_Tag *v)
{
	d->get_dict().insert(std::make_pair(std::string(k), v));
}

static inline sz_Tag *sz_dict_lookup(sz_Tag *d, const char *k)
{
	auto v = d->get_dict().find(k);
	if (v == d->get_dict().end()) {
		log_error("expected \"%s\"", k);
		throw sz_Exception();
	}
	return v->second;
}

#endif  // SRC_SZ_H_

