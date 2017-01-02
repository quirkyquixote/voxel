

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include "chunk.h"
#include "terraform.h"
#include "main_loop.h"
#include "profile.h"
#include "renderer.h"

struct context {
	char *dir;
	struct main_loop *ml;
	struct world *w;
	struct profile_manager *prof_mgr;
	struct renderer *shard_renderer;
	int64_t px, py, pz;
	int chunks_per_tick;
};

void render(void *data);
void update(void *data);
void event(const SDL_Event *e, void *data);

int load_all(struct context *ctx);
int save_all(struct context *ctx);

int load_world(struct world *w, const char *dir);
int save_world(struct world *w, const char *dir);

int load_chunk(struct chunk *c, const char *dir);
int save_chunk(struct chunk *c, const char *dir);

int main(int argc, char *argv[])
{
	struct context ctx;

	ctx.dir = "foo";
	ctx.ml = main_loop(30);
	ctx.w = world();
	ctx.prof_mgr = profile_manager();
	ctx.shard_renderer = renderer(SHARDS_PER_WORLD, &vertex3_traits);
	ctx.chunks_per_tick = 8;
	main_loop_on_event(ctx.ml, event, &ctx);
	main_loop_on_update(ctx.ml, update, &ctx);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
//	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 3);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	main_loop_add_window(ctx.ml, window("voxel", 0, 0, 1024, 768, 0));
	window_on_render(ctx.ml->windows, render, &ctx);
	load_all(&ctx);
	main_loop_run(ctx.ml);
	save_all(&ctx);
	world_destroy(ctx.w);
	main_loop_destroy(ctx.ml);
	renderer_destroy(ctx.shard_renderer);
	profile_manager_destroy(ctx.prof_mgr);
	return 0;
}

int load_all(struct context *ctx)
{
	if (load_world(ctx->w, ctx->dir) != 0)
		return -1;
	ctx->px = ctx->w->x + CHUNK_W * CHUNKS_PER_WORLD / 2;
	ctx->py = CHUNK_H / 2;
	ctx->pz = ctx->w->z + CHUNK_W * CHUNKS_PER_WORLD / 2;
	return 0;
}

int save_all(struct context *ctx)
{
	int x, z;
	struct chunk *c;

	if (save_world(ctx->w, ctx->dir) != 0)
		return -1;
	for (x = 0; x < CHUNKS_PER_WORLD; ++x)
		for (z = 0; z < CHUNKS_PER_WORLD; ++z)
			save_chunk(ctx->w->chunks[x][z], ctx->dir);
	return 0;
}

int load_world(struct world *w, const char *dir)
{
	union sz_tag root;
	char path[256];
	int fd;

	snprintf(path, sizeof(path), "%s/world.dat", dir);
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

int save_world(struct world *w, const char *dir)
{
	union sz_tag *root;
	char path[256];
	int fd;

	if (world_save(w, &root) != 0)
		return -1;
	snprintf(path, sizeof(path), "%s/world.dat", dir);
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

int load_chunk(struct chunk *c, const char *dir)
{
	char path[256];
	int fd;
	union sz_tag root;

	snprintf(path, sizeof(path), "%s/%06x%06x.dat", dir, c->x / CHUNK_W, c->z / CHUNK_D);
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

int save_chunk(struct chunk *c, const char *dir)
{
	char path[256];
	int fd;
	union sz_tag *root;

	if (c->loaded == 0)
		return -1;
	if (chunk_save(c, &root) != 0)
		return -1;
	snprintf(path, sizeof(path), "%s/%06x%06x.dat", dir, c->x / CHUNK_W, c->z / CHUNK_D);
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

void update_vbo(struct context *ctx, int id, int64_t x0, int64_t y0, int64_t z0)
{
	int64_t x1, y1, z1;
	int64_t x, y, z;
	struct vertex3_buf *buf;
	int8_t m, l, d, b;

	x1 = x0 + SHARD_W;
	y1 = y0 + SHARD_H;
	z1 = z0 + SHARD_D;
	buf = vertex3_buf();

	for (x = x0; x < x1; ++x) {
		for (y = y0; y < y1; ++y) {
			for (z = z0; z < z1; ++z) {
				m = WORLD_AT(ctx->w, mat, x, y, z);
				l = WORLD_AT(ctx->w, mat, x - 1, y, z);
				d = y == 0 ? 0 : WORLD_AT(ctx->w, mat, x, y - 1, z);
				b = WORLD_AT(ctx->w, mat, x, y, z - 1);
				if (m == 0) {
					if (l != 0)
						vertex3_buf_right(buf, x - 1, y, z);
					if (d != 0)
						vertex3_buf_up(buf, x, y - 1, z);
					if (b != 0)
						vertex3_buf_front(buf, x, y, z - 1);
				} else {
					if (l == 0)
						vertex3_buf_left(buf, x, y, z);
					if (d == 0)
						vertex3_buf_down(buf, x, y, z);
					if (b == 0)
						vertex3_buf_back(buf, x, y, z);
				}
			}
		}
	}

	renderer_update(ctx->shard_renderer, id, buf->data, buf->size);
	vertex3_buf_destroy(buf);
}

void update_chunks(struct context *ctx)
{
	int x, z, i, j, k;
	struct chunk *c;
	struct chunk *unloaded[CHUNKS_PER_WORLD * CHUNKS_PER_WORLD];

	i = 0;
	for (x = 0; x < CHUNKS_PER_WORLD; ++x) {
		for (z = 0; z < CHUNKS_PER_WORLD; ++z) {
			c = ctx->w->chunks[x][z];
			if (c->loaded == 0) {
				c->x = x * CHUNK_W;
				c->z = z * CHUNK_D;
				c->priority = hypot((double)c->x - (double)ctx->px,
						(double)c->z - (double)ctx->pz);
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
		if (load_chunk(c, ctx->dir) != 0) {
			fprintf(stdout, "; from scratch");
			terraform(0, c);
		} else {
			fprintf(stdout, "; from file");
		}
		for (k = 0; k < SHARDS_PER_CHUNK; ++k) {
			update_vbo(ctx, c->shards[k]->id, c->x, k * SHARD_H, c->z);
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
			profile_manager_reset(ctx->prof_mgr);
		}
	} else if (e->type == SDL_KEYUP) {
		if (e->key.repeat) {
			return;
		} else if (e->key.keysym.sym == SDLK_o) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		} else if (e->key.keysym.sym == SDLK_p) {
			fprintf(stdout, "=== PROFILE DUMP ===\n");
			profile_manager_dump(ctx->prof_mgr);
		}
	}
}

