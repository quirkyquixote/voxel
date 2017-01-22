/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_VERTEX_BUFFER_H_
#define SRC_VERTEX_BUFFER_H_

#include <vector>

#define GL_GLEXT_PROTOTYPES 1
#include "SDL2/SDL_opengl.h"
#include "GL/glext.h"
#undef GL_GLEXT_PROTOTYPES

#include "v2.h"
#include "v3.h"
#include "v4.h"
#include "box2.h"
#include "types.h"

struct Vertex {
	GLfloat x, y, z;
	GLfloat u, v;
	GLfloat r, g, b, a;
};

struct VertexDesc {
	GLfloat x, y, z, c;
	int face;
};

class VertexBuffer {
 public:
	explicit VertexBuffer(int nbufs);
	~VertexBuffer();

	void enable();
	void draw(GLenum mode, size_t buf);
	void draw_slice(GLenum mode, size_t buf, size_t first, size_t count);
	void disable();
	void update(size_t buf, const void *data, size_t size);

	inline size_t get_count() const { return vbo_count; }
	inline GLuint get_name(int i) const { return vbo_names[i]; }
	inline size_t get_size(int i) const { return vbo_sizes[i]; }

 private:
	size_t vbo_count;
	GLuint *vbo_names;
	size_t *vbo_sizes;
};

extern v2f texcoord_from_mat[256][3];

extern VertexDesc vertices_face_dn[];
extern VertexDesc vertices_face_up[];
extern VertexDesc vertices_face_lf[];
extern VertexDesc vertices_face_rt[];
extern VertexDesc vertices_face_bk[];
extern VertexDesc vertices_face_ft[];

extern VertexDesc vertices_slab_dn[];
extern VertexDesc vertices_slab_up[];
extern VertexDesc vertices_slab_lf[];
extern VertexDesc vertices_slab_rt[];
extern VertexDesc vertices_slab_bk[];
extern VertexDesc vertices_slab_ft[];
extern VertexDesc vertices_stairs_dl[];
extern VertexDesc vertices_stairs_dr[];
extern VertexDesc vertices_stairs_db[];
extern VertexDesc vertices_stairs_df[];
extern VertexDesc vertices_stairs_ul[];
extern VertexDesc vertices_stairs_ur[];
extern VertexDesc vertices_stairs_ub[];
extern VertexDesc vertices_stairs_uf[];
extern VertexDesc vertices_pane_x[];
extern VertexDesc vertices_pane_y[];
extern VertexDesc vertices_pane_z[];
extern VertexDesc vertices_pane_dn[];
extern VertexDesc vertices_pane_up[];
extern VertexDesc vertices_pane_lf[];
extern VertexDesc vertices_pane_rt[];
extern VertexDesc vertices_pane_bk[];
extern VertexDesc vertices_pane_ft[];

extern VertexDesc vertices_token_lf[];
extern VertexDesc vertices_token_bk[];
extern VertexDesc vertices_token_rt[];
extern VertexDesc vertices_token_ft[];

void vertices_add(std::vector<Vertex> *s, const VertexDesc *buf,
		size_t len, v3f p, v2f t1, const v2f *t2, const int *tilted);

void texcoord_up(int mat, v2f *ret, int *tilted);
void texcoord_dn(int mat, v2f *ret, int *tilted);
void texcoord_lf(int mat, v2f *ret, int *tilted);
void texcoord_rt(int mat, v2f *ret, int *tilted);
void texcoord_bk(int mat, v2f *ret, int *tilted);
void texcoord_ft(int mat, v2f *ret, int *tilted);

void populate_material_texcoord_table();

#endif  // SRC_VERTEX_BUFFER_H_

