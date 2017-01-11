

#include "context.h"

#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include "lighting.h"
#include "render.h"
#include "update.h"
#include "event.h"

const char *obj_names[] = {
	"block", "slab", "stairs", "pane", "workbench", "crate", "fluid", "pipe",
	"obj08", "obj09", "obj0A", "obj0B", "obj0C", "obj0D", "obj0E", "obj0F",
	"obj10", "obj11", "obj12", "obj13", "obj14", "obj15", "obj16", "obj17",
	"obj18", "obj19", "obj1A", "obj1B", "obj1C", "obj1D", "obj1E", "obj1F",
	"obj20", "obj21", "obj22", "obj23", "obj24", "obj25", "obj26", "obj27",
	"obj28", "obj29", "obj2A", "obj2B", "obj2C", "obj2D", "obj2E", "obj2F",
	"obj30", "obj31", "obj32", "obj33", "obj34", "obj35", "obj36", "obj37",
	"obj38", "obj39", "obj3A", "obj3B", "obj3C", "obj3D", "obj3E", "obj3F",
	"obj40", "obj41", "obj42", "obj43", "obj44", "obj45", "obj46", "obj47",
	"obj48", "obj49", "obj4A", "obj4B", "obj4C", "obj4D", "obj4E", "obj4F",
	"obj50", "obj51", "obj52", "obj53", "obj54", "obj55", "obj56", "obj57",
	"obj58", "obj59", "obj5A", "obj5B", "obj5C", "obj5D", "obj5E", "obj5F",
	"obj60", "obj61", "obj62", "obj63", "obj64", "obj65", "obj66", "obj67",
	"obj68", "obj69", "obj6A", "obj6B", "obj6C", "obj6D", "obj6E", "obj6F",
	"obj70", "obj71", "obj72", "obj73", "obj74", "obj75", "obj76", "obj77",
	"obj78", "obj79", "obj7A", "obj7B", "obj7C", "obj7D", "obj7E", "obj7F",
	"obj80", "obj81", "obj82", "obj83", "obj84", "obj85", "obj86", "obj87",
	"obj88", "obj89", "obj8A", "obj8B", "obj8C", "obj8D", "obj8E", "obj8F",
	"obj90", "obj91", "obj92", "obj93", "obj94", "obj95", "obj96", "obj97",
	"obj98", "obj99", "obj9A", "obj9B", "obj9C", "obj9D", "obj9E", "obj9F",
	"objA0", "objA1", "objA2", "objA3", "objA4", "objA5", "objA6", "objA7",
	"objA8", "objA9", "objAA", "objAB", "objAC", "objAD", "objAE", "objAF",
	"objB0", "objB1", "objB2", "objB3", "objB4", "objB5", "objB6", "objB7",
	"objB8", "objB9", "objBA", "objBB", "objBC", "objBD", "objBE", "objBF",
	"objC0", "objC1", "objC2", "objC3", "objC4", "objC5", "objC6", "objC7",
	"objC8", "objC9", "objCA", "objCB", "objCC", "objCD", "objCE", "objCF",
	"objD0", "objD1", "objD2", "objD3", "objD4", "objD5", "objD6", "objD7",
	"objD8", "objD9", "objDA", "objDB", "objDC", "objDD", "objDE", "objDF",
	"objE0", "objE1", "objE2", "objE3", "objE4", "objE5", "objE6", "objE7",
	"objE8", "objE9", "objEA", "objEB", "objEC", "objED", "objEE", "objEF",
	"objF0", "objF1", "objF2", "objF3", "objF4", "objF5", "objF6", "objF7",
	"objF8", "objF9", "objFA", "objFB", "objFC", "objFD", "objFE", "objFF"
};

