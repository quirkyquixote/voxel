/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "vertex_buffer.h"

#include <assert.h>
#include <stdlib.h>

#include "types.h"
#include "tone_mapper.h"

#define T_COL 1.
#define X_COL .8
#define Z_COL .6
#define B_COL .4

#define TC(mat, a0, b0, a1, b1, a2, b2)\
do {\
	texcoord_from_mat[mat][0] = v2f(a0 / 32., b0 / 32.);\
	texcoord_from_mat[mat][1] = v2f(a1 / 32., b1 / 32.);\
	texcoord_from_mat[mat][2] = v2f(a2 / 32., b2 / 32.);\
} while (0)

#define COMMON_TC(mat, y) \
	TC(mat##_CRATE, 16, y, 17, y, 0, y);\
	TC(mat##_BENCH, 18, y, 19, y, 0, y);\
	TC(mat##_PIPE, 20, y, 21, y, 21, y);\
	TC(mat##_BOARD, 22, y, 23, y, 0, y)

#define STONE_TC(mat, y)\
	TC(mat, 0, y, 0, y, 0, y);\
	TC(mat##_COBBLE, 1, y, 1, y, 1, y);\
	TC(mat##_LBRICK, 6, y, 7, y, 8, y);\
	TC(mat##_SBRICK, 3, y, 4, y, 5, y);\
	TC(mat##_TILE, 2, y, 2, y, 2, y);\
	TC(mat##_COLUMN, 9, y, 10, y, 10, y);\
	TC(mat##_BLOCK, 7, y, 7, y, 7, y);\
	TC(mat##_SAND, 15, y, 15, y, 16, y);\
	COMMON_TC(mat, y)

#define WOOD_TC(mat, y)\
	TC(mat, 1, y, 1, y, 1, y);\
	TC(mat##_LOG, 3, y, 2, y, 2, y);\
	COMMON_TC(mat, y)

v2f texcoord_from_mat[256][3];

void populate_material_texcoord_table()
{
	TC(MAT_DIRT, 1, 0, 1, 0, 1, 0);
	TC(MAT_GRASS, 2, 0, 2, 0, 2, 0);
	STONE_TC(MAT_LIMESTONE, 1);
	STONE_TC(MAT_SANDSTONE, 2);
	STONE_TC(MAT_MARBLE, 3);
	STONE_TC(MAT_GRANITE, 4);
	STONE_TC(MAT_BASALT, 5);
	STONE_TC(MAT_SLATE, 6);
	STONE_TC(MAT_CLAY, 7);
	WOOD_TC(MAT_WOOD1, 8);
	WOOD_TC(MAT_WOOD2, 9);
	WOOD_TC(MAT_WOOD3, 10);
	WOOD_TC(MAT_WOOD4, 11);
	WOOD_TC(MAT_WOOD5, 12);
	WOOD_TC(MAT_WOOD6, 13);
	WOOD_TC(MAT_WOOD7, 14);
	TC(MAT_RUNE_O, 7, 15, 0, 15, 7, 15);
	TC(MAT_RUNE_K, 7, 15, 1, 15, 7, 15);
	TC(MAT_RUNE_A, 7, 15, 2, 15, 7, 15);
	TC(MAT_RUNE_N, 7, 15, 3, 15, 7, 15);
	TC(MAT_RUNE_J, 7, 15, 4, 15, 7, 15);
	TC(MAT_RUNE_C, 7, 15, 5, 15, 7, 15);
	TC(MAT_RUNE_E, 7, 15, 6, 15, 7, 15);
};

struct VertexDesc vertices_face_dn[] = {
	{ 0., 0., 0., B_COL, FACE_DN },
	{ 1., 0., 0., B_COL, FACE_DN },
	{ 0., 0., 1., B_COL, FACE_DN },
	{ 0., 0., 1., B_COL, FACE_DN },
	{ 1., 0., 0., B_COL, FACE_DN },
	{ 1., 0., 1., B_COL, FACE_DN },
};

struct VertexDesc vertices_face_up[] = {
	{ 1., 1., 1., T_COL, FACE_UP },
	{ 1., 1., 0., T_COL, FACE_UP },
	{ 0., 1., 1., T_COL, FACE_UP },
	{ 0., 1., 1., T_COL, FACE_UP },
	{ 1., 1., 0., T_COL, FACE_UP },
	{ 0., 1., 0., T_COL, FACE_UP },
};

struct VertexDesc vertices_face_lf[] = {
	{ 0., 0., 0., X_COL, FACE_LF },
	{ 0., 0., 1., X_COL, FACE_LF },
	{ 0., 1., 0., X_COL, FACE_LF },
	{ 0., 1., 0., X_COL, FACE_LF },
	{ 0., 0., 1., X_COL, FACE_LF },
	{ 0., 1., 1., X_COL, FACE_LF },
};

struct VertexDesc vertices_face_rt[] = {
	{ 1., 0., 1., X_COL, FACE_RT },
	{ 1., 0., 0., X_COL, FACE_RT },
	{ 1., 1., 1., X_COL, FACE_RT },
	{ 1., 1., 1., X_COL, FACE_RT },
	{ 1., 0., 0., X_COL, FACE_RT },
	{ 1., 1., 0., X_COL, FACE_RT },
};

struct VertexDesc vertices_face_bk[] = {
	{ 1., 0., 0., Z_COL, FACE_BK },
	{ 0., 0., 0., Z_COL, FACE_BK },
	{ 1., 1., 0., Z_COL, FACE_BK },
	{ 1., 1., 0., Z_COL, FACE_BK },
	{ 0., 0., 0., Z_COL, FACE_BK },
	{ 0., 1., 0., Z_COL, FACE_BK },
};

struct VertexDesc vertices_face_ft[] = {
	{ 0., 0., 1., Z_COL, FACE_FT },
	{ 1., 0., 1., Z_COL, FACE_FT },
	{ 0., 1., 1., Z_COL, FACE_FT },
	{ 0., 1., 1., Z_COL, FACE_FT },
	{ 1., 0., 1., Z_COL, FACE_FT },
	{ 1., 1., 1., Z_COL, FACE_FT },
};

struct VertexDesc vertices_slab_dn[] = {
	{ 1., .5, 1., T_COL, FACE_UP },
	{ 1., .5, 0., T_COL, FACE_UP },
	{ 0., .5, 1., T_COL, FACE_UP },
	{ 0., .5, 1., T_COL, FACE_UP },
	{ 1., .5, 0., T_COL, FACE_UP },
	{ 0., .5, 0., T_COL, FACE_UP },
	{ 0., 0., 0., X_COL, FACE_LF },
	{ 0., 0., 1., X_COL, FACE_LF },
	{ 0., .5, 0., X_COL, FACE_LF },
	{ 0., .5, 0., X_COL, FACE_LF },
	{ 0., 0., 1., X_COL, FACE_LF },
	{ 0., .5, 1., X_COL, FACE_LF },
	{ 1., 0., 1., X_COL, FACE_RT },
	{ 1., 0., 0., X_COL, FACE_RT },
	{ 1., .5, 1., X_COL, FACE_RT },
	{ 1., .5, 1., X_COL, FACE_RT },
	{ 1., 0., 0., X_COL, FACE_RT },
	{ 1., .5, 0., X_COL, FACE_RT },
	{ 1., 0., 0., Z_COL, FACE_BK },
	{ 0., 0., 0., Z_COL, FACE_BK },
	{ 1., .5, 0., Z_COL, FACE_BK },
	{ 1., .5, 0., Z_COL, FACE_BK },
	{ 0., 0., 0., Z_COL, FACE_BK },
	{ 0., .5, 0., Z_COL, FACE_BK },
	{ 0., 0., 1., Z_COL, FACE_FT },
	{ 1., 0., 1., Z_COL, FACE_FT },
	{ 0., .5, 1., Z_COL, FACE_FT },
	{ 0., .5, 1., Z_COL, FACE_FT },
	{ 1., 0., 1., Z_COL, FACE_FT },
	{ 1., .5, 1., Z_COL, FACE_FT },
};

struct VertexDesc vertices_slab_up[] = {
	{ 0., .5, 0., B_COL, FACE_DN },
	{ 1., .5, 0., B_COL, FACE_DN },
	{ 0., .5, 1., B_COL, FACE_DN },
	{ 0., .5, 1., B_COL, FACE_DN },
	{ 1., .5, 0., B_COL, FACE_DN },
	{ 1., .5, 1., B_COL, FACE_DN },
	{ 0., .5, 0., X_COL, FACE_LF },
	{ 0., .5, 1., X_COL, FACE_LF },
	{ 0., 1., 0., X_COL, FACE_LF },
	{ 0., 1., 0., X_COL, FACE_LF },
	{ 0., .5, 1., X_COL, FACE_LF },
	{ 0., 1., 1., X_COL, FACE_LF },
	{ 1., .5, 1., X_COL, FACE_RT },
	{ 1., .5, 0., X_COL, FACE_RT },
	{ 1., 1., 1., X_COL, FACE_RT },
	{ 1., 1., 1., X_COL, FACE_RT },
	{ 1., .5, 0., X_COL, FACE_RT },
	{ 1., 1., 0., X_COL, FACE_RT },
	{ 1., .5, 0., Z_COL, FACE_BK },
	{ 0., .5, 0., Z_COL, FACE_BK },
	{ 1., 1., 0., Z_COL, FACE_BK },
	{ 1., 1., 0., Z_COL, FACE_BK },
	{ 0., .5, 0., Z_COL, FACE_BK },
	{ 0., 1., 0., Z_COL, FACE_BK },
	{ 0., .5, 1., Z_COL, FACE_FT },
	{ 1., .5, 1., Z_COL, FACE_FT },
	{ 0., 1., 1., Z_COL, FACE_FT },
	{ 0., 1., 1., Z_COL, FACE_FT },
	{ 1., .5, 1., Z_COL, FACE_FT },
	{ 1., 1., 1., Z_COL, FACE_FT },
};

struct VertexDesc vertices_slab_lf[] = {
	{ 0., 0., 0., B_COL, FACE_DN },
	{ .5, 0., 0., B_COL, FACE_DN },
	{ 0., 0., 1., B_COL, FACE_DN },
	{ 0., 0., 1., B_COL, FACE_DN },
	{ .5, 0., 0., B_COL, FACE_DN },
	{ .5, 0., 1., B_COL, FACE_DN },
	{ .5, 1., 1., T_COL, FACE_UP },
	{ .5, 1., 0., T_COL, FACE_UP },
	{ 0., 1., 1., T_COL, FACE_UP },
	{ 0., 1., 1., T_COL, FACE_UP },
	{ .5, 1., 0., T_COL, FACE_UP },
	{ 0., 1., 0., T_COL, FACE_UP },
	{ .5, 0., 1., X_COL, FACE_LF },
	{ .5, 0., 0., X_COL, FACE_LF },
	{ .5, 1., 1., X_COL, FACE_LF },
	{ .5, 1., 1., X_COL, FACE_LF },
	{ .5, 0., 0., X_COL, FACE_LF },
	{ .5, 1., 0., X_COL, FACE_LF },
	{ .5, 0., 0., Z_COL, FACE_BK },
	{ 0., 0., 0., Z_COL, FACE_BK },
	{ .5, 1., 0., Z_COL, FACE_BK },
	{ .5, 1., 0., Z_COL, FACE_BK },
	{ 0., 0., 0., Z_COL, FACE_BK },
	{ 0., 1., 0., Z_COL, FACE_BK },
	{ 0., 0., 1., Z_COL, FACE_FT },
	{ .5, 0., 1., Z_COL, FACE_FT },
	{ 0., 1., 1., Z_COL, FACE_FT },
	{ 0., 1., 1., Z_COL, FACE_FT },
	{ .5, 0., 1., Z_COL, FACE_FT },
	{ .5, 1., 1., Z_COL, FACE_FT },
};

struct VertexDesc vertices_slab_rt[] = {
	{ .5, 0., 0., B_COL, FACE_DN },
	{ 1., 0., 0., B_COL, FACE_DN },
	{ .5, 0., 1., B_COL, FACE_DN },
	{ .5, 0., 1., B_COL, FACE_DN },
	{ 1., 0., 0., B_COL, FACE_DN },
	{ 1., 0., 1., B_COL, FACE_DN },
	{ 1., 1., 1., T_COL, FACE_UP },
	{ 1., 1., 0., T_COL, FACE_UP },
	{ .5, 1., 1., T_COL, FACE_UP },
	{ .5, 1., 1., T_COL, FACE_UP },
	{ 1., 1., 0., T_COL, FACE_UP },
	{ .5, 1., 0., T_COL, FACE_UP },
	{ .5, 0., 0., X_COL, FACE_LF },
	{ .5, 0., 1., X_COL, FACE_LF },
	{ .5, 1., 0., X_COL, FACE_LF },
	{ .5, 1., 0., X_COL, FACE_LF },
	{ .5, 0., 1., X_COL, FACE_LF },
	{ .5, 1., 1., X_COL, FACE_LF },
	{ 1., 0., 0., Z_COL, FACE_BK },
	{ .5, 0., 0., Z_COL, FACE_BK },
	{ 1., 1., 0., Z_COL, FACE_BK },
	{ 1., 1., 0., Z_COL, FACE_BK },
	{ .5, 0., 0., Z_COL, FACE_BK },
	{ .5, 1., 0., Z_COL, FACE_BK },
	{ .5, 0., 1., Z_COL, FACE_FT },
	{ 1., 0., 1., Z_COL, FACE_FT },
	{ .5, 1., 1., Z_COL, FACE_FT },
	{ .5, 1., 1., Z_COL, FACE_FT },
	{ 1., 0., 1., Z_COL, FACE_FT },
	{ 1., 1., 1., Z_COL, FACE_FT },
};

struct VertexDesc vertices_slab_bk[] = {
	{ 0., 0., 0., B_COL, FACE_DN },
	{ 1., 0., 0., B_COL, FACE_DN },
	{ 0., 0., .5, B_COL, FACE_DN },
	{ 0., 0., .5, B_COL, FACE_DN },
	{ 1., 0., 0., B_COL, FACE_DN },
	{ 1., 0., .5, B_COL, FACE_DN },
	{ 1., 1., .5, T_COL, FACE_UP },
	{ 1., 1., 0., T_COL, FACE_UP },
	{ 0., 1., .5, T_COL, FACE_UP },
	{ 0., 1., .5, T_COL, FACE_UP },
	{ 1., 1., 0., T_COL, FACE_UP },
	{ 0., 1., 0., T_COL, FACE_UP },
	{ 0., 0., 0., X_COL, FACE_LF },
	{ 0., 0., .5, X_COL, FACE_LF },
	{ 0., 1., 0., X_COL, FACE_LF },
	{ 0., 1., 0., X_COL, FACE_LF },
	{ 0., 0., .5, X_COL, FACE_LF },
	{ 0., 1., .5, X_COL, FACE_LF },
	{ 1., 0., .5, X_COL, FACE_RT },
	{ 1., 0., 0., X_COL, FACE_RT },
	{ 1., 1., .5, X_COL, FACE_RT },
	{ 1., 1., .5, X_COL, FACE_RT },
	{ 1., 0., 0., X_COL, FACE_RT },
	{ 1., 1., 0., X_COL, FACE_RT },
	{ 0., 0., .5, Z_COL, FACE_BK },
	{ 1., 0., .5, Z_COL, FACE_BK },
	{ 0., 1., .5, Z_COL, FACE_BK },
	{ 0., 1., .5, Z_COL, FACE_BK },
	{ 1., 0., .5, Z_COL, FACE_BK },
	{ 1., 1., .5, Z_COL, FACE_BK },
};

struct VertexDesc vertices_slab_ft[] = {
	{ 0., 0., .5, B_COL, FACE_DN },
	{ 1., 0., .5, B_COL, FACE_DN },
	{ 0., 0., 1., B_COL, FACE_DN },
	{ 0., 0., 1., B_COL, FACE_DN },
	{ 1., 0., .5, B_COL, FACE_DN },
	{ 1., 0., 1., B_COL, FACE_DN },
	{ 1., 1., 1., T_COL, FACE_UP },
	{ 1., 1., .5, T_COL, FACE_UP },
	{ 0., 1., 1., T_COL, FACE_UP },
	{ 0., 1., 1., T_COL, FACE_UP },
	{ 1., 1., .5, T_COL, FACE_UP },
	{ 0., 1., .5, T_COL, FACE_UP },
	{ 0., 0., .5, X_COL, FACE_LF },
	{ 0., 0., 1., X_COL, FACE_LF },
	{ 0., 1., .5, X_COL, FACE_LF },
	{ 0., 1., .5, X_COL, FACE_LF },
	{ 0., 0., 1., X_COL, FACE_LF },
	{ 0., 1., 1., X_COL, FACE_LF },
	{ 1., 0., 1., X_COL, FACE_RT },
	{ 1., 0., .5, X_COL, FACE_RT },
	{ 1., 1., 1., X_COL, FACE_RT },
	{ 1., 1., 1., X_COL, FACE_RT },
	{ 1., 0., .5, X_COL, FACE_RT },
	{ 1., 1., .5, X_COL, FACE_RT },
	{ 1., 0., .5, Z_COL, FACE_BK },
	{ 0., 0., .5, Z_COL, FACE_BK },
	{ 1., 1., .5, Z_COL, FACE_BK },
	{ 1., 1., .5, Z_COL, FACE_BK },
	{ 0., 0., .5, Z_COL, FACE_BK },
	{ 0., 1., .5, Z_COL, FACE_BK },
};

struct VertexDesc vertices_stairs_dl[] = {
	{ 1., .5, 1., T_COL, FACE_UP },
	{ 1., .5, 0., T_COL, FACE_UP },
	{ .5, .5, 1., T_COL, FACE_UP },
	{ .5, .5, 1., T_COL, FACE_UP },
	{ 1., .5, 0., T_COL, FACE_UP },
	{ .5, .5, 0., T_COL, FACE_UP },
	{ .5, 1., 1., T_COL, FACE_UP },
	{ .5, 1., 0., T_COL, FACE_UP },
	{ 0., 1., 1., T_COL, FACE_UP },
	{ 0., 1., 1., T_COL, FACE_UP },
	{ .5, 1., 0., T_COL, FACE_UP },
	{ 0., 1., 0., T_COL, FACE_UP },
	{ 1., 0., 1., X_COL, FACE_RT },
	{ 1., 0., 0., X_COL, FACE_RT },
	{ 1., .5, 1., X_COL, FACE_RT },
	{ 1., .5, 1., X_COL, FACE_RT },
	{ 1., 0., 0., X_COL, FACE_RT },
	{ 1., .5, 0., X_COL, FACE_RT },
	{ .5, .5, 1., X_COL, FACE_LF },
	{ .5, .5, 0., X_COL, FACE_LF },
	{ .5, 1., 1., X_COL, FACE_LF },
	{ .5, 1., 1., X_COL, FACE_LF },
	{ .5, .5, 0., X_COL, FACE_LF },
	{ .5, 1., 0., X_COL, FACE_LF },
	{ 1., 0., 0., Z_COL, FACE_BK },
	{ 0., 0., 0., Z_COL, FACE_BK },
	{ 0., 1., 0., Z_COL, FACE_BK },
	{ 1., 0., 0., Z_COL, FACE_BK },
	{ .5, .5, 0., Z_COL, FACE_BK },
	{ 1., .5, 0., Z_COL, FACE_BK },
	{ .5, .5, 0., Z_COL, FACE_BK },
	{ 0., 1., 0., Z_COL, FACE_BK },
	{ .5, 1., 0., Z_COL, FACE_BK },
	{ 0., 0., 1., Z_COL, FACE_FT },
	{ 1., 0., 1., Z_COL, FACE_FT },
	{ 0., 1., 1., Z_COL, FACE_FT },
	{ .5, .5, 1., Z_COL, FACE_FT },
	{ 1., 0., 1., Z_COL, FACE_FT },
	{ 1., .5, 1., Z_COL, FACE_FT },
	{ 0., 1., 1., Z_COL, FACE_FT },
	{ .5, .5, 1., Z_COL, FACE_FT },
	{ .5, 1., 1., Z_COL, FACE_FT },
};

struct VertexDesc vertices_stairs_dr[] = {
	{ 0., .5, 0., T_COL, FACE_UP },
	{ 0., .5, 1., T_COL, FACE_UP },
	{ .5, .5, 1., T_COL, FACE_UP },
	{ 0., .5, 0., T_COL, FACE_UP },
	{ .5, .5, 1., T_COL, FACE_UP },
	{ .5, .5, 0., T_COL, FACE_UP },
	{ .5, 1., 0., T_COL, FACE_UP },
	{ .5, 1., 1., T_COL, FACE_UP },
	{ 1., 1., 1., T_COL, FACE_UP },
	{ .5, 1., 0., T_COL, FACE_UP },
	{ 1., 1., 1., T_COL, FACE_UP },
	{ 1., 1., 0., T_COL, FACE_UP },
	{ 0., 0., 0., X_COL, FACE_LF },
	{ 0., 0., 1., X_COL, FACE_LF },
	{ 0., .5, 1., X_COL, FACE_LF },
	{ 0., 0., 0., X_COL, FACE_LF },
	{ 0., .5, 1., X_COL, FACE_LF },
	{ 0., .5, 0., X_COL, FACE_LF },
	{ .5, .5, 0., X_COL, FACE_LF },
	{ .5, .5, 1., X_COL, FACE_LF },
	{ .5, 1., 1., X_COL, FACE_LF },
	{ .5, .5, 0., X_COL, FACE_LF },
	{ .5, 1., 1., X_COL, FACE_LF },
	{ .5, 1., 0., X_COL, FACE_LF },
	{ 1., 0., 0., Z_COL, FACE_BK },
	{ 0., 0., 0., Z_COL, FACE_BK },
	{ 1., 1., 0., Z_COL, FACE_BK },
	{ .5, .5, 0., Z_COL, FACE_BK },
	{ 0., 0., 0., Z_COL, FACE_BK },
	{ 0., .5, 0., Z_COL, FACE_BK },
	{ 1., 1., 0., Z_COL, FACE_BK },
	{ .5, .5, 0., Z_COL, FACE_BK },
	{ .5, 1., 0., Z_COL, FACE_BK },
	{ 0., 0., 1., Z_COL, FACE_FT },
	{ 1., 0., 1., Z_COL, FACE_FT },
	{ 1., 1., 1., Z_COL, FACE_FT },
	{ 0., 0., 1., Z_COL, FACE_FT },
	{ .5, .5, 1., Z_COL, FACE_FT },
	{ 0., .5, 1., Z_COL, FACE_FT },
	{ .5, .5, 1., Z_COL, FACE_FT },
	{ 1., 1., 1., Z_COL, FACE_FT },
	{ .5, 1., 1., Z_COL, FACE_FT },
};

struct VertexDesc vertices_stairs_db[] = {
	{ 0., .5, 1., T_COL, FACE_UP },
	{ 1., .5, 1., T_COL, FACE_UP },
	{ 1., .5, .5, T_COL, FACE_UP },
	{ 0., .5, 1., T_COL, FACE_UP },
	{ 1., .5, .5, T_COL, FACE_UP },
	{ 0., .5, .5, T_COL, FACE_UP },
	{ 0., 1., .5, T_COL, FACE_UP },
	{ 1., 1., .5, T_COL, FACE_UP },
	{ 1., 1., 0., T_COL, FACE_UP },
	{ 0., 1., .5, T_COL, FACE_UP },
	{ 1., 1., 0., T_COL, FACE_UP },
	{ 0., 1., 0., T_COL, FACE_UP },
	{ 0., 0., 1., Z_COL, FACE_FT },
	{ 1., 0., 1., Z_COL, FACE_FT },
	{ 1., .5, 1., Z_COL, FACE_FT },
	{ 0., 0., 1., Z_COL, FACE_FT },
	{ 1., .5, 1., Z_COL, FACE_FT },
	{ 0., .5, 1., Z_COL, FACE_FT },
	{ 0., .5, .5, Z_COL, FACE_BK },
	{ 1., .5, .5, Z_COL, FACE_BK },
	{ 1., 1., .5, Z_COL, FACE_BK },
	{ 0., .5, .5, Z_COL, FACE_BK },
	{ 1., 1., .5, Z_COL, FACE_BK },
	{ 0., 1., .5, Z_COL, FACE_BK },
	{ 0., 0., 0., X_COL, FACE_LF },
	{ 0., 0., 1., X_COL, FACE_LF },
	{ 0., 1., 0., X_COL, FACE_LF },
	{ 0., .5, .5, X_COL, FACE_LF },
	{ 0., 0., 1., X_COL, FACE_LF },
	{ 0., .5, 1., X_COL, FACE_LF },
	{ 0., 1., 0., X_COL, FACE_LF },
	{ 0., .5, .5, X_COL, FACE_LF },
	{ 0., 1., .5, X_COL, FACE_LF },
	{ 1., 0., 1., X_COL, FACE_RT },
	{ 1., 0., 0., X_COL, FACE_RT },
	{ 1., 1., 0., X_COL, FACE_RT },
	{ 1., 0., 1., X_COL, FACE_RT },
	{ 1., .5, .5, X_COL, FACE_RT },
	{ 1., .5, 1., X_COL, FACE_RT },
	{ 1., .5, .5, X_COL, FACE_RT },
	{ 1., 1., 0., X_COL, FACE_RT },
	{ 1., 1., .5, X_COL, FACE_RT },
};

struct VertexDesc vertices_stairs_df[] = {
	{ 1., .5, 0., T_COL, FACE_UP },
	{ 0., .5, 0., T_COL, FACE_UP },
	{ 1., .5, .5, T_COL, FACE_UP },
	{ 1., .5, .5, T_COL, FACE_UP },
	{ 0., .5, 0., T_COL, FACE_UP },
	{ 0., .5, .5, T_COL, FACE_UP },
	{ 1., 1., .5, T_COL, FACE_UP },
	{ 0., 1., .5, T_COL, FACE_UP },
	{ 1., 1., 1., T_COL, FACE_UP },
	{ 1., 1., 1., T_COL, FACE_UP },
	{ 0., 1., .5, T_COL, FACE_UP },
	{ 0., 1., 1., T_COL, FACE_UP },
	{ 1., 0., 0., Z_COL, FACE_BK },
	{ 0., 0., 0., Z_COL, FACE_BK },
	{ 1., .5, 0., Z_COL, FACE_BK },
	{ 1., .5, 0., Z_COL, FACE_BK },
	{ 0., 0., 0., Z_COL, FACE_BK },
	{ 0., .5, 0., Z_COL, FACE_BK },
	{ 1., .5, .5, Z_COL, FACE_BK },
	{ 0., .5, .5, Z_COL, FACE_BK },
	{ 1., 1., .5, Z_COL, FACE_BK },
	{ 1., 1., .5, Z_COL, FACE_BK },
	{ 0., .5, .5, Z_COL, FACE_BK },
	{ 0., 1., .5, Z_COL, FACE_BK },
	{ 0., 0., 0., X_COL, FACE_LF },
	{ 0., 0., 1., X_COL, FACE_LF },
	{ 0., 1., 1., X_COL, FACE_LF },
	{ 0., 0., 0., X_COL, FACE_LF },
	{ 0., .5, .5, X_COL, FACE_LF },
	{ 0., .5, 0., X_COL, FACE_LF },
	{ 0., .5, .5, X_COL, FACE_LF },
	{ 0., 1., 1., X_COL, FACE_LF },
	{ 0., 1., .5, X_COL, FACE_LF },
	{ 1., 0., 1., X_COL, FACE_RT },
	{ 1., 0., 0., X_COL, FACE_RT },
	{ 1., 1., 1., X_COL, FACE_RT },
	{ 1., .5, .5, X_COL, FACE_RT },
	{ 1., 0., 0., X_COL, FACE_RT },
	{ 1., .5, 0., X_COL, FACE_RT },
	{ 1., 1., 1., X_COL, FACE_RT },
	{ 1., .5, .5, X_COL, FACE_RT },
	{ 1., 1., .5, X_COL, FACE_RT },
};

struct VertexDesc vertices_stairs_ul[] = {
	{ 1., .5, 0., B_COL, FACE_DN },
	{ 1., .5, 1., B_COL, FACE_DN },
	{ .5, .5, 1., B_COL, FACE_DN },
	{ 1., .5, 0., B_COL, FACE_DN },
	{ .5, .5, 1., B_COL, FACE_DN },
	{ .5, .5, 0., B_COL, FACE_DN },
	{ .5, 0., 0., B_COL, FACE_DN },
	{ .5, 0., 1., B_COL, FACE_DN },
	{ 0., 0., 1., B_COL, FACE_DN },
	{ .5, 0., 0., B_COL, FACE_DN },
	{ 0., 0., 1., B_COL, FACE_DN },
	{ 0., 0., 0., B_COL, FACE_DN },
	{ 1., 1., 0., X_COL, FACE_RT },
	{ 1., 1., 1., X_COL, FACE_RT },
	{ 1., .5, 1., X_COL, FACE_RT },
	{ 1., 1., 0., X_COL, FACE_RT },
	{ 1., .5, 1., X_COL, FACE_RT },
	{ 1., .5, 0., X_COL, FACE_RT },
	{ .5, .5, 0., X_COL, FACE_LF },
	{ .5, .5, 1., X_COL, FACE_LF },
	{ .5, 0., 1., X_COL, FACE_LF },
	{ .5, .5, 0., X_COL, FACE_LF },
	{ .5, 0., 1., X_COL, FACE_LF },
	{ .5, 0., 0., X_COL, FACE_LF },
	{ 0., 1., 0., Z_COL, FACE_BK },
	{ 1., 1., 0., Z_COL, FACE_BK },
	{ 0., 0., 0., Z_COL, FACE_BK },
	{ .5, .5, 0., Z_COL, FACE_BK },
	{ 1., 1., 0., Z_COL, FACE_BK },
	{ 1., .5, 0., Z_COL, FACE_BK },
	{ 0., 0., 0., Z_COL, FACE_BK },
	{ .5, .5, 0., Z_COL, FACE_BK },
	{ .5, 0., 0., Z_COL, FACE_BK },
	{ 1., 1., 1., Z_COL, FACE_FT },
	{ 0., 1., 1., Z_COL, FACE_FT },
	{ 0., 0., 1., Z_COL, FACE_FT },
	{ 1., 1., 1., Z_COL, FACE_FT },
	{ .5, .5, 1., Z_COL, FACE_FT },
	{ 1., .5, 1., Z_COL, FACE_FT },
	{ .5, .5, 1., Z_COL, FACE_FT },
	{ 0., 0., 1., Z_COL, FACE_FT },
	{ .5, 0., 1., Z_COL, FACE_FT },
};

struct VertexDesc vertices_stairs_ur[] = {
	{ 0., .5, 1., B_COL, FACE_DN },
	{ 0., .5, 0., B_COL, FACE_DN },
	{ .5, .5, 1., B_COL, FACE_DN },
	{ .5, .5, 1., B_COL, FACE_DN },
	{ 0., .5, 0., B_COL, FACE_DN },
	{ .5, .5, 0., B_COL, FACE_DN },
	{ .5, 0., 1., B_COL, FACE_DN },
	{ .5, 0., 0., B_COL, FACE_DN },
	{ 1., 0., 1., B_COL, FACE_DN },
	{ 1., 0., 1., B_COL, FACE_DN },
	{ .5, 0., 0., B_COL, FACE_DN },
	{ 1., 0., 0., B_COL, FACE_DN },
	{ 0., 1., 1., X_COL, FACE_LF },
	{ 0., 1., 0., X_COL, FACE_LF },
	{ 0., .5, 1., X_COL, FACE_LF },
	{ 0., .5, 1., X_COL, FACE_LF },
	{ 0., 1., 0., X_COL, FACE_LF },
	{ 0., .5, 0., X_COL, FACE_LF },
	{ .5, .5, 1., X_COL, FACE_LF },
	{ .5, .5, 0., X_COL, FACE_LF },
	{ .5, 0., 1., X_COL, FACE_LF },
	{ .5, 0., 1., X_COL, FACE_LF },
	{ .5, .5, 0., X_COL, FACE_LF },
	{ .5, 0., 0., X_COL, FACE_LF },
	{ 0., 1., 0., Z_COL, FACE_BK },
	{ 1., 1., 0., Z_COL, FACE_BK },
	{ 1., 0., 0., Z_COL, FACE_BK },
	{ 0., 1., 0., Z_COL, FACE_BK },
	{ .5, .5, 0., Z_COL, FACE_BK },
	{ 0., .5, 0., Z_COL, FACE_BK },
	{ .5, .5, 0., Z_COL, FACE_BK },
	{ 1., 0., 0., Z_COL, FACE_BK },
	{ .5, 0., 0., Z_COL, FACE_BK },
	{ 1., 1., 1., Z_COL, FACE_FT },
	{ 0., 1., 1., Z_COL, FACE_FT },
	{ 1., 0., 1., Z_COL, FACE_FT },
	{ .5, .5, 1., Z_COL, FACE_FT },
	{ 0., 1., 1., Z_COL, FACE_FT },
	{ 0., .5, 1., Z_COL, FACE_FT },
	{ 1., 0., 1., Z_COL, FACE_FT },
	{ .5, .5, 1., Z_COL, FACE_FT },
	{ .5, 0., 1., Z_COL, FACE_FT },
};

struct VertexDesc vertices_stairs_ub[] = {
	{ 1., .5, 1., B_COL, FACE_DN },
	{ 0., .5, 1., B_COL, FACE_DN },
	{ 1., .5, .5, B_COL, FACE_DN },
	{ 1., .5, .5, B_COL, FACE_DN },
	{ 0., .5, 1., B_COL, FACE_DN },
	{ 0., .5, .5, B_COL, FACE_DN },
	{ 1., 0., .5, B_COL, FACE_DN },
	{ 0., 0., .5, B_COL, FACE_DN },
	{ 1., 0., 0., B_COL, FACE_DN },
	{ 1., 0., 0., B_COL, FACE_DN },
	{ 0., 0., .5, B_COL, FACE_DN },
	{ 0., 0., 0., B_COL, FACE_DN },
	{ 1., 1., 1., Z_COL, FACE_FT },
	{ 0., 1., 1., Z_COL, FACE_FT },
	{ 1., .5, 1., Z_COL, FACE_FT },
	{ 1., .5, 1., Z_COL, FACE_FT },
	{ 0., 1., 1., Z_COL, FACE_FT },
	{ 0., .5, 1., Z_COL, FACE_FT },
	{ 1., .5, .5, Z_COL, FACE_BK },
	{ 0., .5, .5, Z_COL, FACE_BK },
	{ 1., 0., .5, Z_COL, FACE_BK },
	{ 1., 0., .5, Z_COL, FACE_BK },
	{ 0., .5, .5, Z_COL, FACE_BK },
	{ 0., 0., .5, Z_COL, FACE_BK },
	{ 0., 1., 1., X_COL, FACE_LF },
	{ 0., 1., 0., X_COL, FACE_LF },
	{ 0., 0., 0., X_COL, FACE_LF },
	{ 0., 1., 1., X_COL, FACE_LF },
	{ 0., .5, .5, X_COL, FACE_LF },
	{ 0., .5, 1., X_COL, FACE_LF },
	{ 0., .5, .5, X_COL, FACE_LF },
	{ 0., 0., 0., X_COL, FACE_LF },
	{ 0., 0., .5, X_COL, FACE_LF },
	{ 1., 1., 0., X_COL, FACE_RT },
	{ 1., 1., 1., X_COL, FACE_RT },
	{ 1., 0., 0., X_COL, FACE_RT },
	{ 1., .5, .5, X_COL, FACE_RT },
	{ 1., 1., 1., X_COL, FACE_RT },
	{ 1., .5, 1., X_COL, FACE_RT },
	{ 1., 0., 0., X_COL, FACE_RT },
	{ 1., .5, .5, X_COL, FACE_RT },
	{ 1., 0., .5, X_COL, FACE_RT },
};

struct VertexDesc vertices_stairs_uf[] = {
	{ 0., .5, 0., B_COL, FACE_DN },
	{ 1., .5, 0., B_COL, FACE_DN },
	{ 1., .5, .5, B_COL, FACE_DN },
	{ 0., .5, 0., B_COL, FACE_DN },
	{ 1., .5, .5, B_COL, FACE_DN },
	{ 0., .5, .5, B_COL, FACE_DN },
	{ 0., 0., .5, B_COL, FACE_DN },
	{ 1., 0., .5, B_COL, FACE_DN },
	{ 1., 0., 1., B_COL, FACE_DN },
	{ 0., 0., .5, B_COL, FACE_DN },
	{ 1., 0., 1., B_COL, FACE_DN },
	{ 0., 0., 1., B_COL, FACE_DN },
	{ 0., 1., 0., Z_COL, FACE_BK },
	{ 1., 1., 0., Z_COL, FACE_BK },
	{ 1., .5, 0., Z_COL, FACE_BK },
	{ 0., 1., 0., Z_COL, FACE_BK },
	{ 1., .5, 0., Z_COL, FACE_BK },
	{ 0., .5, 0., Z_COL, FACE_BK },
	{ 0., .5, .5, Z_COL, FACE_BK },
	{ 1., .5, .5, Z_COL, FACE_BK },
	{ 1., 0., .5, Z_COL, FACE_BK },
	{ 0., .5, .5, Z_COL, FACE_BK },
	{ 1., 0., .5, Z_COL, FACE_BK },
	{ 0., 0., .5, Z_COL, FACE_BK },
	{ 0., 1., 1., X_COL, FACE_LF },
	{ 0., 1., 0., X_COL, FACE_LF },
	{ 0., 0., 1., X_COL, FACE_LF },
	{ 0., .5, .5, X_COL, FACE_LF },
	{ 0., 1., 0., X_COL, FACE_LF },
	{ 0., .5, 0., X_COL, FACE_LF },
	{ 0., 0., 1., X_COL, FACE_LF },
	{ 0., .5, .5, X_COL, FACE_LF },
	{ 0., 0., .5, X_COL, FACE_LF },
	{ 1., 1., 0., X_COL, FACE_RT },
	{ 1., 1., 1., X_COL, FACE_RT },
	{ 1., 0., 1., X_COL, FACE_RT },
	{ 1., 1., 0., X_COL, FACE_RT },
	{ 1., .5, .5, X_COL, FACE_RT },
	{ 1., .5, 0., X_COL, FACE_RT },
	{ 1., .5, .5, X_COL, FACE_RT },
	{ 1., 0., 1., X_COL, FACE_RT },
	{ 1., 0., .5, X_COL, FACE_RT },
};

struct VertexDesc vertices_pane_x[] = {
	{ .46875, 0., 0., B_COL, FACE_DN },
	{ .53125, 0., 0., B_COL, FACE_DN },
	{ .46875, 0., 1., B_COL, FACE_DN },
	{ .46875, 0., 1., B_COL, FACE_DN },
	{ .53125, 0., 0., B_COL, FACE_DN },
	{ .53125, 0., 1., B_COL, FACE_DN },

	{ .53125, 1., 1., T_COL, FACE_UP },
	{ .53125, 1., 0., T_COL, FACE_UP },
	{ .46875, 1., 1., T_COL, FACE_UP },
	{ .46875, 1., 1., T_COL, FACE_UP },
	{ .53125, 1., 0., T_COL, FACE_UP },
	{ .46875, 1., 0., T_COL, FACE_UP },

	{ .46875, 0., 0., X_COL, FACE_LF },
	{ .46875, 0., 1., X_COL, FACE_LF },
	{ .46875, 1., 0., X_COL, FACE_LF },
	{ .46875, 1., 0., X_COL, FACE_LF },
	{ .46875, 0., 1., X_COL, FACE_LF },
	{ .46875, 1., 1., X_COL, FACE_LF },

	{ .53125, 0., 1., X_COL, FACE_LF },
	{ .53125, 0., 0., X_COL, FACE_LF },
	{ .53125, 1., 1., X_COL, FACE_LF },
	{ .53125, 1., 1., X_COL, FACE_LF },
	{ .53125, 0., 0., X_COL, FACE_LF },
	{ .53125, 1., 0., X_COL, FACE_LF },

	{ .53125, 0., 0., Z_COL, FACE_BK },
	{ .46875, 0., 0., Z_COL, FACE_BK },
	{ .53125, 1., 0., Z_COL, FACE_BK },
	{ .53125, 1., 0., Z_COL, FACE_BK },
	{ .46875, 0., 0., Z_COL, FACE_BK },
	{ .46875, 1., 0., Z_COL, FACE_BK },

	{ .46875, 0., 1., Z_COL, FACE_FT },
	{ .53125, 0., 1., Z_COL, FACE_FT },
	{ .46875, 1., 1., Z_COL, FACE_FT },
	{ .46875, 1., 1., Z_COL, FACE_FT },
	{ .53125, 0., 1., Z_COL, FACE_FT },
	{ .53125, 1., 1., Z_COL, FACE_FT },
};

struct VertexDesc vertices_pane_y[] = {
	{ 0., .46875, 0., B_COL, FACE_DN },
	{ 1., .46875, 0., B_COL, FACE_DN },
	{ 0., .46875, 1., B_COL, FACE_DN },
	{ 0., .46875, 1., B_COL, FACE_DN },
	{ 1., .46875, 0., B_COL, FACE_DN },
	{ 1., .46875, 1., B_COL, FACE_DN },

	{ 1., .53125, 1., T_COL, FACE_UP },
	{ 1., .53125, 0., T_COL, FACE_UP },
	{ 0., .53125, 1., T_COL, FACE_UP },
	{ 0., .53125, 1., T_COL, FACE_UP },
	{ 1., .53125, 0., T_COL, FACE_UP },
	{ 0., .53125, 0., T_COL, FACE_UP },

	{ 0., .46875, 0., X_COL, FACE_LF },
	{ 0., .46875, 1., X_COL, FACE_LF },
	{ 0., .53125, 0., X_COL, FACE_LF },
	{ 0., .53125, 0., X_COL, FACE_LF },
	{ 0., .46875, 1., X_COL, FACE_LF },
	{ 0., .53125, 1., X_COL, FACE_LF },

	{ 1., .46875, 1., X_COL, FACE_RT },
	{ 1., .46875, 0., X_COL, FACE_RT },
	{ 1., .53125, 1., X_COL, FACE_RT },
	{ 1., .53125, 1., X_COL, FACE_RT },
	{ 1., .46875, 0., X_COL, FACE_RT },
	{ 1., .53125, 0., X_COL, FACE_RT },

	{ 1., .46875, 0., Z_COL, FACE_BK },
	{ 0., .46875, 0., Z_COL, FACE_BK },
	{ 1., .53125, 0., Z_COL, FACE_BK },
	{ 1., .53125, 0., Z_COL, FACE_BK },
	{ 0., .46875, 0., Z_COL, FACE_BK },
	{ 0., .53125, 0., Z_COL, FACE_BK },

	{ 0., .46875, 1., Z_COL, FACE_FT },
	{ 1., .46875, 1., Z_COL, FACE_FT },
	{ 0., .53125, 1., Z_COL, FACE_FT },
	{ 0., .53125, 1., Z_COL, FACE_FT },
	{ 1., .46875, 1., Z_COL, FACE_FT },
	{ 1., .53125, 1., Z_COL, FACE_FT },
};

struct VertexDesc vertices_pane_z[] = {
	{ 0., 0., .46875, B_COL, FACE_DN },
	{ 1., 0., .46875, B_COL, FACE_DN },
	{ 0., 0., .53125, B_COL, FACE_DN },
	{ 0., 0., .53125, B_COL, FACE_DN },
	{ 1., 0., .46875, B_COL, FACE_DN },
	{ 1., 0., .53125, B_COL, FACE_DN },

	{ 1., 1., .53125, T_COL, FACE_UP },
	{ 1., 1., .46875, T_COL, FACE_UP },
	{ 0., 1., .53125, T_COL, FACE_UP },
	{ 0., 1., .53125, T_COL, FACE_UP },
	{ 1., 1., .46875, T_COL, FACE_UP },
	{ 0., 1., .46875, T_COL, FACE_UP },

	{ 0., 0., .46875, X_COL, FACE_LF },
	{ 0., 0., .53125, X_COL, FACE_LF },
	{ 0., 1., .46875, X_COL, FACE_LF },
	{ 0., 1., .46875, X_COL, FACE_LF },
	{ 0., 0., .53125, X_COL, FACE_LF },
	{ 0., 1., .53125, X_COL, FACE_LF },

	{ 1., 0., .53125, X_COL, FACE_RT },
	{ 1., 0., .46875, X_COL, FACE_RT },
	{ 1., 1., .53125, X_COL, FACE_RT },
	{ 1., 1., .53125, X_COL, FACE_RT },
	{ 1., 0., .46875, X_COL, FACE_RT },
	{ 1., 1., .46875, X_COL, FACE_RT },

	{ 1., 0., .46875, Z_COL, FACE_BK },
	{ 0., 0., .46875, Z_COL, FACE_BK },
	{ 1., 1., .46875, Z_COL, FACE_BK },
	{ 1., 1., .46875, Z_COL, FACE_BK },
	{ 0., 0., .46875, Z_COL, FACE_BK },
	{ 0., 1., .46875, Z_COL, FACE_BK },

	{ 0., 0., .53125, Z_COL, FACE_BK },
	{ 1., 0., .53125, Z_COL, FACE_BK },
	{ 0., 1., .53125, Z_COL, FACE_BK },
	{ 0., 1., .53125, Z_COL, FACE_BK },
	{ 1., 0., .53125, Z_COL, FACE_BK },
	{ 1., 1., .53125, Z_COL, FACE_BK },
};

struct VertexDesc vertices_pane_dn[] = {
	{ 1., .0625, 1., T_COL, FACE_UP },
	{ 1., .0625, 0., T_COL, FACE_UP },
	{ 0., .0625, 1., T_COL, FACE_UP },
	{ 0., .0625, 1., T_COL, FACE_UP },
	{ 1., .0625, 0., T_COL, FACE_UP },
	{ 0., .0625, 0., T_COL, FACE_UP },
	{ 0., 0., 0., X_COL, FACE_LF },
	{ 0., 0., 1., X_COL, FACE_LF },
	{ 0., .0625, 0., X_COL, FACE_LF },
	{ 0., .0625, 0., X_COL, FACE_LF },
	{ 0., 0., 1., X_COL, FACE_LF },
	{ 0., .0625, 1., X_COL, FACE_LF },
	{ 1., 0., 1., X_COL, FACE_RT },
	{ 1., 0., 0., X_COL, FACE_RT },
	{ 1., .0625, 1., X_COL, FACE_RT },
	{ 1., .0625, 1., X_COL, FACE_RT },
	{ 1., 0., 0., X_COL, FACE_RT },
	{ 1., .0625, 0., X_COL, FACE_RT },
	{ 1., 0., 0., Z_COL, FACE_BK },
	{ 0., 0., 0., Z_COL, FACE_BK },
	{ 1., .0625, 0., Z_COL, FACE_BK },
	{ 1., .0625, 0., Z_COL, FACE_BK },
	{ 0., 0., 0., Z_COL, FACE_BK },
	{ 0., .0625, 0., Z_COL, FACE_BK },
	{ 0., 0., 1., Z_COL, FACE_FT },
	{ 1., 0., 1., Z_COL, FACE_FT },
	{ 0., .0625, 1., Z_COL, FACE_FT },
	{ 0., .0625, 1., Z_COL, FACE_FT },
	{ 1., 0., 1., Z_COL, FACE_FT },
	{ 1., .0625, 1., Z_COL, FACE_FT },
};

struct VertexDesc vertices_pane_up[] = {
	{ 0., .9375, 0., B_COL, FACE_DN },
	{ 1., .9375, 0., B_COL, FACE_DN },
	{ 0., .9375, 1., B_COL, FACE_DN },
	{ 0., .9375, 1., B_COL, FACE_DN },
	{ 1., .9375, 0., B_COL, FACE_DN },
	{ 1., .9375, 1., B_COL, FACE_DN },
	{ 0., .9375, 0., X_COL, FACE_LF },
	{ 0., .9375, 1., X_COL, FACE_LF },
	{ 0., 1., 0., X_COL, FACE_LF },
	{ 0., 1., 0., X_COL, FACE_LF },
	{ 0., .9375, 1., X_COL, FACE_LF },
	{ 0., 1., 1., X_COL, FACE_LF },
	{ 1., .9375, 1., X_COL, FACE_RT },
	{ 1., .9375, 0., X_COL, FACE_RT },
	{ 1., 1., 1., X_COL, FACE_RT },
	{ 1., 1., 1., X_COL, FACE_RT },
	{ 1., .9375, 0., X_COL, FACE_RT },
	{ 1., 1., 0., X_COL, FACE_RT },
	{ 1., .9375, 0., Z_COL, FACE_BK },
	{ 0., .9375, 0., Z_COL, FACE_BK },
	{ 1., 1., 0., Z_COL, FACE_BK },
	{ 1., 1., 0., Z_COL, FACE_BK },
	{ 0., .9375, 0., Z_COL, FACE_BK },
	{ 0., 1., 0., Z_COL, FACE_BK },
	{ 0., .9375, 1., Z_COL, FACE_FT },
	{ 1., .9375, 1., Z_COL, FACE_FT },
	{ 0., 1., 1., Z_COL, FACE_FT },
	{ 0., 1., 1., Z_COL, FACE_FT },
	{ 1., .9375, 1., Z_COL, FACE_FT },
	{ 1., 1., 1., Z_COL, FACE_FT },
};

struct VertexDesc vertices_pane_lf[] = {
	{ 0., 0., 0., B_COL, FACE_DN },
	{ .0625, 0., 0., B_COL, FACE_DN },
	{ 0., 0., 1., B_COL, FACE_DN },
	{ 0., 0., 1., B_COL, FACE_DN },
	{ .0625, 0., 0., B_COL, FACE_DN },
	{ .0625, 0., 1., B_COL, FACE_DN },
	{ .0625, 1., 1., T_COL, FACE_UP },
	{ .0625, 1., 0., T_COL, FACE_UP },
	{ 0., 1., 1., T_COL, FACE_UP },
	{ 0., 1., 1., T_COL, FACE_UP },
	{ .0625, 1., 0., T_COL, FACE_UP },
	{ 0., 1., 0., T_COL, FACE_UP },
	{ .0625, 0., 1., X_COL, FACE_LF },
	{ .0625, 0., 0., X_COL, FACE_LF },
	{ .0625, 1., 1., X_COL, FACE_LF },
	{ .0625, 1., 1., X_COL, FACE_LF },
	{ .0625, 0., 0., X_COL, FACE_LF },
	{ .0625, 1., 0., X_COL, FACE_LF },
	{ .0625, 0., 0., Z_COL, FACE_BK },
	{ 0., 0., 0., Z_COL, FACE_BK },
	{ .0625, 1., 0., Z_COL, FACE_BK },
	{ .0625, 1., 0., Z_COL, FACE_BK },
	{ 0., 0., 0., Z_COL, FACE_BK },
	{ 0., 1., 0., Z_COL, FACE_BK },
	{ 0., 0., 1., Z_COL, FACE_FT },
	{ .0625, 0., 1., Z_COL, FACE_FT },
	{ 0., 1., 1., Z_COL, FACE_FT },
	{ 0., 1., 1., Z_COL, FACE_FT },
	{ .0625, 0., 1., Z_COL, FACE_FT },
	{ .0625, 1., 1., Z_COL, FACE_FT },
};

struct VertexDesc vertices_pane_rt[] = {
	{ .9375, 0., 0., B_COL, FACE_DN },
	{ 1., 0., 0., B_COL, FACE_DN },
	{ .9375, 0., 1., B_COL, FACE_DN },
	{ .9375, 0., 1., B_COL, FACE_DN },
	{ 1., 0., 0., B_COL, FACE_DN },
	{ 1., 0., 1., B_COL, FACE_DN },
	{ 1., 1., 1., T_COL, FACE_UP },
	{ 1., 1., 0., T_COL, FACE_UP },
	{ .9375, 1., 1., T_COL, FACE_UP },
	{ .9375, 1., 1., T_COL, FACE_UP },
	{ 1., 1., 0., T_COL, FACE_UP },
	{ .9375, 1., 0., T_COL, FACE_UP },
	{ .9375, 0., 0., X_COL, FACE_LF },
	{ .9375, 0., 1., X_COL, FACE_LF },
	{ .9375, 1., 0., X_COL, FACE_LF },
	{ .9375, 1., 0., X_COL, FACE_LF },
	{ .9375, 0., 1., X_COL, FACE_LF },
	{ .9375, 1., 1., X_COL, FACE_LF },
	{ 1., 0., 0., Z_COL, FACE_BK },
	{ .9375, 0., 0., Z_COL, FACE_BK },
	{ 1., 1., 0., Z_COL, FACE_BK },
	{ 1., 1., 0., Z_COL, FACE_BK },
	{ .9375, 0., 0., Z_COL, FACE_BK },
	{ .9375, 1., 0., Z_COL, FACE_BK },
	{ .9375, 0., 1., Z_COL, FACE_FT },
	{ 1., 0., 1., Z_COL, FACE_FT },
	{ .9375, 1., 1., Z_COL, FACE_FT },
	{ .9375, 1., 1., Z_COL, FACE_FT },
	{ 1., 0., 1., Z_COL, FACE_FT },
	{ 1., 1., 1., Z_COL, FACE_FT },
};

struct VertexDesc vertices_pane_bk[] = {
	{ 0., 0., 0., B_COL, FACE_DN },
	{ 1., 0., 0., B_COL, FACE_DN },
	{ 0., 0., .0625, B_COL, FACE_DN },
	{ 0., 0., .0625, B_COL, FACE_DN },
	{ 1., 0., 0., B_COL, FACE_DN },
	{ 1., 0., .0625, B_COL, FACE_DN },
	{ 1., 1., .0625, T_COL, FACE_UP },
	{ 1., 1., 0., T_COL, FACE_UP },
	{ 0., 1., .0625, T_COL, FACE_UP },
	{ 0., 1., .0625, T_COL, FACE_UP },
	{ 1., 1., 0., T_COL, FACE_UP },
	{ 0., 1., 0., T_COL, FACE_UP },
	{ 0., 0., 0., X_COL, FACE_LF },
	{ 0., 0., .0625, X_COL, FACE_LF },
	{ 0., 1., 0., X_COL, FACE_LF },
	{ 0., 1., 0., X_COL, FACE_LF },
	{ 0., 0., .0625, X_COL, FACE_LF },
	{ 0., 1., .0625, X_COL, FACE_LF },
	{ 1., 0., .0625, X_COL, FACE_RT },
	{ 1., 0., 0., X_COL, FACE_RT },
	{ 1., 1., .0625, X_COL, FACE_RT },
	{ 1., 1., .0625, X_COL, FACE_RT },
	{ 1., 0., 0., X_COL, FACE_RT },
	{ 1., 1., 0., X_COL, FACE_RT },
	{ 0., 0., .0625, Z_COL, FACE_BK },
	{ 1., 0., .0625, Z_COL, FACE_BK },
	{ 0., 1., .0625, Z_COL, FACE_BK },
	{ 0., 1., .0625, Z_COL, FACE_BK },
	{ 1., 0., .0625, Z_COL, FACE_BK },
	{ 1., 1., .0625, Z_COL, FACE_BK },
};

struct VertexDesc vertices_pane_ft[] = {
	{ 0., 0., .9375, B_COL, FACE_DN },
	{ 1., 0., .9375, B_COL, FACE_DN },
	{ 0., 0., 1., B_COL, FACE_DN },
	{ 0., 0., 1., B_COL, FACE_DN },
	{ 1., 0., .9375, B_COL, FACE_DN },
	{ 1., 0., 1., B_COL, FACE_DN },
	{ 1., 1., 1., T_COL, FACE_UP },
	{ 1., 1., .9375, T_COL, FACE_UP },
	{ 0., 1., 1., T_COL, FACE_UP },
	{ 0., 1., 1., T_COL, FACE_UP },
	{ 1., 1., .9375, T_COL, FACE_UP },
	{ 0., 1., .9375, T_COL, FACE_UP },
	{ 0., 0., .9375, X_COL, FACE_LF },
	{ 0., 0., 1., X_COL, FACE_LF },
	{ 0., 1., .9375, X_COL, FACE_LF },
	{ 0., 1., .9375, X_COL, FACE_LF },
	{ 0., 0., 1., X_COL, FACE_LF },
	{ 0., 1., 1., X_COL, FACE_LF },
	{ 1., 0., 1., X_COL, FACE_RT },
	{ 1., 0., .9375, X_COL, FACE_RT },
	{ 1., 1., 1., X_COL, FACE_RT },
	{ 1., 1., 1., X_COL, FACE_RT },
	{ 1., 0., .9375, X_COL, FACE_RT },
	{ 1., 1., .9375, X_COL, FACE_RT },
	{ 1., 0., .9375, Z_COL, FACE_BK },
	{ 0., 0., .9375, Z_COL, FACE_BK },
	{ 1., 1., .9375, Z_COL, FACE_BK },
	{ 1., 1., .9375, Z_COL, FACE_BK },
	{ 0., 0., .9375, Z_COL, FACE_BK },
	{ 0., 1., .9375, Z_COL, FACE_BK },
};

struct VertexDesc vertices_token_lf[] = {
	{ 1., .5, 1., T_COL, FACE_UP },
	{ 1., .5, 0., T_COL, FACE_UP },
	{ 0., .5, 1., T_COL, FACE_UP },
	{ 0., .5, 1., T_COL, FACE_UP },
	{ 1., .5, 0., T_COL, FACE_UP },
	{ 0., .5, 0., T_COL, FACE_UP },
	{ 0., 0., 0., X_COL, FACE_LF },
	{ 0., 0., 1., X_COL, FACE_LF },
	{ 0., .5, 0., X_COL, FACE_LF },
	{ 0., .5, 0., X_COL, FACE_LF },
	{ 0., 0., 1., X_COL, FACE_LF },
	{ 0., .5, 1., X_COL, FACE_LF },
	{ 1., 0., 1., X_COL, FACE_RT },
	{ 1., 0., 0., X_COL, FACE_RT },
	{ 1., .5, 1., X_COL, FACE_RT },
	{ 1., .5, 1., X_COL, FACE_RT },
	{ 1., 0., 0., X_COL, FACE_RT },
	{ 1., .5, 0., X_COL, FACE_RT },
	{ 1., 0., 0., Z_COL, FACE_BK },
	{ 0., 0., 0., Z_COL, FACE_BK },
	{ 1., .5, 0., Z_COL, FACE_BK },
	{ 1., .5, 0., Z_COL, FACE_BK },
	{ 0., 0., 0., Z_COL, FACE_BK },
	{ 0., .5, 0., Z_COL, FACE_BK },
	{ 0., 0., 1., Z_COL, FACE_FT },
	{ 1., 0., 1., Z_COL, FACE_FT },
	{ 0., .5, 1., Z_COL, FACE_FT },
	{ 0., .5, 1., Z_COL, FACE_FT },
	{ 1., 0., 1., Z_COL, FACE_FT },
	{ 1., .5, 1., Z_COL, FACE_FT },
};

struct VertexDesc vertices_token_bk[] = {
	{ 1., .5, 1., T_COL, FACE_UP },
	{ 1., .5, 0., T_COL, FACE_UP },
	{ 0., .5, 1., T_COL, FACE_UP },
	{ 0., .5, 1., T_COL, FACE_UP },
	{ 1., .5, 0., T_COL, FACE_UP },
	{ 0., .5, 0., T_COL, FACE_UP },
	{ 0., 0., 0., X_COL, FACE_LF },
	{ 0., 0., 1., X_COL, FACE_LF },
	{ 0., .5, 0., X_COL, FACE_LF },
	{ 0., .5, 0., X_COL, FACE_LF },
	{ 0., 0., 1., X_COL, FACE_LF },
	{ 0., .5, 1., X_COL, FACE_LF },
	{ 1., 0., 1., X_COL, FACE_RT },
	{ 1., 0., 0., X_COL, FACE_RT },
	{ 1., .5, 1., X_COL, FACE_RT },
	{ 1., .5, 1., X_COL, FACE_RT },
	{ 1., 0., 0., X_COL, FACE_RT },
	{ 1., .5, 0., X_COL, FACE_RT },
	{ 1., 0., 0., Z_COL, FACE_BK },
	{ 0., 0., 0., Z_COL, FACE_BK },
	{ 1., .5, 0., Z_COL, FACE_BK },
	{ 1., .5, 0., Z_COL, FACE_BK },
	{ 0., 0., 0., Z_COL, FACE_BK },
	{ 0., .5, 0., Z_COL, FACE_BK },
	{ 0., 0., 1., Z_COL, FACE_FT },
	{ 1., 0., 1., Z_COL, FACE_FT },
	{ 0., .5, 1., Z_COL, FACE_FT },
	{ 0., .5, 1., Z_COL, FACE_FT },
	{ 1., 0., 1., Z_COL, FACE_FT },
	{ 1., .5, 1., Z_COL, FACE_FT },
};

struct VertexDesc vertices_token_rt[] = {
	{ 1., .5, 1., T_COL, FACE_UP },
	{ 1., .5, 0., T_COL, FACE_UP },
	{ 0., .5, 1., T_COL, FACE_UP },
	{ 0., .5, 1., T_COL, FACE_UP },
	{ 1., .5, 0., T_COL, FACE_UP },
	{ 0., .5, 0., T_COL, FACE_UP },
	{ 0., 0., 0., X_COL, FACE_LF },
	{ 0., 0., 1., X_COL, FACE_LF },
	{ 0., .5, 0., X_COL, FACE_LF },
	{ 0., .5, 0., X_COL, FACE_LF },
	{ 0., 0., 1., X_COL, FACE_LF },
	{ 0., .5, 1., X_COL, FACE_LF },
	{ 1., 0., 1., X_COL, FACE_RT },
	{ 1., 0., 0., X_COL, FACE_RT },
	{ 1., .5, 1., X_COL, FACE_RT },
	{ 1., .5, 1., X_COL, FACE_RT },
	{ 1., 0., 0., X_COL, FACE_RT },
	{ 1., .5, 0., X_COL, FACE_RT },
	{ 1., 0., 0., Z_COL, FACE_BK },
	{ 0., 0., 0., Z_COL, FACE_BK },
	{ 1., .5, 0., Z_COL, FACE_BK },
	{ 1., .5, 0., Z_COL, FACE_BK },
	{ 0., 0., 0., Z_COL, FACE_BK },
	{ 0., .5, 0., Z_COL, FACE_BK },
	{ 0., 0., 1., Z_COL, FACE_FT },
	{ 1., 0., 1., Z_COL, FACE_FT },
	{ 0., .5, 1., Z_COL, FACE_FT },
	{ 0., .5, 1., Z_COL, FACE_FT },
	{ 1., 0., 1., Z_COL, FACE_FT },
	{ 1., .5, 1., Z_COL, FACE_FT },
};

struct VertexDesc vertices_token_ft[] = {
	{ 1., .5, 1., T_COL, FACE_UP },
	{ 1., .5, 0., T_COL, FACE_UP },
	{ 0., .5, 1., T_COL, FACE_UP },
	{ 0., .5, 1., T_COL, FACE_UP },
	{ 1., .5, 0., T_COL, FACE_UP },
	{ 0., .5, 0., T_COL, FACE_UP },
	{ 0., 0., 0., X_COL, FACE_LF },
	{ 0., 0., 1., X_COL, FACE_LF },
	{ 0., .5, 0., X_COL, FACE_LF },
	{ 0., .5, 0., X_COL, FACE_LF },
	{ 0., 0., 1., X_COL, FACE_LF },
	{ 0., .5, 1., X_COL, FACE_LF },
	{ 1., 0., 1., X_COL, FACE_RT },
	{ 1., 0., 0., X_COL, FACE_RT },
	{ 1., .5, 1., X_COL, FACE_RT },
	{ 1., .5, 1., X_COL, FACE_RT },
	{ 1., 0., 0., X_COL, FACE_RT },
	{ 1., .5, 0., X_COL, FACE_RT },
	{ 1., 0., 0., Z_COL, FACE_BK },
	{ 0., 0., 0., Z_COL, FACE_BK },
	{ 1., .5, 0., Z_COL, FACE_BK },
	{ 1., .5, 0., Z_COL, FACE_BK },
	{ 0., 0., 0., Z_COL, FACE_BK },
	{ 0., .5, 0., Z_COL, FACE_BK },
	{ 0., 0., 1., Z_COL, FACE_FT },
	{ 1., 0., 1., Z_COL, FACE_FT },
	{ 0., .5, 1., Z_COL, FACE_FT },
	{ 0., .5, 1., Z_COL, FACE_FT },
	{ 1., 0., 1., Z_COL, FACE_FT },
	{ 1., .5, 1., Z_COL, FACE_FT },
};

VertexBuffer::VertexBuffer(int vbo_count) : vbo_count(vbo_count)
{
	vbo_names = new GLuint[vbo_count];
	vbo_sizes = new size_t[vbo_count];
	glGenBuffers(vbo_count, vbo_names);
	memset(vbo_sizes, 0, sizeof(*vbo_sizes) * vbo_count);
	assert(glGetError() == GL_NO_ERROR);
}

VertexBuffer::~VertexBuffer()
{
	assert(glGetError() == GL_NO_ERROR);
	glDeleteBuffers(vbo_count, vbo_names);
	delete [] vbo_names;
	delete [] vbo_sizes;
}

void VertexBuffer::enable()
{
	assert(glGetError() == GL_NO_ERROR);
}

void VertexBuffer::draw_slice(GLenum mode, size_t buf, size_t first,
		size_t count)
{
	assert(buf < vbo_count);
	assert(first + count <= vbo_sizes[buf]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_names[buf]);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex),
		reinterpret_cast<char *>(0));
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex),
		reinterpret_cast<char *>(offsetof(Vertex, u)));
	glColorPointer(4, GL_FLOAT, sizeof(Vertex),
		reinterpret_cast<char *>(offsetof(Vertex, r)));
	glDrawArrays(mode, first, count);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	assert(glGetError() == GL_NO_ERROR);
}

void VertexBuffer::draw(GLenum mode, size_t buf)
{
	draw_slice(mode, buf, 0, vbo_sizes[buf]);
}

void VertexBuffer::disable()
{
	assert(glGetError() == GL_NO_ERROR);
}

void VertexBuffer::update(size_t buf, const void *data, size_t size)
{
	assert(glGetError() == GL_NO_ERROR);
	assert(buf < vbo_count);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_names[buf]);
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(Vertex), data, GL_STATIC_DRAW);
	vbo_sizes[buf] = size;
	assert(glGetError() == GL_NO_ERROR);
}

void vertices_add(std::vector<Vertex> *s, const VertexDesc *buf, size_t len,
		v3f p, v2f t1, const v2f *t2, const int *tilted, World *w)
{
	int i;
	const VertexDesc *d;
	Vertex v;
	for (i = 0; i < len; ++i) {
		d = &buf[i];
		v.x = d->x + p.x;
		v.y = d->y + p.y;
		v.z = d->z + p.z;
		if (d->face == FACE_LF || d->face == FACE_RT) {
			v.u = d->z;
			v.v = 1. - d->y;
		} else if (d->face == FACE_DN || d->face == FACE_UP) {
			v.u = d->z;
			v.v = d->x;
		} else if (d->face == FACE_BK || d->face == FACE_FT) {
			v.u = d->x;
			v.v = 1. - d->y;
		}
		if (tilted[d->face])
			std::swap(v.u, v.v);
		v.u = t2[d->face].x + v.u / 32.;
		v.v = t2[d->face].y + v.v / 32.;
		v.r = 1;
		if (w != nullptr && (d->x == 0 || d->x == 1)
				&& (d->y == 0 || d->y == 1)
				&& (d->z == 0 || d->z == 1)) {
			v3ll q(floor(v.x), floor(v.y), floor(v.z));
			v2f l(0, 0);
			if (d->face == FACE_LF) {
				--q.x;
				l = l + texcoord_from_light(w->get_light(q)) * .25f;
				if (w->get_shape(q) != SHAPE_NONE)
					v.r -= .125;
				--q.y;
				l = l + texcoord_from_light(w->get_light(q)) * .25f;
				if (w->get_shape(q) != SHAPE_NONE)
					v.r -= .125;
				--q.z;
				l = l + texcoord_from_light(w->get_light(q)) * .25f;
				if (w->get_shape(q) != SHAPE_NONE)
					v.r -= .125;
				++q.y;
				l = l + texcoord_from_light(w->get_light(q)) * .25f;
				if (w->get_shape(q) != SHAPE_NONE)
					v.r -= .125;
			} else if (d->face == FACE_RT) {
				l = l + texcoord_from_light(w->get_light(q)) * .25f;
				if (w->get_shape(q) != SHAPE_NONE)
					v.r -= .125;
				--q.y;
				l = l + texcoord_from_light(w->get_light(q)) * .25f;
				if (w->get_shape(q) != SHAPE_NONE)
					v.r -= .125;
				--q.z;
				l = l + texcoord_from_light(w->get_light(q)) * .25f;
				if (w->get_shape(q) != SHAPE_NONE)
					v.r -= .125;
				++q.y;
				l = l + texcoord_from_light(w->get_light(q)) * .25f;
				if (w->get_shape(q) != SHAPE_NONE)
					v.r -= .125;
			} else if (d->face == FACE_DN) {
				--q.y;
				l = l + texcoord_from_light(w->get_light(q)) * .25f;
				if (w->get_shape(q) != SHAPE_NONE)
					v.r -= .125;
				--q.x;
				l = l + texcoord_from_light(w->get_light(q)) * .25f;
				if (w->get_shape(q) != SHAPE_NONE)
					v.r -= .125;
				--q.z;
				l = l + texcoord_from_light(w->get_light(q)) * .25f;
				if (w->get_shape(q) != SHAPE_NONE)
					v.r -= .125;
				++q.x;
				l = l + texcoord_from_light(w->get_light(q)) * .25f;
				if (w->get_shape(q) != SHAPE_NONE)
					v.r -= .125;
			} else if (d->face == FACE_UP) {
				l = l + texcoord_from_light(w->get_light(q)) * .25f;
				if (w->get_shape(q) != SHAPE_NONE)
					v.r -= .125;
				--q.x;
				l = l + texcoord_from_light(w->get_light(q)) * .25f;
				if (w->get_shape(q) != SHAPE_NONE)
					v.r -= .125;
				--q.z;
				l = l + texcoord_from_light(w->get_light(q)) * .25f;
				if (w->get_shape(q) != SHAPE_NONE)
					v.r -= .125;
				++q.x;
				l = l + texcoord_from_light(w->get_light(q)) * .25f;
				if (w->get_shape(q) != SHAPE_NONE)
					v.r -= .125;
			} else if (d->face == FACE_BK) {
				--q.z;
				l = l + texcoord_from_light(w->get_light(q)) * .25f;
				if (w->get_shape(q) != SHAPE_NONE)
					v.r -= .125;
				--q.y;
				l = l + texcoord_from_light(w->get_light(q)) * .25f;
				if (w->get_shape(q) != SHAPE_NONE)
					v.r -= .125;
				--q.x;
				l = l + texcoord_from_light(w->get_light(q)) * .25f;
				if (w->get_shape(q) != SHAPE_NONE)
					v.r -= .125;
				++q.y;
				l = l + texcoord_from_light(w->get_light(q)) * .25f;
				if (w->get_shape(q) != SHAPE_NONE)
					v.r -= .125;
			} else if (d->face == FACE_FT) {
				l = l + texcoord_from_light(w->get_light(q)) * .25f;
				if (w->get_shape(q) != SHAPE_NONE)
					v.r -= .125;
				--q.y;
				l = l + texcoord_from_light(w->get_light(q)) * .25f;
				if (w->get_shape(q) != SHAPE_NONE)
					v.r -= .125;
				--q.x;
				l = l + texcoord_from_light(w->get_light(q)) * .25f;
				if (w->get_shape(q) != SHAPE_NONE)
					v.r -= .125;
				++q.y;
				l = l + texcoord_from_light(w->get_light(q)) * .25f;
				if (w->get_shape(q) != SHAPE_NONE)
					v.r -= .125;
			}
			v.g = l.x;
			v.b = l.y;
		} else {
			v.g = t1.x;
			v.b = t1.y;
		}
		v.r *= d->c;
		v.a = 1;
		s->push_back(v);
	}
}

void texcoord_up(int mat, v2f *ret, int *tilted)
{
	ret[FACE_UP] = texcoord_from_mat[mat][1];
	ret[FACE_DN] = texcoord_from_mat[mat][2];
	ret[FACE_LF] = texcoord_from_mat[mat][0];
	ret[FACE_RT] = texcoord_from_mat[mat][0];
	ret[FACE_BK] = texcoord_from_mat[mat][0];
	ret[FACE_FT] = texcoord_from_mat[mat][0];
	tilted[FACE_UP] = 0;
	tilted[FACE_DN] = 0;
	tilted[FACE_LF] = 0;
	tilted[FACE_RT] = 0;
	tilted[FACE_BK] = 0;
	tilted[FACE_FT] = 0;
}

void texcoord_dn(int mat, v2f *ret, int *tilted)
{
	ret[FACE_UP] = texcoord_from_mat[mat][2];
	ret[FACE_DN] = texcoord_from_mat[mat][1];
	ret[FACE_LF] = texcoord_from_mat[mat][0];
	ret[FACE_RT] = texcoord_from_mat[mat][0];
	ret[FACE_BK] = texcoord_from_mat[mat][0];
	ret[FACE_FT] = texcoord_from_mat[mat][0];
	tilted[FACE_UP] = 0;
	tilted[FACE_DN] = 0;
	tilted[FACE_LF] = 0;
	tilted[FACE_RT] = 0;
	tilted[FACE_BK] = 0;
	tilted[FACE_FT] = 0;
}

void texcoord_lf(int mat, v2f *ret, int *tilted)
{
	ret[FACE_UP] = texcoord_from_mat[mat][0];
	ret[FACE_DN] = texcoord_from_mat[mat][0];
	ret[FACE_LF] = texcoord_from_mat[mat][2];
	ret[FACE_RT] = texcoord_from_mat[mat][1];
	ret[FACE_BK] = texcoord_from_mat[mat][0];
	ret[FACE_FT] = texcoord_from_mat[mat][0];
	tilted[FACE_UP] = 0;
	tilted[FACE_DN] = 1;
	tilted[FACE_LF] = 0;
	tilted[FACE_RT] = 0;
	tilted[FACE_BK] = 1;
	tilted[FACE_FT] = 1;
}

void texcoord_rt(int mat, v2f *ret, int *tilted)
{
	ret[FACE_UP] = texcoord_from_mat[mat][0];
	ret[FACE_DN] = texcoord_from_mat[mat][0];
	ret[FACE_LF] = texcoord_from_mat[mat][1];
	ret[FACE_RT] = texcoord_from_mat[mat][2];
	ret[FACE_BK] = texcoord_from_mat[mat][0];
	ret[FACE_FT] = texcoord_from_mat[mat][0];
	tilted[FACE_UP] = 0;
	tilted[FACE_DN] = 1;
	tilted[FACE_LF] = 0;
	tilted[FACE_RT] = 0;
	tilted[FACE_BK] = 1;
	tilted[FACE_FT] = 1;
}

void texcoord_bk(int mat, v2f *ret, int *tilted)
{
	ret[FACE_UP] = texcoord_from_mat[mat][0];
	ret[FACE_DN] = texcoord_from_mat[mat][0];
	ret[FACE_LF] = texcoord_from_mat[mat][0];
	ret[FACE_RT] = texcoord_from_mat[mat][0];
	ret[FACE_BK] = texcoord_from_mat[mat][1];
	ret[FACE_FT] = texcoord_from_mat[mat][2];
	tilted[FACE_UP] = 1;
	tilted[FACE_DN] = 0;
	tilted[FACE_LF] = 1;
	tilted[FACE_RT] = 1;
	tilted[FACE_BK] = 0;
	tilted[FACE_FT] = 0;
}

void texcoord_ft(int mat, v2f *ret, int *tilted)
{
	ret[FACE_UP] = texcoord_from_mat[mat][0];
	ret[FACE_DN] = texcoord_from_mat[mat][0];
	ret[FACE_LF] = texcoord_from_mat[mat][0];
	ret[FACE_RT] = texcoord_from_mat[mat][0];
	ret[FACE_BK] = texcoord_from_mat[mat][1];
	ret[FACE_FT] = texcoord_from_mat[mat][2];
	tilted[FACE_UP] = 1;
	tilted[FACE_DN] = 0;
	tilted[FACE_LF] = 1;
	tilted[FACE_RT] = 1;
	tilted[FACE_BK] = 0;
	tilted[FACE_FT] = 0;
}


