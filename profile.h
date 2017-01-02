

#ifndef VOXEL_PROFILE_H_
#define VOXEL_PROFILE_H_

#include <stdint.h>

struct profile;

struct profile_traits {
	void(*init)(struct profile *);
	void(*deinit)(struct profile *);
	void(*begin)(struct profile *);
	void(*end)(struct profile *);
	void(*update)(struct profile *);
};

struct profile {
	const struct profile_traits *traits;
	struct profile *next;
	const char *tag;
	uint64_t acc;
	uint64_t samples;
	union {
		uint64_t cpu;
		unsigned int gpu;
	};
};

struct profile_manager {
	struct profile *first;
	struct profile *last;
};

const struct profile_traits cpu_profile;
const struct profile_traits gpu_profile;

struct profile *profile(const char *name, const struct profile_traits *traits);
void profile_destroy(struct profile *p);
void profile_begin(struct profile *p);
void profile_end(struct profile *p);
void profile_reset(struct profile *p);
void profile_dump(struct profile *p);

struct profile_manager *profile_manager(void);
void profile_manager_destroy(struct profile_manager *pm);
void profile_manager_add(struct profile_manager *pm, struct profile *p);
void profile_manager_remove(struct profile_manager *pm, struct profile *p);
void profile_manager_update(struct profile_manager *pm);
void profile_manager_reset(struct profile_manager *pm);
void profile_manager_dump(struct profile_manager *pm);


#endif