const char *mat_names[] = {
	"mat00", "mat01", "mat02", "mat03", "mat04", "mat05", "mat06", "mat07",
	"mat08", "mat09", "mat0A", "mat0B", "mat0C", "mat0D", "mat0E", "mat0F",
	"mat10", "mat11", "mat12", "mat13", "mat14", "mat15", "mat16", "mat17",
	"mat18", "mat19", "mat1A", "mat1B", "mat1C", "mat1D", "mat1E", "mat1F",
	"mat20", "mat21", "mat22", "mat23", "mat24", "mat25", "mat26", "mat27",
	"mat28", "mat29", "mat2A", "mat2B", "mat2C", "mat2D", "mat2E", "mat2F",
	"mat30", "mat31", "mat32", "mat33", "mat34", "mat35", "mat36", "mat37",
	"mat38", "mat39", "mat3A", "mat3B", "mat3C", "mat3D", "mat3E", "mat3F",
	"mat40", "mat41", "mat42", "mat43", "mat44", "mat45", "mat46", "mat47",
	"mat48", "mat49", "mat4A", "mat4B", "mat4C", "mat4D", "mat4E", "mat4F",
	"mat50", "mat51", "mat52", "mat53", "mat54", "mat55", "mat56", "mat57",
	"mat58", "mat59", "mat5A", "mat5B", "mat5C", "mat5D", "mat5E", "mat5F",
	"mat60", "mat61", "mat62", "mat63", "mat64", "mat65", "mat66", "mat67",
	"mat68", "mat69", "mat6A", "mat6B", "mat6C", "mat6D", "mat6E", "mat6F",
	"mat70", "mat71", "mat72", "mat73", "mat74", "mat75", "mat76", "mat77",
	"mat78", "mat79", "mat7A", "mat7B", "mat7C", "mat7D", "mat7E", "mat7F",
	"mat80", "mat81", "mat82", "mat83", "mat84", "mat85", "mat86", "mat87",
	"mat88", "mat89", "mat8A", "mat8B", "mat8C", "mat8D", "mat8E", "mat8F",
	"mat90", "mat91", "mat92", "mat93", "mat94", "mat95", "mat96", "mat97",
	"mat98", "mat99", "mat9A", "mat9B", "mat9C", "mat9D", "mat9E", "mat9F",
	"matA0", "matA1", "matA2", "matA3", "matA4", "matA5", "matA6", "matA7",
	"matA8", "matA9", "matAA", "matAB", "matAC", "matAD", "matAE", "matAF",
	"matB0", "matB1", "matB2", "matB3", "matB4", "matB5", "matB6", "matB7",
	"matB8", "matB9", "matBA", "matBB", "matBC", "matBD", "matBE", "matBF",
	"matC0", "matC1", "matC2", "matC3", "matC4", "matC5", "matC6", "matC7",
	"matC8", "matC9", "matCA", "matCB", "matCC", "matCD", "matCE", "matCF",
	"matD0", "matD1", "matD2", "matD3", "matD4", "matD5", "matD6", "matD7",
	"matD8", "matD9", "matDA", "matDB", "matDC", "matDD", "matDE", "matDF",
	"matE0", "matE1", "matE2", "matE3", "matE4", "matE5", "matE6", "matE7",
	"matE8", "matE9", "matEA", "matEB", "matEC", "matED", "matEE", "matEF",
	"matF0", "matF1", "matF2", "matF3", "matF4", "matF5", "matF6", "matF7",
	"matF8", "matF9", "matFA", "matFB", "matFC", "matFD", "matFE", "foo"
};

