

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include "chunk.h"
#include "terraform.h"
#include "main_loop.h"

struct context {
	struct main_loop *ml;
	struct world *w;
	int chunks_per_tick;
};

void render(void *data);
void update(void *data);
void event(const SDL_Event *e, void *data);

int load_all(struct context *ctx);
int save_all(struct context *ctx);

int load_world(struct world *w);
int save_world(struct world *w);

int load_chunk(struct chunk *c);
int save_chunk(struct chunk *c);

int main(int argc, char *argv[])
{
	struct context ctx;

	ctx.ml = main_loop(30);
	ctx.w = world();
	ctx.chunks_per_tick = 8;
	main_loop_on_event(ctx.ml, event, &ctx);
	main_loop_on_update(ctx.ml, update, &ctx);
	main_loop_add_window(ctx.ml, window("voxel", 0, 0, 1024, 768, 0));
	window_on_render(ctx.ml->windows, render, &ctx);
	load_all(&ctx);
	main_loop_run(ctx.ml);
	save_all(&ctx);
	world_destroy(ctx.w);
	main_loop_destroy(ctx.ml);
	return 0;
}

int load_all(struct context *ctx)
{
	if (load_world(ctx->w) != 0)
		return -1;
	return 0;
}

int save_all(struct context *ctx)
{
	int x, z;
	struct chunk *c;

	if (save_world(ctx->w) != 0)
		return -1;
	for (x = 0; x < CHUNKS_PER_WORLD; ++x)
		for (z = 0; z < CHUNKS_PER_WORLD; ++z)
			save_chunk(ctx->w->chunks[x][z]);
	return 0;
}

int load_world(struct world *w)
{
	union sz_tag root;
	char path[256];
	int fd;

	snprintf(path, sizeof(path), "world.dat");
	fd = open(path, O_RDONLY, 0400);
	if (fd < 0) {
		perror(path);
		return -1;
	}
	if (sz_read(fd, &root) != 0)
		return -1;
	close(fd);
	if (world_load(w, &root) != 0)
		return -1;
	//sz_destroy(root);
	return 0;
}

int save_world(struct world *w)
{
	union sz_tag *root;
	char path[256];
	int fd;

	if (world_save(w, &root) != 0)
		return -1;
	snprintf(path, sizeof(path), "world.dat");
	fd = creat(path, 0600);
	if (fd < 0) {
		perror(path);
		return -1;
	}
	if (sz_write(fd, root) != 0)
		return -1;
	close(fd);
	sz_destroy(root);
	return 0;
}

int load_chunk(struct chunk *c)
{
	char path[256];
	int fd;
	union sz_tag root;

	snprintf(path, sizeof(path), "%06x%06x.dat", c->x / CHUNK_W, c->z / CHUNK_D);
	fd = open(path, O_RDONLY, 0400);
	if (fd < 0)
		return -1;
	if (sz_read(fd, &root) != 0)
		return -1;
	close(fd);
	if (chunk_load(c, &root) != 0)
		return -1;
	//	sz_destroy(root);
	return 0;
}

int save_chunk(struct chunk *c)
{
	char path[256];
	int fd;
	union sz_tag *root;

	if (c->loaded == 0)
		return -1;
	if (chunk_save(c, &root) != 0)
		return -1;
	snprintf(path, sizeof(path), "%06x%06x.dat", c->x / CHUNK_W, c->z / CHUNK_D);
	fd = creat(path, 0600);
	if (fd < 0) {
		perror(path);
		return -1;
	}
	if (sz_write(fd, root) != 0)
		return -1;
	close(fd);
	sz_destroy(root);
	return 0;
}

void render(void *data)
{
	struct context *ctx = data;
}

int chunks_by_priority(const void *p1, const void *p2)
{
	const struct chunk *c1, *c2;
	c1 = *(const void **)p1;
	c2 = *(const void **)p2;
	return c1->priority - c2->priority;
}

void update_chunks(struct context *ctx)
{
	int x, z, i, j;
	struct chunk *c;
	struct chunk *unloaded[CHUNKS_PER_WORLD * CHUNKS_PER_WORLD];

	i = 0;
	for (x = 0; x < CHUNKS_PER_WORLD; ++x) {
		for (z = 0; z < CHUNKS_PER_WORLD; ++z) {
			c = ctx->w->chunks[x][z];
			if (c->loaded == 0) {
				c->x = x * CHUNK_W;
				c->z = z * CHUNK_D;
				c->priority = c->x + c->z;
				unloaded[i++] = c;
			}
		}
	}
	if (i == 0)
		return;
	qsort(unloaded, i, sizeof(*unloaded), chunks_by_priority);
	i = i < ctx->chunks_per_tick ? i : ctx->chunks_per_tick;
	for (j = 0; j < i; ++j) {
		c = unloaded[j];
		fprintf(stdout, "Update chunk %d,%d; priority:%d", c->x, c->z, c->priority);
		if (load_chunk(c) != 0) {
			fprintf(stdout, "; from scratch");
			terraform(0, c);
		} else {
			fprintf(stdout, "; from file");
		}
		fprintf(stdout, "\n");
		c->loaded = 1;
	}
}

void update(void *data)
{
	struct context *ctx = data;
	update_chunks(ctx);
}

void event(const SDL_Event *e, void *data)
{
	struct context *ctx = data;

	if (e->type == SDL_KEYDOWN) {
		if (e->key.repeat) {
			return;
		} else if (e->key.keysym.sym == SDLK_ESCAPE) {
			main_loop_kill(ctx->ml);
		} else if (e->key.keysym.sym == SDLK_o) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		} else if (e->key.keysym.sym == SDLK_p) {
			fprintf(stdout, "=== PROFILE START ===\n");
			//			profile_reset(ctx->prof);
		}
	} else if (e->type == SDL_KEYUP) {
		if (e->key.repeat) {
			return;
		} else if (e->key.keysym.sym == SDLK_o) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		} else if (e->key.keysym.sym == SDLK_p) {
			fprintf(stdout, "=== PROFILE DUMP ===\n");
			//			profile_dump(ctx->prof);
		}
	}
}

