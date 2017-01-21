/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "profile.h"

#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GL_GLEXT_PROTOTYPES 1
#include "SDL2/SDL_opengl.h"
#include "GL/glext.h"
#undef GL_GLEXT_PROTOTYPES

static void cpu_profile_init(struct profile *p);
static void cpu_profile_deinit(struct profile *p);
static void cpu_profile_begin(struct profile *p);
static void cpu_profile_end(struct profile *p);
static void cpu_profile_update(struct profile *p);

static void gpu_profile_init(struct profile *p);
static void gpu_profile_deinit(struct profile *p);
static void gpu_profile_begin(struct profile *p);
static void gpu_profile_end(struct profile *p);
static void gpu_profile_update(struct profile *p);

const struct profile_traits cpu_profile = {
	cpu_profile_init,
	cpu_profile_deinit,
	cpu_profile_begin,
	cpu_profile_end,
	cpu_profile_update,
};

const struct profile_traits gpu_profile = {
	gpu_profile_init,
	gpu_profile_deinit,
	gpu_profile_begin,
	gpu_profile_end,
	gpu_profile_update,
};

static inline uint64_t usec()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (uint64_t) tv.tv_usec + (uint64_t) tv.tv_sec * 1000000;
}

struct profile *profile(const char *tag, const struct profile_traits *traits)
{
	struct profile *p = calloc(1, sizeof(*p));
	p->tag = strdup(tag);
	p->traits = traits;
	p->traits->init(p);
	return p;
}

void profile_destroy(struct profile *p)
{
	p->traits->deinit(p);
	free(p);
}

void profile_reset(struct profile *p)
{
	p->acc = 0;
	p->samples = 0;
}

void profile_dump(struct profile *p)
{
	fprintf(stdout, "%28s", p->tag);
	fprintf(stdout, "%8u", p->acc);
	fprintf(stdout, "%8u", p->samples);
	fprintf(stdout, "%8u", p->acc / p->samples);
	fprintf(stdout, "\n");
}

void cpu_profile_init(struct profile *p)
{
}

void cpu_profile_deinit(struct profile *p)
{
}

void cpu_profile_begin(struct profile *p)
{
	p->cpu = usec();
}

void cpu_profile_end(struct profile *p)
{
	p->acc += usec() - p->cpu;
	++p->samples;
}

void cpu_profile_update(struct profile *p)
{
}

void gpu_profile_init(struct profile *p)
{
	glGenQueries(1, &p->gpu);
}

void gpu_profile_deinit(struct profile *p)
{
	glDeleteQueries(1, &p->gpu);
}

void gpu_profile_begin(struct profile *p)
{
	glBeginQuery(GL_TIME_ELAPSED, p->gpu);
}

void gpu_profile_end(struct profile *p)
{
	glEndQuery(GL_TIME_ELAPSED);
}

void gpu_profile_update(struct profile *p)
{
	GLint done;
	GLuint dt;
	do {
		glGetQueryObjectiv(p->gpu, GL_QUERY_RESULT_AVAILABLE, &done);
	} while (!done);

	glGetQueryObjectuiv(p->gpu, GL_QUERY_RESULT, &dt);
	p->acc += dt / 1000;
	++p->samples;
}


struct profile_manager *profile_manager(void)
{
	struct profile_manager *pm = calloc(1, sizeof(*pm));
	return pm;
}

void profile_manager_destroy(struct profile_manager *pm)
{
	struct profile *p, *next;
	p = pm->first;
	while (p != NULL) {
		next = p->next;
		profile_destroy(p);
		p = next;
	}
}

void profile_manager_add(struct profile_manager *pm, struct profile *p)
{
	if (pm->first)
		pm->first = p;
	else
		pm->last->next = p;
	pm->last = p;
}

void profile_manager_update(struct profile_manager *pm)
{
	struct profile *p;
	for (p = pm->first; p != NULL; p = p->next)
		p->traits->update(p);
}

void profile_manager_reset(struct profile_manager *pm)
{
	struct profile *p;
	for (p = pm->first; p != NULL; p = p->next)
		profile_reset(p);
}

void profile_manager_dump(struct profile_manager *pm)
{
	struct profile *p;
	for (p = pm->first; p != NULL; p = p->next)
		profile_dump(p);
}

