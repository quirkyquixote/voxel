
#ifndef VOXEL_RESOURCE_H_
#define VOXEL_RESOURCE_H_

#include <stdlib.h>

struct resource {
	struct resource *next;
	char *name;
	void *data;
	size_t ref_count;
};

struct resource_traits {
	int(*create)(const char *name, void **data);
	int(*destroy)(void *data);
};

struct resource_factory {
	const struct resource_traits *traits;
	struct resource *list;
};

extern const struct resource_traits texture_traits;
extern const struct resource_traits sample_traits;

static inline void resource_grab(struct resource *r)
{
	++r->ref_count;
}

static inline void resource_drop(struct resource *r)
{
	--r->ref_count;
}

struct resource_factory *resource_factory(const struct resource_traits *traits);
void resource_factory_destroy(struct resource_factory *rf);
struct resource *resource_factory_create(struct resource_factory *rf, const char *name);
void resource_factory_gcollect(struct resource_factory *rf);
void resource_factory_clear(struct resource_factory *rf);

#endif

