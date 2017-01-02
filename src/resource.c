
#include "resource.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static int create_texture(const char *name, void **data);
static int destroy_texture(void *data);

static int create_sample(const char *name, void **data);
static int destroy_sample(void *data);

const struct resource_traits texture_traits = {
	create_texture,
	destroy_texture,
};

const struct resource_traits sample_traits = {
	create_sample,
	destroy_sample,
};

static int create_texture(const char *name, void **data)
{
	fprintf(stderr, "%s: unimplemented\n", __func__);
	return -1;
}

static int destroy_texture(void *data)
{
	fprintf(stderr, "%s: unimplemented\n", __func__);
	return -1;
}

static int create_sample(const char *name, void **data)
{
	fprintf(stderr, "%s: unimplemented\n", __func__);
	return -1;
}

static int destroy_sample(void *data)
{
	fprintf(stderr, "%s: unimplemented\n", __func__);
	return -1;
}

struct resource_factory *resource_factory(const struct resource_traits *traits)
{
	struct resource_factory *rf = calloc(1, sizeof(*rf));
	rf->traits = traits;
	return rf;
}

void resource_factory_destroy(struct resource_factory *rf)
{
	resource_factory_clear(rf);
	free(rf);
}

struct resource *resource_factory_create(struct resource_factory *rf, const char *name)
{
	char *real_name;
	void *data;
	struct resource *it;
	for (it = rf->list; it != NULL; ++it) {
		if (strcmp(it->name, name) == 0) {
			++it->ref_count;
			return it;
		}
	}
	if (rf->traits->create(name, &data) != 0)
		return NULL;
	it = calloc(1, sizeof(*it));
	it->name = strdup(name);
	it->data = data;
	it->next = rf->list;
	rf->list = it;
	return it;
}

void resource_factory_gcollect(struct resource_factory *rf)
{
	struct resource **itp;
	itp = &rf->list;
	while (*itp != NULL) {
		if ((*itp)->ref_count == 0) {
			free((*itp)->name);
			rf->traits->destroy((*itp)->data);
			free(*itp);
			*itp = (*itp)->next;
		} else {
			itp = &(*itp)->next;
		}
	}
}

void resource_factory_clear(struct resource_factory *rf)
{
	struct resource *it;
	struct resource *next;
	it = rf->list;
	while (it != NULL) {
		next = it->next;
		free(it->name);
		rf->traits->destroy(it->data);
		free(it);
		it = next;
	}
}



