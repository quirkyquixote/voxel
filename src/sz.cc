/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */


#include "sz.h"

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#include "context.h"

void read_bytes(int fd, void *data, size_t nbytes)
{
	int ret;
	ret = read(fd, data, nbytes);
	if (ret < 0) {
		log_error("%s", strerror(errno));
		throw sz_Exception();
	}
	if (ret < nbytes) {
		log_error("read %d of %zd bytes", ret, nbytes);
		throw sz_Exception();
	}
}

template<typename T> void read_atom(int fd, T *atom)
{
	read_bytes(fd, atom, sizeof(*atom));
}

void read_str(int fd, char **data, int16_t *rlen)
{
	int16_t len;
	read_atom(fd, &len);
	if (rlen != NULL)
		*rlen = len;
	if (len == 0) {
		*data = NULL;
	} else {
		*data = new char[len + 1];
		read_bytes(fd, *data, len);
		(*data)[len] = 0;
	}
}

void read_raw(int fd, void **data, int32_t *len)
{
	read_atom(fd, len);
	*data = new char[*len];
	read_bytes(fd, *data, *len);
}

sz_Tag *read_list(int fd)
{
	sz_Tag *root, *tag;
	int32_t size;
	int i;

	root = new sz_List();
	read_atom(fd, &size);
	for (i = 0; i < size; ++i) {
		tag = sz_read(fd);
		root->get_list().push_back(tag);
	}
	return root;
}

sz_Tag *read_dict(int fd)
{
	sz_Tag *root, *tag;
	int32_t size;
	int i;
	char *key;

	root = new sz_Dict();
	read_atom(fd, &size);
	for (i = 0; i < size; ++i) {
		read_str(fd, &key, NULL);
		tag = sz_read(fd);
		root->get_dict().insert(std::make_pair(key, tag));
	}
	return root;
}

sz_Tag *sz_read(int fd)
{
	uint8_t tag;
	read_atom(fd, &tag);
	if (tag == SZ_NULL) {
		return NULL;
	} else if (tag == SZ_I8) {
		int8_t data;
		read_atom(fd, &data);
		return new sz_i8(data);
	} else if (tag == SZ_I16) {
		int16_t data;
		read_atom(fd, &data);
		return new sz_i16(data);
	} else if (tag == SZ_I32) {
		int32_t data;
		read_atom(fd, &data);
		return new sz_i32(data);
	} else if (tag == SZ_I64) {
		int64_t data;
		read_atom(fd, &data);
		return new sz_i64(data);
	} else if (tag == SZ_F32) {
		float data;
		read_atom(fd, &data);
		return new sz_f32(data);
	} else if (tag == SZ_F64) {
		double data;
		read_atom(fd, &data);
		return new sz_f64(data);
	} else if (tag == SZ_STR) {
		char *data;
		read_str(fd, &data, NULL);
		return new sz_Str(data);
	} else if (tag == SZ_RAW) {
		void *data;
		int32_t size;
		read_raw(fd, &data, &size);
		return new sz_Raw(data, size);
	} else if (tag == SZ_LIST) {
		return read_list(fd);
	} else if (tag == SZ_DICT) {
		return read_dict(fd);
	}
	log_error("bad tag %02x", tag);
	throw sz_Exception();
}

void write_bytes(int fd, const void *data, size_t nbytes)
{
	int ret;
	ret = write(fd, data, nbytes);
	if (ret < 0) {
		log_error("%s", strerror(errno));
		throw sz_Exception();
	}
	if (ret < nbytes) {
		log_error("write %d of %zd bytes", ret, nbytes);
		throw sz_Exception();
	}
}

template<typename T> void write_atom(int fd, T atom)
{
	write_bytes(fd, &atom, sizeof(atom));
}

void write_str(int fd, const char *data, int16_t len)
{
	write_atom(fd, len);
	write_bytes(fd, data, len);
}

void write_raw(int fd, const char *data, int32_t len)
{
	write_atom(fd, len);
	write_bytes(fd, data, len);
}

void write_list(int fd, sz_Tag *root)
{
	int32_t size = root->get_list().size();
	write_atom(fd, size);
	for (auto &iter : root->get_list())
		sz_write(fd, iter);
}

void write_dict(int fd, sz_Tag *root)
{
	int32_t size = root->get_dict().size();
	write_atom(fd, size);
	for (auto &iter : root->get_dict()) {
		write_str(fd, iter.first.data(), iter.first.size());
		sz_write(fd, iter.second);
	}
}

void sz_write(int fd, sz_Tag *tag)
{
	int8_t tt = tag->get_tag();
	write_atom(fd, tt);
	if (tt == SZ_NULL) {
		return;
	} else if (tt == SZ_I8) {
		return write_atom(fd, tag->get_i8());
	} else if (tt == SZ_I16) {
		return write_atom(fd, tag->get_i16());
	} else if (tt == SZ_I32) {
		return write_atom(fd, tag->get_i32());
	} else if (tt == SZ_I64) {
		return write_atom(fd, tag->get_i64());
	} else if (tt == SZ_F32) {
		return write_atom(fd, tag->get_f32());
	} else if (tt == SZ_F64) {
		return write_atom(fd, tag->get_f64());
	} else if (tt == SZ_STR) {
		return write_str(fd, tag->get_str(), strlen(tag->get_str()));
	} else if (tt == SZ_RAW) {
		return write_raw(fd, tag->get_raw().data(), tag->get_raw().size());
	} else if (tt == SZ_LIST) {
		return write_list(fd, tag);
	} else if (tt == SZ_DICT) {
		return write_dict(fd, tag);
	}
	log_error("bad tag %02x", tt);
	throw sz_Exception();
}
