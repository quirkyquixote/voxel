
#ifndef VOXEL_RENDERER_H_
#define VOXEL_RENDERER_H_

#define GL_GLEXT_PROTOTYPES 1
#include "SDL2/SDL_opengl.h"
#include "GL/glext.h"
#undef GL_GLEXT_PROTOTYPES

#include "v2.h"
#include "v3.h"
#include "v4.h"
#include "box2.h"
#include "array.h"
#include "types.h"

struct vertex {
	GLfloat x, y, z;
	GLfloat u0, v0;
	GLfloat u1, v1;
	GLubyte r;
	GLubyte g;
	GLubyte b;
	GLubyte a;
};

struct vertex_desc {
	GLfloat x, y, z;
	GLfloat u, v;
	GLfloat r, g, b;
	int face;
};

struct vertex_buffer {
	size_t vbo_count;
	GLuint *vbo_names;
	size_t *vbo_sizes;
};

extern const struct v2f texcoord_from_mat[][3];

extern struct vertex_desc vertices_face_dn[];
extern struct vertex_desc vertices_face_up[];
extern struct vertex_desc vertices_face_lf[];
extern struct vertex_desc vertices_face_rt[];
extern struct vertex_desc vertices_face_bk[];
extern struct vertex_desc vertices_face_ft[];

extern struct vertex_desc vertices_slab_dn[];
extern struct vertex_desc vertices_slab_up[];
extern struct vertex_desc vertices_slab_lf[];
extern struct vertex_desc vertices_slab_rt[];
extern struct vertex_desc vertices_slab_bk[];
extern struct vertex_desc vertices_slab_ft[];
extern struct vertex_desc vertices_stairs_dl[];
extern struct vertex_desc vertices_stairs_dr[];
extern struct vertex_desc vertices_stairs_db[];
extern struct vertex_desc vertices_stairs_df[];
extern struct vertex_desc vertices_stairs_ul[];
extern struct vertex_desc vertices_stairs_ur[];
extern struct vertex_desc vertices_stairs_ub[];
extern struct vertex_desc vertices_stairs_uf[];
extern struct vertex_desc vertices_pane_x[];
extern struct vertex_desc vertices_pane_y[];
extern struct vertex_desc vertices_pane_z[];

struct vertex_buffer *vertex_buffer(int nbufs);
void vertex_buffer_destroy(struct vertex_buffer *r);
void vertex_buffer_enable(struct vertex_buffer *r);
void vertex_buffer_draw(struct vertex_buffer *r, GLenum mode, size_t buf);
void vertex_buffer_draw_slice(struct vertex_buffer *r, GLenum mode, size_t buf,
		size_t first, size_t count);
void vertex_buffer_disable(struct vertex_buffer *r);
void vertex_buffer_update(struct vertex_buffer *r, size_t buf, const void *data, size_t size);

static inline void vertices_add(struct array *s, const struct vertex_desc *buf, size_t len,
	struct v3f p, struct v2f t1, const struct v2f *t2)
{
	int i;
	struct vertex v;
	for (i = 0; i < len; ++i) {
		v.x = buf[i].x + p.x;
		v.y = buf[i].y + p.y;
		v.z = buf[i].z + p.z;
		v.u0 = t1.x;
		v.v0 = t1.y;
		v.u1 = t2[buf[i].face].x + buf[i].u / 32.;
		v.v1 = t2[buf[i].face].y + buf[i].v / 32.;
		v.r = buf[i].r * 255;
		v.g = buf[i].g * 255;
		v.b = buf[i].b * 255;
		v.a = 255;
		array_push(s, &v);
	}
}

static inline void texcoord_up(int mat, struct v2f *ret)
{
	ret[FACE_UP].x = texcoord_from_mat[mat][1].x;
	ret[FACE_UP].y = texcoord_from_mat[mat][1].y;
	ret[FACE_DN].x = texcoord_from_mat[mat][2].x;
	ret[FACE_DN].y = texcoord_from_mat[mat][2].y;
	ret[FACE_LF].x = texcoord_from_mat[mat][0].x;
	ret[FACE_LF].y = texcoord_from_mat[mat][0].y;
	ret[FACE_RT].x = texcoord_from_mat[mat][0].x;
	ret[FACE_RT].y = texcoord_from_mat[mat][0].y;
	ret[FACE_BK].x = texcoord_from_mat[mat][0].x;
	ret[FACE_BK].y = texcoord_from_mat[mat][0].y;
	ret[FACE_FT].x = texcoord_from_mat[mat][0].x;
	ret[FACE_FT].y = texcoord_from_mat[mat][0].y;
}

