
#ifndef VOXEL_STACK_H_
#define VOXEL_STACK_H_

#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct stack {
	size_t elem_size;
	size_t alloc;
	size_t size;
	char *data;
};

static inline struct stack *stack(size_t elem_size)
{
	struct stack *s = calloc(1, sizeof(*s));
	s->elem_size = elem_size;
	return s;
}

static inline void stack_destroy(struct stack *s)
{
	if (s->data != NULL)
		free(s->data);
	free(s);
}

static inline void stack_push(struct stack *s, void *e)
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

static inline void stack_pop(struct stack *s, void *e)
{
	assert(s->size > 0);
	--s->size;
	memcpy(e, s->data + s->size * s->elem_size, s->elem_size);
}

static inline int stack_size(struct stack *s)
{
	return s->size;
}

#endif