const char *shape_names[] = {
	"none",
	"block_dn", "block_up", "block_lf", "block_rt", "block_bk", "block_ft",
	"slab_dn", "slab_up", "slab_lf", "slab_rt", "slab_bk", "slab_ft",
	"pane_x", "pane_y", "pane_z",
	"stairs_df", "stairs_dl", "stairs_db", "stairs_dr",
	"stairs_uf", "stairs_ul", "stairs_ub", "stairs_ur",
	"workbench", "crate",
	"flowsim1", "flowsim2", "flowsim3", "flowsim4", "flowsim5", "flowsim6", "flowsim7",
	"flowsim8", "flowsim9", "flowsim10", "flowsim11", "flowsim12", "flowsim13",
	"flowsim14", "flowsim15", "flowsim16",
        "pipe_x", "pipe_y", "pipe_z",
	"sha2D", "sha2E", "sha2F",
	"sha30", "sha31", "sha32", "sha33", "sha34", "sha35", "sha36", "sha37",
	"sha38", "sha39", "sha3A", "sha3B", "sha3C", "sha3D", "sha3E", "sha3F",
	"sha40", "sha41", "sha42", "sha43", "sha44", "sha45", "sha46", "sha47",
	"sha48", "sha49", "sha4A", "sha4B", "sha4C", "sha4D", "sha4E", "sha4F",
	"sha50", "sha51", "sha52", "sha53", "sha54", "sha55", "sha56", "sha57",
	"sha58", "sha59", "sha5A", "sha5B", "sha5C", "sha5D", "sha5E", "sha5F",
	"sha60", "sha61", "sha62", "sha63", "sha64", "sha65", "sha66", "sha67",
	"sha68", "sha69", "sha6A", "sha6B", "sha6C", "sha6D", "sha6E", "sha6F",
	"sha70", "sha71", "sha72", "sha73", "sha74", "sha75", "sha76", "sha77",
	"sha78", "sha79", "sha7A", "sha7B", "sha7C", "sha7D", "sha7E", "sha7F",
	"sha80", "sha81", "sha82", "sha83", "sha84", "sha85", "sha86", "sha87",
	"sha88", "sha89", "sha8A", "sha8B", "sha8C", "sha8D", "sha8E", "sha8F",
	"sha90", "sha91", "sha92", "sha93", "sha94", "sha95", "sha96", "sha97",
	"sha98", "sha99", "sha9A", "sha9B", "sha9C", "sha9D", "sha9E", "sha9F",
	"shaA0", "shaA1", "shaA2", "shaA3", "shaA4", "shaA5", "shaA6", "shaA7",
	"shaA8", "shaA9", "shaAA", "shaAB", "shaAC", "shaAD", "shaAE", "shaAF",
	"shaB0", "shaB1", "shaB2", "shaB3", "shaB4", "shaB5", "shaB6", "shaB7",
	"shaB8", "shaB9", "shaBA", "shaBB", "shaBC", "shaBD", "shaBE", "shaBF",
	"shaC0", "shaC1", "shaC2", "shaC3", "shaC4", "shaC5", "shaC6", "shaC7",
	"shaC8", "shaC9", "shaCA", "shaCB", "shaCC", "shaCD", "shaCE", "shaCF",
	"shaD0", "shaD1", "shaD2", "shaD3", "shaD4", "shaD5", "shaD6", "shaD7",
	"shaD8", "shaD9", "shaDA", "shaDB", "shaDC", "shaDD", "shaDE", "shaDF",
	"shaE0", "shaE1", "shaE2", "shaE3", "shaE4", "shaE5", "shaE6", "shaE7",
	"shaE8", "shaE9", "shaEA", "shaEB", "shaEC", "shaED", "shaEE", "shaEF",
	"shaF0", "shaF1", "shaF2", "shaF3", "shaF4", "shaF5", "shaF6", "shaF7",
	"shaF8", "shaF9", "shaFA", "shaFB", "shaFC", "shaFD", "shaFE", "shaFF"
};

const char *face_names[] = {
	"front",
	"left",
	"back",
	"right",
	"up",
	"down",
};

int load_all(struct context *ctx);
int save_all(struct context *ctx);

int load_world(struct world *w, const char *dir);
int save_world(struct world *w, const char *dir);

int load_chunk(struct chunk *c, const char *dir);
int save_chunk(struct chunk *c, const char *dir);

