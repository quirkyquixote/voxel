

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

#include "chunk.h"
#include "terraform.h"
#include "main_loop.h"

struct context {
	struct main_loop *ml;
	struct world *w;
};

void render(void *data);
void update(void *data);
void event(const SDL_Event *e, void *data);

int main(int argc, char *argv[])
{
	struct context ctx;
	union sz_tag *root;
	int fd;

	ctx.ml = main_loop(30);
	main_loop_on_event(ctx.ml, event, &ctx);
	main_loop_on_update(ctx.ml, update, &ctx);
	main_loop_add_window(ctx.ml, window("voxel", 0, 0, 1024, 768, 0));
	window_on_render(ctx.ml->windows, render, &ctx);
	main_loop_run(ctx.ml);

	ctx.w = world();
	terraform(0, 0, 0, ctx.w->chunks[0][0]);

	if (world_save(ctx.w, &root) != 0)
		return -1;
	fd = creat("world.dat", 0600);
	if (fd < 0) {
		perror("world");
		return -1;
	}
	if (sz_write(fd, root) != 0)
		return -1;
	close(fd);
	sz_destroy(root);

	if (chunk_save(ctx.w->chunks[0][0], &root) != 0)
		return -1;
	fd = creat("chunk.dat", 0600);
	if (fd < 0) {
		perror("world");
		return -1;
	}
	if (sz_write(fd, root) != 0)
		return -1;
	close(fd);
	sz_destroy(root);

	world_destroy(ctx.w);
	main_loop_destroy(ctx.ml);
	return 0;
}

void render(void *data)
{
	struct context *ctx = data;
}

void update(void *data)
{
	struct context *ctx = data;
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

