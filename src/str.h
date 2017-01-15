
#ifndef VOXEL_STRING_H_
#define VOXEL_STRING_H_

#include <str.h>
#include <assert.h>
#include <stdlib.h>

struct str {
	char *str;
	size_t alloc;
	size_t len;
};

static inline struct str *str(void)
{
	struct str *s = calloc(1, sizeof(*s));
	return s;
}

static inline void str_destroy(struct str *s)
{
	if (s->str != NULL)
		free(s->str);
	free(s);
}

static inline void str_grow(struct str *s, size_t len)
{
	if (s->alloc < len) {
		do
			if (s->alloc == 0)
				s->alloc = 2;
			else
				s->alloc *= 2;
		while(s->alloc < len);
		s->str = realloc(s->str, s->alloc);
	}
}

static inline void str_assign(struct str *s, const void *str, size_t len)
{
	str_grow(s, len + 1);
	memcpy(s->str, str, len + 1);
	s->len = len;
}

static inline void str_insert(struct str *s, size_t where, const void *str, size_t len)
{
	assert(where <= s->len);
	str_grow(s, s->len + len + 1);
	memmove(s->str + (where + len), s->str + where, s->len - where + 1);
	memcpy(s->str + where, str, len);
	s->len += len;
}

static inline void str_append(struct str *s, const void *str, size_t len)
{
	str_insert(s, s->len, str, len);
}

static inline void str_prepend(struct str *s, const void *str, size_t len)
{
	str_insert(s, 0, str, len);
}

static inline void str_replace(struct str *s, size_t where, const void *str, size_t len)
{
	assert(where + len <= s->len);
	memcpy(s->str + where, str, len);
}

static inline void str_erase(struct str *s, size_t where, size_t len)
{
	assert(where + len <= s->len);
	memmove(s->str + where, s->str + (where + len), s->len - where + len + 1);
	s->len -= len;
}

static inline void str_insert_char(struct str *s, size_t where, char c)
{
	str_insert(s, where, &c, 1);
}

static inline void str_append_char(struct str *s, char c)
{
	str_append(s, &c, 1);
}

static inline void str_prepend_char(struct str *s, char c)
{
	str_prepend(s, &c, 1);
}

static inline void str_replace_char(struct str *s, size_t where, char c)
{
	str_replace(s, where, &c, 1);
}

static inline void str_erase_char(struct str *s, size_t where)
{
	str_erase(s, where, 1);
}

#endif