static inline void texcoord_dn(int mat, struct v2f *ret)
{
	ret[FACE_UP].x = texcoord_from_mat[mat][2].x;
	ret[FACE_UP].y = texcoord_from_mat[mat][2].y;
	ret[FACE_DN].x = texcoord_from_mat[mat][1].x;
	ret[FACE_DN].y = texcoord_from_mat[mat][1].y;
	ret[FACE_LF].x = texcoord_from_mat[mat][0].x;
	ret[FACE_LF].y = texcoord_from_mat[mat][0].y;
	ret[FACE_RT].x = texcoord_from_mat[mat][0].x;
	ret[FACE_RT].y = texcoord_from_mat[mat][0].y;
	ret[FACE_BK].x = texcoord_from_mat[mat][0].x;
	ret[FACE_BK].y = texcoord_from_mat[mat][0].y;
	ret[FACE_FT].x = texcoord_from_mat[mat][0].x;
	ret[FACE_FT].y = texcoord_from_mat[mat][0].y;
}

static inline void texcoord_lf(int mat, struct v2f *ret)
{
	ret[FACE_UP].x = texcoord_from_mat[mat][0].x;
	ret[FACE_UP].y = texcoord_from_mat[mat][0].y;
	ret[FACE_DN].x = texcoord_from_mat[mat][0].x;
	ret[FACE_DN].y = texcoord_from_mat[mat][0].y;
	ret[FACE_LF].x = texcoord_from_mat[mat][1].x;
	ret[FACE_LF].y = texcoord_from_mat[mat][1].y;
	ret[FACE_RT].x = texcoord_from_mat[mat][2].x;
	ret[FACE_RT].y = texcoord_from_mat[mat][2].y;
	ret[FACE_BK].x = texcoord_from_mat[mat][0].x;
	ret[FACE_BK].y = texcoord_from_mat[mat][0].y;
	ret[FACE_FT].x = texcoord_from_mat[mat][0].x;
	ret[FACE_FT].y = texcoord_from_mat[mat][0].y;
}

static inline void texcoord_rt(int mat, struct v2f *ret)
{
	ret[FACE_UP].x = texcoord_from_mat[mat][0].x;
	ret[FACE_UP].y = texcoord_from_mat[mat][0].y;
	ret[FACE_DN].x = texcoord_from_mat[mat][0].x;
	ret[FACE_DN].y = texcoord_from_mat[mat][0].y;
	ret[FACE_LF].x = texcoord_from_mat[mat][2].x;
	ret[FACE_LF].y = texcoord_from_mat[mat][2].y;
	ret[FACE_RT].x = texcoord_from_mat[mat][1].x;
	ret[FACE_RT].y = texcoord_from_mat[mat][1].y;
	ret[FACE_BK].x = texcoord_from_mat[mat][0].x;
	ret[FACE_BK].y = texcoord_from_mat[mat][0].y;
	ret[FACE_FT].x = texcoord_from_mat[mat][0].x;
	ret[FACE_FT].y = texcoord_from_mat[mat][0].y;
}

static inline void texcoord_bk(int mat, struct v2f *ret)
{
	ret[FACE_UP].x = texcoord_from_mat[mat][0].x;
	ret[FACE_UP].y = texcoord_from_mat[mat][0].y;
	ret[FACE_DN].x = texcoord_from_mat[mat][0].x;
	ret[FACE_DN].y = texcoord_from_mat[mat][0].y;
	ret[FACE_LF].x = texcoord_from_mat[mat][0].x;
	ret[FACE_LF].y = texcoord_from_mat[mat][0].y;
	ret[FACE_RT].x = texcoord_from_mat[mat][0].x;
	ret[FACE_RT].y = texcoord_from_mat[mat][0].y;
	ret[FACE_BK].x = texcoord_from_mat[mat][1].x;
	ret[FACE_BK].y = texcoord_from_mat[mat][1].y;
	ret[FACE_FT].x = texcoord_from_mat[mat][2].x;
	ret[FACE_FT].y = texcoord_from_mat[mat][2].y;
}

static inline void texcoord_ft(int mat, struct v2f *ret)
{
	ret[FACE_UP].x = texcoord_from_mat[mat][0].x;
	ret[FACE_UP].y = texcoord_from_mat[mat][0].y;
	ret[FACE_DN].x = texcoord_from_mat[mat][0].x;
	ret[FACE_DN].y = texcoord_from_mat[mat][0].y;
	ret[FACE_LF].x = texcoord_from_mat[mat][0].x;
	ret[FACE_LF].y = texcoord_from_mat[mat][0].y;
	ret[FACE_RT].x = texcoord_from_mat[mat][0].x;
	ret[FACE_RT].y = texcoord_from_mat[mat][0].y;
	ret[FACE_BK].x = texcoord_from_mat[mat][2].x;
	ret[FACE_BK].y = texcoord_from_mat[mat][2].y;
	ret[FACE_FT].x = texcoord_from_mat[mat][1].x;
	ret[FACE_FT].y = texcoord_from_mat[mat][1].y;
}

#endif

