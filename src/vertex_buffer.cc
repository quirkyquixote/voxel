/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "vertex_buffer.h"

#include <assert.h>
#include <stdlib.h>

#include "types.h"

#define T_COL 1.
#define X_COL .8
#define Z_COL .6
#define B_COL .4

#define TC(mat,a0,b0,a1,b1,a2,b2)\
do {\
	texcoord_from_mat[mat][0] = v2f(a0 / 32., b0 / 32.);\
	texcoord_from_mat[mat][1] = v2f(a1 / 32., b1 / 32.);\
	texcoord_from_mat[mat][2] = v2f(a2 / 32., b2 / 32.);\
} while (0)

#define COMMON_TC(mat,y) \
	TC(mat##_CRATE, 16, y, 17, y, 0, y);\
	TC(mat##_BENCH, 18, y, 19, y, 0, y);\
	TC(mat##_PIPE, 20, y, 21, y, 21, y);\
	TC(mat##_BOARD, 22, y, 23, y, 0, y)

#define STONE_TC(mat,y)\
	TC(mat, 0, y, 0, y, 0, y);\
	TC(mat##_COBBLE, 1, y, 1, y, 1, y);\
	TC(mat##_LBRICK, 6, y, 7, y, 8, y);\
	TC(mat##_SBRICK, 3, y, 4, y, 5, y);\
	TC(mat##_TILE, 2, y, 2, y, 2, y);\
	TC(mat##_COLUMN, 9, y, 10, y, 10, y);\
	TC(mat##_BLOCK, 7, y, 7, y, 7, y);\
	TC(mat##_SAND, 15, y, 15, y, 16, y);\
	COMMON_TC(mat, y)

#define WOOD_TC(mat,y)\
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
	{ 0., 0., 0., 0., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., 0., 0., 1., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ 0., 0., 1., 0., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ 0., 0., 1., 0., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., 0., 0., 1., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., 0., 1., 1., 0., B_COL, B_COL, B_COL, FACE_DN },
};

struct VertexDesc vertices_face_up[] = {
	{ 1., 1., 1., 0., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 1., 1., 0., 1., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., 1., 1., 0., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., 1., 1., 0., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 1., 1., 0., 1., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., 1., 0., 1., 0., T_COL, T_COL, T_COL, FACE_UP },
};

struct VertexDesc vertices_face_lf[] = {
	{ 0., 0., 0., 0., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 0., 1., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 1., 0., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 1., 0., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 0., 1., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 1., 1., 1., 0., X_COL, X_COL, X_COL, FACE_LF },
};

struct VertexDesc vertices_face_rt[] = {
	{ 1., 0., 1., 0., 1., X_COL, X_COL, X_COL, FACE_RT },
	{ 1., 0., 0., 1., 1., X_COL, X_COL, X_COL, FACE_RT },
	{ 1., 1., 1., 0., 0., X_COL, X_COL, X_COL, FACE_RT },
	{ 1., 1., 1., 0., 0., X_COL, X_COL, X_COL, FACE_RT },
	{ 1., 0., 0., 1., 1., X_COL, X_COL, X_COL, FACE_RT },
	{ 1., 1., 0., 1., 0., X_COL, X_COL, X_COL, FACE_RT },
};

struct VertexDesc vertices_face_bk[] = {
	{ 1., 0., 0., 0., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., 0., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 1., 0., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 1., 0., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., 0., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 1., 0., 1., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
};

struct VertexDesc vertices_face_ft[] = {
	{ 0., 0., 1., 0., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 0., 1., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 1., 1., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 1., 1., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 0., 1., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 1., 1., 1., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
};

struct VertexDesc vertices_slab_dn[] = {
	{ 1., .5, 1., 0., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 1., .5, 0., 1., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., .5, 1., 0., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., .5, 1., 0., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 1., .5, 0., 1., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., .5, 0., 1., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., 0., 0., 0., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 0., 1., 1., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 0., .5, 0., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., .5, 0., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 0., 1., 1., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 0., .5, 1., 1., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 1., 0., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 0., 1., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .5, 1., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .5, 1., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 0., 1., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .5, 0., 1., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 0., 0., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., 0., 1., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., .5, 0., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., .5, 0., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., 0., 1., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., .5, 0., 1., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., 1., 0., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 0., 1., 1., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., .5, 1., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., .5, 1., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 0., 1., 1., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., .5, 1., 1., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
};

struct VertexDesc vertices_slab_up[] = {
	{ 0., .5, 0., 0., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., .5, 0., 1., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ 0., .5, 1., 0., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ 0., .5, 1., 0., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., .5, 0., 1., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., .5, 1., 1., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ 0., .5, 0., 0., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., .5, 1., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 1., 0., 0., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 1., 0., 0., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 0., .5, 1., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 1., 1., 1., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .5, 1., 0., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .5, 0., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 1., 1., 0., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 1., 1., 0., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .5, 0., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 1., 0., 1., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .5, 0., 0., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., .5, 0., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 1., 0., 0., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 1., 0., 0., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., .5, 0., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 1., 0., 1., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., .5, 1., 0., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., .5, 1., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 1., 1., 0., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 1., 1., 0., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., .5, 1., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 1., 1., 1., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
};

struct VertexDesc vertices_slab_lf[] = {
	{ 0., 0., 0., 0., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ .5, 0., 0., 1., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ 0., 0., 1., 0., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ 0., 0., 1., 0., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ .5, 0., 0., 1., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ .5, 0., 1., 1., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ .5, 1., 1., 0., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ .5, 1., 0., 1., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., 1., 1., 0., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., 1., 1., 0., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ .5, 1., 0., 1., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., 1., 0., 1., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ .5, 0., 1., 0., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ .5, 0., 0., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ .5, 1., 1., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ .5, 1., 1., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ .5, 0., 0., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ .5, 1., 0., 1., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ .5, 0., 0., 0., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., 0., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .5, 1., 0., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .5, 1., 0., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., 0., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 1., 0., 1., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., 1., 0., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .5, 0., 1., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 1., 1., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 1., 1., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .5, 0., 1., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .5, 1., 1., 1., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
};

struct VertexDesc vertices_slab_rt[] = {
	{ .5, 0., 0., 0., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., 0., 0., 1., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ .5, 0., 1., 0., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ .5, 0., 1., 0., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., 0., 0., 1., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., 0., 1., 1., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., 1., 1., 0., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 1., 1., 0., 1., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ .5, 1., 1., 0., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ .5, 1., 1., 0., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 1., 1., 0., 1., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ .5, 1., 0., 1., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ .5, 0., 0., 0., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ .5, 0., 1., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ .5, 1., 0., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ .5, 1., 0., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ .5, 0., 1., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ .5, 1., 1., 1., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 0., 0., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .5, 0., 0., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 1., 0., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 1., 0., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .5, 0., 0., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .5, 1., 0., 1., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .5, 0., 1., 0., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 0., 1., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .5, 1., 1., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .5, 1., 1., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 0., 1., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 1., 1., 1., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
};

struct VertexDesc vertices_slab_bk[] = {
	{ 0., 0., 0., 0., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., 0., 0., 1., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ 0., 0., .5, 0., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ 0., 0., .5, 0., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., 0., 0., 1., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., 0., .5, 1., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., 1., .5, 0., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 1., 1., 0., 1., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., 1., .5, 0., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., 1., .5, 0., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 1., 1., 0., 1., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., 1., 0., 1., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., 0., 0., 0., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 0., .5, 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 1., 0., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 1., 0., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 0., .5, 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 1., .5, 1., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., .5, 0., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 0., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 1., .5, 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 1., .5, 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 0., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 1., 0., 1., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 0., .5, 0., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 0., .5, 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 1., .5, 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 1., .5, 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 0., .5, 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 1., .5, 1., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
};

struct VertexDesc vertices_slab_ft[] = {
	{ 0., 0., .5, 0., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., 0., .5, 1., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ 0., 0., 1., 0., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ 0., 0., 1., 0., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., 0., .5, 1., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., 0., 1., 1., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., 1., 1., 0., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 1., 1., .5, 1., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., 1., 1., 0., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., 1., 1., 0., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 1., 1., .5, 1., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., 1., .5, 1., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., 0., .5, 0., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 0., 1., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 1., .5, 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 1., .5, 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 0., 1., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 1., 1., 1., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 1., 0., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., .5, 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 1., 1., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 1., 1., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., .5, 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 1., .5, 1., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., .5, 0., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., .5, 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 1., .5, 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 1., .5, 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., .5, 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 1., .5, 1., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
};

struct VertexDesc vertices_stairs_dl[] = {
	{ 1., .5, 1., 0., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 1., .5, 0., 1., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ .5, .5, 1., 0., .5, T_COL, T_COL, T_COL, FACE_UP },
	{ .5, .5, 1., 0., .5, T_COL, T_COL, T_COL, FACE_UP },
	{ 1., .5, 0., 1., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ .5, .5, 0., 1., .5, T_COL, T_COL, T_COL, FACE_UP },
	{ .5, 1., 1., 0., .5, T_COL, T_COL, T_COL, FACE_UP },
	{ .5, 1., 0., 1., .5, T_COL, T_COL, T_COL, FACE_UP },
	{ 0., 1., 1., 0., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., 1., 1., 0., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ .5, 1., 0., 1., .5, T_COL, T_COL, T_COL, FACE_UP },
	{ 0., 1., 0., 1., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 1., 0., 1., 0., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 0., 1., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .5, 1., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .5, 1., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 0., 1., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .5, 0., 1., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ .5, .5, 1., 0., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ .5, .5, 0., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ .5, 1., 1., 0., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ .5, 1., 1., 0., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ .5, .5, 0., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ .5, 1., 0., 1., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 0., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., 0., 0., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 1., 0., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 0., 0., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .5, .5, 0., .5, .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., .5, 0., 1., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .5, .5, 0., .5, .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 1., 0., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .5, 1., 0., .5, 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., 1., 0., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 0., 1., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 1., 1., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .5, .5, 1., .5, .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 0., 1., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., .5, 1., 1., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 1., 1., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .5, .5, 1., .5, .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .5, 1., 1., .5, 0., Z_COL, Z_COL, Z_COL, FACE_BK },
};

struct VertexDesc vertices_stairs_dr[] = {
	{ 0., .5, 0., 1., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., .5, 1., 0., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ .5, .5, 1., 0., .5, T_COL, T_COL, T_COL, FACE_UP },
	{ 0., .5, 0., 1., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ .5, .5, 1., 0., .5, T_COL, T_COL, T_COL, FACE_UP },
	{ .5, .5, 0., 1., .5, T_COL, T_COL, T_COL, FACE_UP },
	{ .5, 1., 0., 1., .5, T_COL, T_COL, T_COL, FACE_UP },
	{ .5, 1., 1., 0., .5, T_COL, T_COL, T_COL, FACE_UP },
	{ 1., 1., 1., 0., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ .5, 1., 0., 1., .5, T_COL, T_COL, T_COL, FACE_UP },
	{ 1., 1., 1., 0., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 1., 1., 0., 1., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., 0., 0., 1., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 0., 1., 0., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 0., .5, 1., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 0., 0., 1., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 0., .5, 1., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., .5, 0., 1., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ .5, .5, 0., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ .5, .5, 1., 0., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ .5, 1., 1., 0., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ .5, .5, 0., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ .5, 1., 1., 0., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ .5, 1., 0., 1., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 0., 0., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., 0., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 1., 0., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .5, .5, 0., .5, .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., 0., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., .5, 0., 1., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 1., 0., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .5, .5, 0., .5, .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .5, 1., 0., .5, 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., 1., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 0., 1., 0., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 1., 1., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., 1., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .5, .5, 1., .5, .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., .5, 1., 1., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .5, .5, 1., .5, .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 1., 1., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .5, 1., 1., .5, 0., Z_COL, Z_COL, Z_COL, FACE_BK },
};

struct VertexDesc vertices_stairs_db[] = {
	{ 0., .5, 1., 1., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 1., .5, 1., 0., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 1., .5, .5, 0., .5, T_COL, T_COL, T_COL, FACE_UP },
	{ 0., .5, 1., 1., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 1., .5, .5, 0., .5, T_COL, T_COL, T_COL, FACE_UP },
	{ 0., .5, .5, 1., .5, T_COL, T_COL, T_COL, FACE_UP },
	{ 0., 1., .5, 1., .5, T_COL, T_COL, T_COL, FACE_UP },
	{ 1., 1., .5, 0., .5, T_COL, T_COL, T_COL, FACE_UP },
	{ 1., 1., 0., 0., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., 1., .5, 1., .5, T_COL, T_COL, T_COL, FACE_UP },
	{ 1., 1., 0., 0., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., 1., 0., 1., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., 0., 1., 1., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 0., 1., 0., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., .5, 1., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., 1., 1., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., .5, 1., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., .5, 1., 1., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., .5, .5, 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., .5, .5, 0., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 1., .5, 0., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., .5, .5, 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 1., .5, 0., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 1., .5, 1., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., 0., 0., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 0., 1., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 1., 0., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., .5, .5, .5, .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 0., 1., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., .5, 1., 1., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 1., 0., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., .5, .5, .5, .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 1., .5, .5, 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 1., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 0., 0., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 1., 0., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 1., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .5, .5, .5, .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .5, 1., 1., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .5, .5, .5, .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 1., 0., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 1., .5, .5, 0., X_COL, X_COL, X_COL, FACE_LF },
};

struct VertexDesc vertices_stairs_df[] = {
	{ 1., .5, 0., 0., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., .5, 0., 1., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 1., .5, .5, 0., .5, T_COL, T_COL, T_COL, FACE_UP },
	{ 1., .5, .5, 0., .5, T_COL, T_COL, T_COL, FACE_UP },
	{ 0., .5, 0., 1., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., .5, .5, 1., .5, T_COL, T_COL, T_COL, FACE_UP },
	{ 1., 1., .5, 0., .5, T_COL, T_COL, T_COL, FACE_UP },
	{ 0., 1., .5, 1., .5, T_COL, T_COL, T_COL, FACE_UP },
	{ 1., 1., 1., 0., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 1., 1., 1., 0., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., 1., .5, 1., .5, T_COL, T_COL, T_COL, FACE_UP },
	{ 0., 1., 1., 1., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 1., 0., 0., 0., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., 0., 1., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., .5, 0., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., .5, 0., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., 0., 1., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., .5, 0., 1., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., .5, .5, 0., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., .5, .5, 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 1., .5, 0., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 1., .5, 0., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., .5, .5, 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 1., .5, 1., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., 0., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 0., 1., 0., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 1., 1., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 0., 0., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., .5, .5, .5, .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 0., .5, 0., 1., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 0., .5, .5, .5, .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 1., 1., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 1., .5, .5, 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 1., 0., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 0., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 1., 1., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .5, .5, .5, .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 0., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .5, 0., 1., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 1., 1., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .5, .5, .5, .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 1., .5, .5, 0., X_COL, X_COL, X_COL, FACE_LF },
};

struct VertexDesc vertices_stairs_ul[] = {
	{ 1., .5, 0., 1., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., .5, 1., 0., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ .5, .5, 1., 0., .5, B_COL, B_COL, B_COL, FACE_DN },
	{ 1., .5, 0., 1., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ .5, .5, 1., 0., .5, B_COL, B_COL, B_COL, FACE_DN },
	{ .5, .5, 0., 1., .5, B_COL, B_COL, B_COL, FACE_DN },
	{ .5, 0., 0., 1., .5, B_COL, B_COL, B_COL, FACE_DN },
	{ .5, 0., 1., 0., .5, B_COL, B_COL, B_COL, FACE_DN },
	{ 0., 0., 1., 0., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ .5, 0., 0., 1., .5, B_COL, B_COL, B_COL, FACE_DN },
	{ 0., 0., 1., 0., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ 0., 0., 0., 1., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., 1., 0., 1., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 1., 1., 0., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .5, 1., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 1., 0., 1., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .5, 1., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .5, 0., 1., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ .5, .5, 0., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ .5, .5, 1., 0., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ .5, 0., 1., 0., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ .5, .5, 0., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ .5, 0., 1., 0., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ .5, 0., 0., 1., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 1., 0., 0., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 1., 0., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., 0., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .5, .5, 0., .5, .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 1., 0., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., .5, 0., 1., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., 0., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .5, .5, 0., .5, .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .5, 0., 0., .5, 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 1., 1., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 1., 1., 0., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., 1., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 1., 1., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .5, .5, 1., .5, .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., .5, 1., 1., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .5, .5, 1., .5, .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., 1., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .5, 0., 1., .5, 0., Z_COL, Z_COL, Z_COL, FACE_BK },
};

struct VertexDesc vertices_stairs_ur[] = {
	{ 0., .5, 1., 0., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ 0., .5, 0., 1., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ .5, .5, 1., 0., .5, B_COL, B_COL, B_COL, FACE_DN },
	{ .5, .5, 1., 0., .5, B_COL, B_COL, B_COL, FACE_DN },
	{ 0., .5, 0., 1., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ .5, .5, 0., 1., .5, B_COL, B_COL, B_COL, FACE_DN },
	{ .5, 0., 1., 0., .5, B_COL, B_COL, B_COL, FACE_DN },
	{ .5, 0., 0., 1., .5, B_COL, B_COL, B_COL, FACE_DN },
	{ 1., 0., 1., 0., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., 0., 1., 0., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ .5, 0., 0., 1., .5, B_COL, B_COL, B_COL, FACE_DN },
	{ 1., 0., 0., 1., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ 0., 1., 1., 0., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 1., 0., 1., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 0., .5, 1., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., .5, 1., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 1., 0., 1., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 0., .5, 0., 1., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ .5, .5, 1., 0., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ .5, .5, 0., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ .5, 0., 1., 0., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ .5, 0., 1., 0., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ .5, .5, 0., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ .5, 0., 0., 1., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 1., 0., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 1., 0., 0., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 0., 0., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 1., 0., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .5, .5, 0., .5, .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., .5, 0., 1., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .5, .5, 0., .5, .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 0., 0., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .5, 0., 0., .5, 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 1., 1., 0., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 1., 1., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 0., 1., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .5, .5, 1., .5, .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 1., 1., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., .5, 1., 1., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 0., 1., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .5, .5, 1., .5, .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .5, 0., 1., .5, 0., Z_COL, Z_COL, Z_COL, FACE_BK },
};

struct VertexDesc vertices_stairs_ub[] = {
	{ 1., .5, 1., 0., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ 0., .5, 1., 1., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., .5, .5, 0., .5, B_COL, B_COL, B_COL, FACE_DN },
	{ 1., .5, .5, 0., .5, B_COL, B_COL, B_COL, FACE_DN },
	{ 0., .5, 1., 1., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ 0., .5, .5, 1., .5, B_COL, B_COL, B_COL, FACE_DN },
	{ 1., 0., .5, 0., .5, B_COL, B_COL, B_COL, FACE_DN },
	{ 0., 0., .5, 1., .5, B_COL, B_COL, B_COL, FACE_DN },
	{ 1., 0., 0., 0., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., 0., 0., 0., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ 0., 0., .5, 1., .5, B_COL, B_COL, B_COL, FACE_DN },
	{ 0., 0., 0., 1., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., 1., 1., 0., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 1., 1., 1., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., .5, 1., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., .5, 1., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 1., 1., 1., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., .5, 1., 1., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., .5, .5, 0., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., .5, .5, 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 0., .5, 0., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 0., .5, 0., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., .5, .5, 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., .5, 1., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 1., 1., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 1., 0., 0., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 0., 0., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 1., 1., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., .5, .5, .5, .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 0., .5, 1., 1., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 0., .5, .5, .5, .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 0., 0., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 0., .5, .5, 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 1., 0., 0., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 1., 1., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 0., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .5, .5, .5, .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 1., 1., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .5, 1., 1., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 0., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .5, .5, .5, .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., .5, .5, 0., X_COL, X_COL, X_COL, FACE_LF },
};

struct VertexDesc vertices_stairs_uf[] = {
	{ 0., .5, 0., 1., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., .5, 0., 0., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., .5, .5, 0., .5, B_COL, B_COL, B_COL, FACE_DN },
	{ 0., .5, 0., 1., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., .5, .5, 0., .5, B_COL, B_COL, B_COL, FACE_DN },
	{ 0., .5, .5, 1., .5, B_COL, B_COL, B_COL, FACE_DN },
	{ 0., 0., .5, 1., .5, B_COL, B_COL, B_COL, FACE_DN },
	{ 1., 0., .5, 0., .5, B_COL, B_COL, B_COL, FACE_DN },
	{ 1., 0., 1., 0., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ 0., 0., .5, 1., .5, B_COL, B_COL, B_COL, FACE_DN },
	{ 1., 0., 1., 0., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ 0., 0., 1., 1., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ 0., 1., 0., 1., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 1., 0., 0., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., .5, 0., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 1., 0., 1., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., .5, 0., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., .5, 0., 1., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., .5, .5, 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., .5, .5, 0., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 0., .5, 0., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., .5, .5, 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 0., .5, 0., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., .5, 1., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 1., 1., 0., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 1., 0., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 0., 1., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., .5, .5, .5, .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 1., 0., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., .5, 0., 1., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 0., 1., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., .5, .5, .5, .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 0., .5, .5, 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 1., 0., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 1., 1., 0., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 1., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 1., 0., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .5, .5, .5, .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .5, 0., 1., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .5, .5, .5, .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 1., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., .5, .5, 0., X_COL, X_COL, X_COL, FACE_LF },
};

struct VertexDesc vertices_pane_x[] = {
	{ .46875, 0., 0., 0., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ .53125, 0., 0., 1., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ .46875, 0., 1., 0., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ .46875, 0., 1., 0., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ .53125, 0., 0., 1., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ .53125, 0., 1., 1., 0., B_COL, B_COL, B_COL, FACE_DN },

	{ .53125, 1., 1., 0., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ .53125, 1., 0., 1., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ .46875, 1., 1., 0., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ .46875, 1., 1., 0., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ .53125, 1., 0., 1., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ .46875, 1., 0., 1., 0., T_COL, T_COL, T_COL, FACE_UP },

	{ .46875, 0., 0., 0., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ .46875, 0., 1., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ .46875, 1., 0., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ .46875, 1., 0., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ .46875, 0., 1., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ .46875, 1., 1., 1., 0., X_COL, X_COL, X_COL, FACE_LF },

	{ .53125, 0., 1., 0., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ .53125, 0., 0., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ .53125, 1., 1., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ .53125, 1., 1., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ .53125, 0., 0., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ .53125, 1., 0., 1., 0., X_COL, X_COL, X_COL, FACE_LF },

	{ .53125, 0., 0., 0., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .46875, 0., 0., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .53125, 1., 0., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .53125, 1., 0., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .46875, 0., 0., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .46875, 1., 0., 1., 0., Z_COL, Z_COL, Z_COL, FACE_BK },

	{ .46875, 0., 1., 0., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .53125, 0., 1., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .46875, 1., 1., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .46875, 1., 1., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .53125, 0., 1., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .53125, 1., 1., 1., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
};

struct VertexDesc vertices_pane_y[] = {
	{ 0., .46875, 0., 0., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., .46875, 0., 1., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ 0., .46875, 1., 0., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ 0., .46875, 1., 0., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., .46875, 0., 1., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., .46875, 1., 1., 0., B_COL, B_COL, B_COL, FACE_DN },

	{ 1., .53125, 1., 0., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 1., .53125, 0., 1., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., .53125, 1., 0., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., .53125, 1., 0., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 1., .53125, 0., 1., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., .53125, 0., 1., 0., T_COL, T_COL, T_COL, FACE_UP },

	{ 0., .46875, 0., 0., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., .46875, 1., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., .53125, 0., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., .53125, 0., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., .46875, 1., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., .53125, 1., 1., 0., X_COL, X_COL, X_COL, FACE_LF },

	{ 1., .46875, 1., 0., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .46875, 0., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .53125, 1., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .53125, 1., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .46875, 0., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .53125, 0., 1., 0., X_COL, X_COL, X_COL, FACE_LF },

	{ 1., .46875, 0., 0., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., .46875, 0., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., .53125, 0., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., .53125, 0., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., .46875, 0., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., .53125, 0., 1., 0., Z_COL, Z_COL, Z_COL, FACE_BK },

	{ 0., .46875, 1., 0., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., .46875, 1., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., .53125, 1., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., .53125, 1., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., .46875, 1., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., .53125, 1., 1., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
};

struct VertexDesc vertices_pane_z[] = {
	{ 0., 0., .46875, 0., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., 0., .46875, 1., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ 0., 0., .53125, 0., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ 0., 0., .53125, 0., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., 0., .46875, 1., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., 0., .53125, 1., 0., B_COL, B_COL, B_COL, FACE_DN },

	{ 1., 1., .53125, 0., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 1., 1., .46875, 1., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., 1., .53125, 0., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., 1., .53125, 0., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 1., 1., .46875, 1., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., 1., .46875, 1., 0., T_COL, T_COL, T_COL, FACE_UP },

	{ 0., 0., .46875, 0., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 0., .53125, 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 1., .46875, 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 1., .46875, 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 0., .53125, 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 1., .53125, 1., 0., X_COL, X_COL, X_COL, FACE_LF },

	{ 1., 0., .53125, 0., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., .46875, 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 1., .53125, 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 1., .53125, 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., .46875, 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 1., .46875, 1., 0., X_COL, X_COL, X_COL, FACE_LF },

	{ 1., 0., .46875, 0., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., .46875, 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 1., .46875, 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 1., .46875, 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., .46875, 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 1., .46875, 1., 0., Z_COL, Z_COL, Z_COL, FACE_BK },

	{ 0., 0., .53125, 0., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 0., .53125, 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 1., .53125, 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 1., .53125, 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 0., .53125, 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 1., .53125, 1., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
};

struct VertexDesc vertices_pane_dn[] = {
	{ 1., .0625, 1., 0., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 1., .0625, 0., 1., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., .0625, 1., 0., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., .0625, 1., 0., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 1., .0625, 0., 1., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., .0625, 0., 1., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., 0., 0., 0., .0625, X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 0., 1., 1., .0625, X_COL, X_COL, X_COL, FACE_LF },
	{ 0., .0625, 0., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., .0625, 0., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 0., 1., 1., .0625, X_COL, X_COL, X_COL, FACE_LF },
	{ 0., .0625, 1., 1., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 1., 0., .0625, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 0., 1., .0625, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .0625, 1., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .0625, 1., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 0., 1., .0625, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .0625, 0., 1., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 0., 0., .0625, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., 0., 1., .0625, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., .0625, 0., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., .0625, 0., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., 0., 1., .0625, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., .0625, 0., 1., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., 1., 0., .0625, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 0., 1., 1., .0625, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., .0625, 1., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., .0625, 1., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 0., 1., 1., .0625, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., .0625, 1., 1., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
};

struct VertexDesc vertices_pane_up[] = {
	{ 0., .9375, 0., 0., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., .9375, 0., 1., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ 0., .9375, 1., 0., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ 0., .9375, 1., 0., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., .9375, 0., 1., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., .9375, 1., 1., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ 0., .9375, 0., 0., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., .9375, 1., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 1., 0., 0., .9375, X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 1., 0., 0., .9375, X_COL, X_COL, X_COL, FACE_LF },
	{ 0., .9375, 1., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 1., 1., 1., .9375, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .9375, 1., 0., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .9375, 0., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 1., 1., 0., .9375, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 1., 1., 0., .9375, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .9375, 0., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 1., 0., 1., .9375, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .9375, 0., 0., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., .9375, 0., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 1., 0., 0., .9375, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 1., 0., 0., .9375, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., .9375, 0., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 1., 0., 1., .9375, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., .9375, 1., 0., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., .9375, 1., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 1., 1., 0., .9375, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 1., 1., 0., .9375, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., .9375, 1., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 1., 1., 1., .9375, Z_COL, Z_COL, Z_COL, FACE_BK },
};

struct VertexDesc vertices_pane_lf[] = {
	{ 0., 0., 0., 0., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ .0625, 0., 0., 1., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ 0., 0., 1., 0., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ 0., 0., 1., 0., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ .0625, 0., 0., 1., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ .0625, 0., 1., 1., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ .0625, 1., 1., 0., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ .0625, 1., 0., 1., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., 1., 1., 0., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., 1., 1., 0., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ .0625, 1., 0., 1., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., 1., 0., 1., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ .0625, 0., 1., 0., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ .0625, 0., 0., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ .0625, 1., 1., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ .0625, 1., 1., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ .0625, 0., 0., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ .0625, 1., 0., 1., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ .0625, 0., 0., 0., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., 0., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .0625, 1., 0., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .0625, 1., 0., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., 0., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 1., 0., 1., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., 1., 0., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .0625, 0., 1., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 1., 1., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 1., 1., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .0625, 0., 1., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .0625, 1., 1., 1., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
};

struct VertexDesc vertices_pane_rt[] = {
	{ .9375, 0., 0., 0., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., 0., 0., 1., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ .9375, 0., 1., 0., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ .9375, 0., 1., 0., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., 0., 0., 1., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., 0., 1., 1., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., 1., 1., 0., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 1., 1., 0., 1., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ .9375, 1., 1., 0., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ .9375, 1., 1., 0., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 1., 1., 0., 1., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ .9375, 1., 0., 1., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ .9375, 0., 0., 0., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ .9375, 0., 1., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ .9375, 1., 0., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ .9375, 1., 0., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ .9375, 0., 1., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ .9375, 1., 1., 1., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 0., 0., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .9375, 0., 0., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 1., 0., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 1., 0., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .9375, 0., 0., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .9375, 1., 0., 1., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .9375, 0., 1., 0., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 0., 1., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .9375, 1., 1., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ .9375, 1., 1., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 0., 1., 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 1., 1., 1., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
};

struct VertexDesc vertices_pane_bk[] = {
	{ 0., 0., 0., 0., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., 0., 0., 1., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ 0., 0., .0625, 0., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ 0., 0., .0625, 0., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., 0., 0., 1., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., 0., .0625, 1., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., 1., .0625, 0., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 1., 1., 0., 1., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., 1., .0625, 0., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., 1., .0625, 0., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 1., 1., 0., 1., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., 1., 0., 1., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., 0., 0., 0., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 0., .0625, 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 1., 0., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 1., 0., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 0., .0625, 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 1., .0625, 1., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., .0625, 0., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 0., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 1., .0625, 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 1., .0625, 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 0., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 1., 0., 1., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 0., .0625, 0., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 0., .0625, 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 1., .0625, 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 1., .0625, 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 0., .0625, 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 1., .0625, 1., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
};

struct VertexDesc vertices_pane_ft[] = {
	{ 0., 0., .9375, 0., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., 0., .9375, 1., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ 0., 0., 1., 0., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ 0., 0., 1., 0., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., 0., .9375, 1., 1., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., 0., 1., 1., 0., B_COL, B_COL, B_COL, FACE_DN },
	{ 1., 1., 1., 0., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 1., 1., .9375, 1., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., 1., 1., 0., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., 1., 1., 0., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 1., 1., .9375, 1., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., 1., .9375, 1., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., 0., .9375, 0., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 0., 1., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 1., .9375, 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 1., .9375, 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 0., 1., 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 1., 1., 1., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 1., 0., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., .9375, 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 1., 1., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 1., 1., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., .9375, 1., 1., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 1., .9375, 1., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., .9375, 0., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., .9375, 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 1., .9375, 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 1., .9375, 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., .9375, 1., 1., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 1., .9375, 1., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
};

struct VertexDesc vertices_token_lf[] = {
	{ 1., .5, 1., 0., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 1., .5, 0., 1., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., .5, 1., 0., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., .5, 1., 0., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 1., .5, 0., 1., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., .5, 0., 1., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., 0., 0., 0., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 0., 1., 1., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 0., .5, 0., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., .5, 0., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 0., 1., 1., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 0., .5, 1., 1., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 1., 0., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 0., 1., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .5, 1., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .5, 1., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 0., 1., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .5, 0., 1., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 0., 0., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., 0., 1., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., .5, 0., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., .5, 0., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., 0., 1., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., .5, 0., 1., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., 1., 0., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 0., 1., 1., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., .5, 1., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., .5, 1., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 0., 1., 1., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., .5, 1., 1., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
};

struct VertexDesc vertices_token_bk[] = {
	{ 1., .5, 1., 1., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 1., .5, 0., 1., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., .5, 1., 0., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., .5, 1., 0., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 1., .5, 0., 1., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., .5, 0., 0., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., 0., 0., 0., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 0., 1., 1., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 0., .5, 0., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., .5, 0., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 0., 1., 1., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 0., .5, 1., 1., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 1., 0., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 0., 1., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .5, 1., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .5, 1., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 0., 1., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .5, 0., 1., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 0., 0., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., 0., 1., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., .5, 0., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., .5, 0., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., 0., 1., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., .5, 0., 1., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., 1., 0., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 0., 1., 1., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., .5, 1., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., .5, 1., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 0., 1., 1., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., .5, 1., 1., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
};

struct VertexDesc vertices_token_rt[] = {
	{ 1., .5, 1., 1., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 1., .5, 0., 0., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., .5, 1., 1., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., .5, 1., 1., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 1., .5, 0., 0., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., .5, 0., 0., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., 0., 0., 0., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 0., 1., 1., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 0., .5, 0., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., .5, 0., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 0., 1., 1., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 0., .5, 1., 1., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 1., 0., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 0., 1., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .5, 1., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .5, 1., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 0., 1., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .5, 0., 1., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 0., 0., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., 0., 1., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., .5, 0., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., .5, 0., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., 0., 1., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., .5, 0., 1., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., 1., 0., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 0., 1., 1., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., .5, 1., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., .5, 1., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 0., 1., 1., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., .5, 1., 1., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
};

struct VertexDesc vertices_token_ft[] = {
	{ 1., .5, 1., 0., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 1., .5, 0., 0., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., .5, 1., 1., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., .5, 1., 1., 0., T_COL, T_COL, T_COL, FACE_UP },
	{ 1., .5, 0., 0., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., .5, 0., 1., 1., T_COL, T_COL, T_COL, FACE_UP },
	{ 0., 0., 0., 0., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 0., 1., 1., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 0., .5, 0., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., .5, 0., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 0., 0., 1., 1., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 0., .5, 1., 1., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 1., 0., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 0., 1., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .5, 1., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .5, 1., 0., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 0., 1., .5, X_COL, X_COL, X_COL, FACE_LF },
	{ 1., .5, 0., 1., 0., X_COL, X_COL, X_COL, FACE_LF },
	{ 1., 0., 0., 0., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., 0., 1., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., .5, 0., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., .5, 0., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., 0., 1., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., .5, 0., 1., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., 0., 1., 0., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 0., 1., 1., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., .5, 1., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 0., .5, 1., 0., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., 0., 1., 1., .5, Z_COL, Z_COL, Z_COL, FACE_BK },
	{ 1., .5, 1., 1., 0., Z_COL, Z_COL, Z_COL, FACE_BK },
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

void VertexBuffer::draw_slice(GLenum mode, size_t buf, size_t first, size_t count)
{
	assert(buf < vbo_count);
	assert(first + count <= vbo_sizes[buf]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_names[buf]);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex), (char *)0);
	glTexCoordPointer(4, GL_FLOAT, sizeof(Vertex), (char *)offsetof(Vertex, u0));
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), (char *)offsetof(Vertex, r));
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


