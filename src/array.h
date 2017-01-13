
#ifndef VOXEL_ARRAY_H_
#define VOXEL_ARRAY_H_

#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct array {
	size_t elem_size;
	size_t alloc;
	size_t size;
	char *data;
};

static inline struct array *array(size_t elem_size)
{
	struct array *s = calloc(1, sizeof(*s));
	s->elem_size = elem_size;
	return s;
}

static inline void array_destroy(struct array *s)
{
	if (s->data != NULL)
		free(s->data);
	free(s);
}

static inline void array_resize(struct array *s, size_t size)
{
	size_t alloc = 2;
	while (alloc < size)
		alloc *= 2;
	if (alloc != s->alloc)
		s->data = realloc(s->data, alloc * s->elem_size);
	s->alloc = alloc;
	s->size = size;
}

static inline void array_push(struct array *s, void *e)
{
	if (s->size == s->alloc) {
		if (s->alloc == 0)
			s->alloc = 2;
		else
			s->alloc *= 2;
		s->data = realloc(s->data, s->alloc * s->elem_size);
	}
	memcpy(s->data + s->size * s->elem_size, e, s->elem_size);
	++s->size;
}

static inline void *array_pop(struct array *s, void *e)
{
	assert(s->size > 0);
	--s->size;
	return memcpy(e, s->data + s->size * s->elem_size, s->elem_size);
}

static inline void *array_top(struct array *s, void *e)
{
	assert(s->size > 0);
	return memcpy(e, s->data + (s->size - 1) * s->elem_size, s->elem_size);
}

static inline void *array_get(struct array *s, size_t i, void *e)
{
	assert(i < s->size);
	return memcpy(e, s->data + i * s->elem_size, s->elem_size);
}

static inline void *array_set(struct array *s, size_t i, void *e)
{
	assert(i < s->size);
	return memcpy(s->data + i * s->elem_size, e, s->elem_size);
}

static inline int array_size(struct array *s)
{
	return s->size;
}

static inline void array_clear(struct array *s)
{
	s->size = 0;
}

#define array_foreach(_iter,_cont) \
for (int i = 0; i < array_size(_cont) && array_get(_cont, i, &_iter); ++i)

#endif
