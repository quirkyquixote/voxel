/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_RENDERER_H_
#define SRC_RENDERER_H_

#include "vertex_buffer.h"
#include "tone_mapper.h"
#include "shader.h"
#include "camera.h"
#include "media.h"
#include "callback.h"

class Context;

class Renderer {
 public:
	explicit Renderer(Context *ctx);
	~Renderer();

	void operator()();

	inline Camera *get_cam() { return cam; }

	void render_shards();
	void render_block_entities();
	void render_roaming_entities();
	void render_commandline();
	void render_string(const char *str);
	void render_inventory(const std::vector<Item> &items, const v3ll &p);
	void render_board(const std::vector<Item> &inv, const v3ll &p);
	void render_item(int obj, int mat, GLfloat alpha);

	void update_face_lf(std::vector<Vertex> *buf, int64_t x, int64_t y, int64_t z, const v2f *mt, const int *tilted);
	void update_face_rt(std::vector<Vertex> *buf, int64_t x, int64_t y, int64_t z, const v2f *mt, const int *tilted);
	void update_face_dn(std::vector<Vertex> *buf, int64_t x, int64_t y, int64_t z, const v2f *mt, const int *tilted);
	void update_face_up(std::vector<Vertex> *buf, int64_t x, int64_t y, int64_t z, const v2f *mt, const int *tilted);
	void update_face_bk(std::vector<Vertex> *buf, int64_t x, int64_t y, int64_t z, const v2f *mt, const int *tilted);
	void update_face_ft(std::vector<Vertex> *buf, int64_t x, int64_t y, int64_t z, const v2f *mt, const int *tilted);
	void update_cell(std::vector<Vertex> *buf, int64_t x, int64_t y, int64_t z);
	void update_shard(int id, int64_t x0, int64_t y0, int64_t z0);
	void update_camera();

	inline void add_callback(Callback *cb) { callback_list.push_back(cb); }

 private:
	Context *ctx;
	VertexBuffer *shard_vertex_buffer;
	VertexBuffer *obj_vertex_buffer;
	ToneMapper *tone_mapper;
	Shader *shader;
	Camera *cam;
	GLuint tex_terrain;
	GLuint tex_font;
	List<Callback> callback_list;
};

#endif  // SRC_RENDERER_H_

