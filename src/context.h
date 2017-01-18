/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_CONTEXT_H_
#define SRC_CONTEXT_H_

#include <tcl.h>

#include <list>

#include "chunk.h"
#include "terraform.h"
#include "main_loop.h"
#include "physics.h"
#include "profile.h"
#include "vertex_buffer.h"
#include "tone_mapper.h"
#include "shader.h"
#include "camera.h"
#include "media.h"
#include "inventory.h"
#include "cli.h"
#include "tcl_commands.h"
#include "roaming_entity.h"

enum {
	MODE_ROAM,
	MODE_COMMAND,
};

class Context {
 public:
	const char *dir;
	MainLoop *ml;
	Tcl_Interp *tcl;
	CommandLine *cli;
	World *w;
	VertexBuffer *shard_vertex_buffer;
	VertexBuffer *obj_vertex_buffer;
	ToneMapper *tone_mapper;
	Shader *shader;
	Camera *cam;
	GLuint tex_terrain;
	GLuint tex_font;
	Space *space;
	Body *player;
	Query cur;
	box3c move;
	Inventory inv;
	std::list<RoamingEntity*> entities;
	char mode;
	char act;
	char use;
	char pick;
	char run;
	char tool;
	v2c rot;
	int chunks_per_tick;
	uint64_t tick;

	void event(const SDL_Event *e);
	void render();
	void update();

	void render_shards();
	void render_cursor();
	void render_block_entities();
	void render_roaming_entities();
	void render_held_item();
	void render_hotbar();
	void render_commandline();
	void render_string(const char *str);
	void render_inventory(const std::vector<Item> &items, const v3ll &p);
	void render_item(int obj, int mat, GLfloat alpha);

	void update_face_lf(std::vector<Vertex> *buf, int64_t x, int64_t y, int64_t z, const v2f *mt, const int *tilted);
	void update_face_rt(std::vector<Vertex> *buf, int64_t x, int64_t y, int64_t z, const v2f *mt, const int *tilted);
	void update_face_dn(std::vector<Vertex> *buf, int64_t x, int64_t y, int64_t z, const v2f *mt, const int *tilted);
	void update_face_up(std::vector<Vertex> *buf, int64_t x, int64_t y, int64_t z, const v2f *mt, const int *tilted);
	void update_face_bk(std::vector<Vertex> *buf, int64_t x, int64_t y, int64_t z, const v2f *mt, const int *tilted);
	void update_face_ft(std::vector<Vertex> *buf, int64_t x, int64_t y, int64_t z, const v2f *mt, const int *tilted);
	void update_cell(std::vector<Vertex> *buf, int64_t x, int64_t y, int64_t z);
	void update_vbo(int id, int64_t x0, int64_t y0, int64_t z0);
	void spill_inventory(const v3ll &p);
	void use_inventory(std::vector<Item> *items);
	void drop_block(const v3ll &p);
	void use_workbench(std::vector<Item> *inv);
	void use_tool();
	void update_player();
	void update_camera();
	void update_chunks();
	void update_entities();
};

unsigned long long next_id(void);
unsigned long long max_id(void);

#define log_info(fmt, ...) \
	fprintf(stderr, "INFO: %s: " fmt "\n", __func__, ##__VA_ARGS__)
#define log_warning(fmt, ...) \
	fprintf(stderr, "WARNING: %s: " fmt "\n", __func__, ##__VA_ARGS__)
#define log_error(fmt, ...) \
	fprintf(stderr, "ERROR: %s: " fmt "\n", __func__, ##__VA_ARGS__)

#endif  // SRC_CONTEXT_H_