int main(int argc, char *argv[])
{
	struct context *ctx;

	ctx = calloc(1, sizeof(*ctx));
	ctx->dir = "foo";
	ctx->w = world();
	ctx->prof_mgr = profile_manager();
	ctx->chunks_per_tick = 1;

	/* Setup main loop */
	ctx->ml = main_loop(30);
	main_loop_set_event_callback(ctx->ml, event, ctx);
	main_loop_set_update_callback(ctx->ml, update, ctx);
	main_loop_add_window(ctx->ml, window("voxel", 0, 0, 1280, 768, 0));
	window_set_render_callback(ctx->ml->windows, render, ctx);

	/* Load textures */
	ctx->tex_terrain = texture("data/gradient.png");

	/* Initialize physics */
	ctx->space = space(ctx->w);
	space_set_gravity(ctx->space, v3f(0, -0.05, 0));
	space_set_iterations(ctx->space, 2);
	space_set_impulse(ctx->space, 0.001);
	space_set_terminal_speed(ctx->space, 1);

	/* Initialize flowsim */
	ctx->flowsim = flowsim(ctx->w);

	/* Setup camera */
	ctx->cam = camera();
	camera_set_max_distance(ctx->cam, 1024);
	camera_set_aspect_ratio(ctx->cam, 1280.0 / 768.0);

	/* Create vertex_buffers */
	ctx->shard_vertex_buffer = vertex_buffer(SHARDS_PER_WORLD);
	ctx->tone_mapper = tone_mapper(1. / 30., 16);
	ctx->shader = shader("data/shader.vert", "data/shader.frag");

	/* Load world */
	load_all(ctx);

	/* Run */
	main_loop_run(ctx->ml);

	/* Save the world */
	save_all(ctx);

	/* Destroy everything */
	world_destroy(ctx->w);
	main_loop_destroy(ctx->ml);
	vertex_buffer_destroy(ctx->shard_vertex_buffer);
	tone_mapper_destroy(ctx->tone_mapper);
	profile_manager_destroy(ctx->prof_mgr);
	free(ctx);
	return 0;
}

int load_all(struct context *ctx)
{
	int x, z;
	struct chunk *c;
	struct v3f p;
	int from_scratch;

	from_scratch = load_world(ctx->w, ctx->dir);
	for (x = 0; x < CHUNKS_PER_WORLD; ++x) {
		for (z = 0; z < CHUNKS_PER_WORLD; ++z) {
			c = ctx->w->chunks[x][z];
			c->x = ctx->w->x + x * CHUNK_W;
			c->z = ctx->w->z + z * CHUNK_D;
			if (load_chunk(c, ctx->dir) != 0) {
				terraform(0, c);
				c->flags |= CHUNK_UNLIT;
			}
		}
	}
	if (from_scratch)
		update_lighting(ctx->w, aab3ll(0, 0, 0, WORLD_W, WORLD_H, WORLD_D), NULL);
	p.x = ctx->w->x + CHUNK_W * CHUNKS_PER_WORLD / 2;
	p.y = CHUNK_H;
	p.z = ctx->w->z + CHUNK_W * CHUNKS_PER_WORLD / 2;
	ctx->player = body(ctx->space);
	body_set_position(ctx->player, p);
	body_set_size(ctx->player, v2f(0.325, 0.825));
	body_set_step_size(ctx->player, 1);
	ctx->inv = inventory(9);
	ctx->tool = 0;
	for (z = 255; z < 256; ++z) {
		for (x = 0; x < OBJ_COUNT; ++x) {
			inventory_add(ctx->inv, x, z, 64);
		}
	}
	list_init(&ctx->drops);
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
	union sz_tag *root;
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
	if (world_load(w, root) != 0)
		return -1;
	sz_destroy(root);
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
	union sz_tag *root;

	snprintf(path, sizeof(path), "%s/%06x%06x.dat", dir, c->x / CHUNK_W, c->z / CHUNK_D);
	fd = open(path, O_RDONLY, 0400);
	if (fd < 0)
		return -1;
	if (sz_read(fd, &root) != 0)
		return -1;
	close(fd);
	if (chunk_load(c, root) != 0)
		return -1;
	sz_destroy(root);
	return 0;
}

int save_chunk(struct chunk *c, const char *dir)
{
	char path[256];
	int fd;
	union sz_tag *root;

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

static unsigned long long id = 0;

unsigned long long next_id(void)
{
	return id++;
}

unsigned long long max_id(void)
{
	return id;
}

