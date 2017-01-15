
#include "vertex_buffer.h"

#include <assert.h>
#include <stdlib.h>

#include "types.h"

#define T_COL 1.
#define X_COL .8
#define Z_COL .6
#define B_COL .4

#define TC(a,b) { a / 32., b / 32. }

#define COMMON_TC(mat,y) \
	[mat##_CRATE] = { TC(16, y), TC(17, y), TC(0, y) },\
	[mat##_BENCH] = { TC(18, y), TC(19, y), TC(0, y) },\
	[mat##_PIPE] = { TC(20, y), TC(21, y), TC(21, y) }

#define STONE_TC(mat,y)\
	[mat] = { TC(0, y), TC(0, y), TC(0, y) },\
	[mat##_COBBLE] = { TC(1, y), TC(1, y), TC(1, y) },\
	[mat##_LBRICK] = { TC(6, y), TC(7, y), TC(8, y) },\
	[mat##_SBRICK] = { TC(3, y), TC(4, y), TC(5, y) },\
	[mat##_TILE] = { TC(2, y), TC(2, y), TC(2, y) },\
	[mat##_COLUMN] = { TC(9, y), TC(10, y), TC(10, y) },\
	[mat##_BLOCK] = { TC(7, y), TC(7, y), TC(7, y) },\
	[mat##_SAND] = { TC(15, y), TC(15, y), TC(16, y) },\
	COMMON_TC(mat, y)

#define WOOD_TC(mat,y)\
	[mat] = { TC(1, y), TC(1, y), TC(1, y) },\
	[mat##_LOG] = { TC(3, y), TC(2, y), TC(2, y) },\
	COMMON_TC(mat, y)

const struct v2f texcoord_from_mat[256][3] = {
	[MAT_DIRT] = { TC(1, 0), TC(1, 0), TC(1, 0) },
	[MAT_GRASS] = { TC(2, 0), TC(2, 0), TC(2, 0) },
	STONE_TC(MAT_LIMESTONE, 1),
	STONE_TC(MAT_SANDSTONE, 2),
	STONE_TC(MAT_MARBLE, 3),
	STONE_TC(MAT_GRANITE, 4),
	STONE_TC(MAT_BASALT, 5),
	STONE_TC(MAT_SLATE, 6),
	STONE_TC(MAT_CLAY, 7),
	WOOD_TC(MAT_WOOD1, 8),
	WOOD_TC(MAT_WOOD2, 9),
	WOOD_TC(MAT_WOOD3, 10),
	WOOD_TC(MAT_WOOD4, 11),
	WOOD_TC(MAT_WOOD5, 12),
	WOOD_TC(MAT_WOOD6, 13),
	WOOD_TC(MAT_WOOD7, 14),
};

struct vertex_desc vertices_face_dn[] = {
	{ 0., 0., 0., 0., 1., B_COL, B_COL, B_COL, 2 },
	{ 1., 0., 0., 1., 1., B_COL, B_COL, B_COL, 2 },
	{ 0., 0., 1., 0., 0., B_COL, B_COL, B_COL, 2 },
	{ 0., 0., 1., 0., 0., B_COL, B_COL, B_COL, 2 },
	{ 1., 0., 0., 1., 1., B_COL, B_COL, B_COL, 2 },
	{ 1., 0., 1., 1., 0., B_COL, B_COL, B_COL, 2 },
};

struct vertex_desc vertices_face_up[] = {
	{ 1., 0., 1., 0., 1., T_COL, T_COL, T_COL, 1 },
	{ 1., 0., 0., 1., 1., T_COL, T_COL, T_COL, 1 },
	{ 0., 0., 1., 0., 0., T_COL, T_COL, T_COL, 1 },
	{ 0., 0., 1., 0., 0., T_COL, T_COL, T_COL, 1 },
	{ 1., 0., 0., 1., 1., T_COL, T_COL, T_COL, 1 },
	{ 0., 0., 0., 1., 0., T_COL, T_COL, T_COL, 1 },
};

struct vertex_desc vertices_face_lf[] = {
	{ 0., 0., 0., 0., 1., X_COL, X_COL, X_COL },
	{ 0., 0., 1., 1., 1., X_COL, X_COL, X_COL },
	{ 0., 1., 0., 0., 0., X_COL, X_COL, X_COL },
	{ 0., 1., 0., 0., 0., X_COL, X_COL, X_COL },
	{ 0., 0., 1., 1., 1., X_COL, X_COL, X_COL },
	{ 0., 1., 1., 1., 0., X_COL, X_COL, X_COL },
};

struct vertex_desc vertices_face_rt[] = {
	{ 0., 0., 1., 0., 1., X_COL, X_COL, X_COL },
	{ 0., 0., 0., 1., 1., X_COL, X_COL, X_COL },
	{ 0., 1., 1., 0., 0., X_COL, X_COL, X_COL },
	{ 0., 1., 1., 0., 0., X_COL, X_COL, X_COL },
	{ 0., 0., 0., 1., 1., X_COL, X_COL, X_COL },
	{ 0., 1., 0., 1., 0., X_COL, X_COL, X_COL },
};

struct vertex_desc vertices_face_bk[] = {
	{ 1., 0., 0., 0., 1., Z_COL, Z_COL, Z_COL },
	{ 0., 0., 0., 1., 1., Z_COL, Z_COL, Z_COL },
	{ 1., 1., 0., 0., 0., Z_COL, Z_COL, Z_COL },
	{ 1., 1., 0., 0., 0., Z_COL, Z_COL, Z_COL },
	{ 0., 0., 0., 1., 1., Z_COL, Z_COL, Z_COL },
	{ 0., 1., 0., 1., 0., Z_COL, Z_COL, Z_COL },
};

struct vertex_desc vertices_face_ft[] = {
	{ 0., 0., 0., 0., 1., Z_COL, Z_COL, Z_COL },
	{ 1., 0., 0., 1., 1., Z_COL, Z_COL, Z_COL },
	{ 0., 1., 0., 0., 0., Z_COL, Z_COL, Z_COL },
	{ 0., 1., 0., 0., 0., Z_COL, Z_COL, Z_COL },
	{ 1., 0., 0., 1., 1., Z_COL, Z_COL, Z_COL },
	{ 1., 1., 0., 1., 0., Z_COL, Z_COL, Z_COL },
};

struct vertex_desc vertices_slab_dn[] = {
	{ 1., .5, 1., 0., 1., T_COL, T_COL, T_COL, 1 },
	{ 1., .5, 0., 1., 1., T_COL, T_COL, T_COL, 1 },
	{ 0., .5, 1., 0., 0., T_COL, T_COL, T_COL, 1 },
	{ 0., .5, 1., 0., 0., T_COL, T_COL, T_COL, 1 },
	{ 1., .5, 0., 1., 1., T_COL, T_COL, T_COL, 1 },
	{ 0., .5, 0., 1., 0., T_COL, T_COL, T_COL, 1 },
	{ 0., 0., 0., 0., .5, X_COL, X_COL, X_COL },
	{ 0., 0., 1., 1., .5, X_COL, X_COL, X_COL },
	{ 0., .5, 0., 0., 0., X_COL, X_COL, X_COL },
	{ 0., .5, 0., 0., 0., X_COL, X_COL, X_COL },
	{ 0., 0., 1., 1., .5, X_COL, X_COL, X_COL },
	{ 0., .5, 1., 1., 0., X_COL, X_COL, X_COL },
	{ 1., 0., 1., 0., .5, X_COL, X_COL, X_COL },
	{ 1., 0., 0., 1., .5, X_COL, X_COL, X_COL },
	{ 1., .5, 1., 0., 0., X_COL, X_COL, X_COL },
	{ 1., .5, 1., 0., 0., X_COL, X_COL, X_COL },
	{ 1., 0., 0., 1., .5, X_COL, X_COL, X_COL },
	{ 1., .5, 0., 1., 0., X_COL, X_COL, X_COL },
	{ 1., 0., 0., 0., .5, Z_COL, Z_COL, Z_COL },
	{ 0., 0., 0., 1., .5, Z_COL, Z_COL, Z_COL },
	{ 1., .5, 0., 0., 0., Z_COL, Z_COL, Z_COL },
	{ 1., .5, 0., 0., 0., Z_COL, Z_COL, Z_COL },
	{ 0., 0., 0., 1., .5, Z_COL, Z_COL, Z_COL },
	{ 0., .5, 0., 1., 0., Z_COL, Z_COL, Z_COL },
	{ 0., 0., 1., 0., .5, Z_COL, Z_COL, Z_COL },
	{ 1., 0., 1., 1., .5, Z_COL, Z_COL, Z_COL },
	{ 0., .5, 1., 0., 0., Z_COL, Z_COL, Z_COL },
	{ 0., .5, 1., 0., 0., Z_COL, Z_COL, Z_COL },
	{ 1., 0., 1., 1., .5, Z_COL, Z_COL, Z_COL },
	{ 1., .5, 1., 1., 0., Z_COL, Z_COL, Z_COL },
};

struct vertex_desc vertices_slab_up[] = {
	{ 0., .5, 0., 0., 1., B_COL, B_COL, B_COL, 2 },
	{ 1., .5, 0., 1., 1., B_COL, B_COL, B_COL, 2 },
	{ 0., .5, 1., 0., 0., B_COL, B_COL, B_COL, 2 },
	{ 0., .5, 1., 0., 0., B_COL, B_COL, B_COL, 2 },
	{ 1., .5, 0., 1., 1., B_COL, B_COL, B_COL, 2 },
	{ 1., .5, 1., 1., 0., B_COL, B_COL, B_COL, 2 },
	{ 0., .5, 0., 0., 1., X_COL, X_COL, X_COL },
	{ 0., .5, 1., 1., 1., X_COL, X_COL, X_COL },
	{ 0., 1., 0., 0., .5, X_COL, X_COL, X_COL },
	{ 0., 1., 0., 0., .5, X_COL, X_COL, X_COL },
	{ 0., .5, 1., 1., 1., X_COL, X_COL, X_COL },
	{ 0., 1., 1., 1., .5, X_COL, X_COL, X_COL },
	{ 1., .5, 1., 0., 1., X_COL, X_COL, X_COL },
	{ 1., .5, 0., 1., 1., X_COL, X_COL, X_COL },
	{ 1., 1., 1., 0., .5, X_COL, X_COL, X_COL },
	{ 1., 1., 1., 0., .5, X_COL, X_COL, X_COL },
	{ 1., .5, 0., 1., 1., X_COL, X_COL, X_COL },
	{ 1., 1., 0., 1., .5, X_COL, X_COL, X_COL },
	{ 1., .5, 0., 0., 1., Z_COL, Z_COL, Z_COL },
	{ 0., .5, 0., 1., 1., Z_COL, Z_COL, Z_COL },
	{ 1., 1., 0., 0., .5, Z_COL, Z_COL, Z_COL },
	{ 1., 1., 0., 0., .5, Z_COL, Z_COL, Z_COL },
	{ 0., .5, 0., 1., 1., Z_COL, Z_COL, Z_COL },
	{ 0., 1., 0., 1., .5, Z_COL, Z_COL, Z_COL },
	{ 0., .5, 1., 0., 1., Z_COL, Z_COL, Z_COL },
	{ 1., .5, 1., 1., 1., Z_COL, Z_COL, Z_COL },
	{ 0., 1., 1., 0., .5, Z_COL, Z_COL, Z_COL },
	{ 0., 1., 1., 0., .5, Z_COL, Z_COL, Z_COL },
	{ 1., .5, 1., 1., 1., Z_COL, Z_COL, Z_COL },
	{ 1., 1., 1., 1., .5, Z_COL, Z_COL, Z_COL },
};

struct vertex_desc vertices_slab_lf[] = {
	{ 0., 0., 0., 0., 1., B_COL, B_COL, B_COL, 2 },
	{ .5, 0., 0., 1., 1., B_COL, B_COL, B_COL, 2 },
	{ 0., 0., 1., 0., 0., B_COL, B_COL, B_COL, 2 },
	{ 0., 0., 1., 0., 0., B_COL, B_COL, B_COL, 2 },
	{ .5, 0., 0., 1., 1., B_COL, B_COL, B_COL, 2 },
	{ .5, 0., 1., 1., 0., B_COL, B_COL, B_COL, 2 },
	{ .5, 1., 1., 0., 1., T_COL, T_COL, T_COL, 1 },
	{ .5, 1., 0., 1., 1., T_COL, T_COL, T_COL, 1 },
	{ 0., 1., 1., 0., 0., T_COL, T_COL, T_COL, 1 },
	{ 0., 1., 1., 0., 0., T_COL, T_COL, T_COL, 1 },
	{ .5, 1., 0., 1., 1., T_COL, T_COL, T_COL, 1 },
	{ 0., 1., 0., 1., 0., T_COL, T_COL, T_COL, 1 },
	{ .5, 0., 1., 0., 1., X_COL, X_COL, X_COL },
	{ .5, 0., 0., 1., 1., X_COL, X_COL, X_COL },
	{ .5, 1., 1., 0., 0., X_COL, X_COL, X_COL },
	{ .5, 1., 1., 0., 0., X_COL, X_COL, X_COL },
	{ .5, 0., 0., 1., 1., X_COL, X_COL, X_COL },
	{ .5, 1., 0., 1., 0., X_COL, X_COL, X_COL },
	{ .5, 0., 0., 0., 1., Z_COL, Z_COL, Z_COL },
	{ 0., 0., 0., 1., 1., Z_COL, Z_COL, Z_COL },
	{ .5, 1., 0., 0., 0., Z_COL, Z_COL, Z_COL },
	{ .5, 1., 0., 0., 0., Z_COL, Z_COL, Z_COL },
	{ 0., 0., 0., 1., 1., Z_COL, Z_COL, Z_COL },
	{ 0., 1., 0., 1., 0., Z_COL, Z_COL, Z_COL },
	{ 0., 0., 1., 0., 1., Z_COL, Z_COL, Z_COL },
	{ .5, 0., 1., 1., 1., Z_COL, Z_COL, Z_COL },
	{ 0., 1., 1., 0., 0., Z_COL, Z_COL, Z_COL },
	{ 0., 1., 1., 0., 0., Z_COL, Z_COL, Z_COL },
	{ .5, 0., 1., 1., 1., Z_COL, Z_COL, Z_COL },
	{ .5, 1., 1., 1., 0., Z_COL, Z_COL, Z_COL },
};

struct vertex_desc vertices_slab_rt[] = {
	{ .5, 0., 0., 0., 1., B_COL, B_COL, B_COL, 2 },
	{ 1., 0., 0., 1., 1., B_COL, B_COL, B_COL, 2 },
	{ .5, 0., 1., 0., 0., B_COL, B_COL, B_COL, 2 },
	{ .5, 0., 1., 0., 0., B_COL, B_COL, B_COL, 2 },
	{ 1., 0., 0., 1., 1., B_COL, B_COL, B_COL, 2 },
	{ 1., 0., 1., 1., 0., B_COL, B_COL, B_COL, 2 },
	{ 1., 1., 1., 0., 1., T_COL, T_COL, T_COL, 1 },
	{ 1., 1., 0., 1., 1., T_COL, T_COL, T_COL, 1 },
	{ .5, 1., 1., 0., 0., T_COL, T_COL, T_COL, 1 },
	{ .5, 1., 1., 0., 0., T_COL, T_COL, T_COL, 1 },
	{ 1., 1., 0., 1., 1., T_COL, T_COL, T_COL, 1 },
	{ .5, 1., 0., 1., 0., T_COL, T_COL, T_COL, 1 },
	{ .5, 0., 0., 0., 1., X_COL, X_COL, X_COL },
	{ .5, 0., 1., 1., 1., X_COL, X_COL, X_COL },
	{ .5, 1., 0., 0., 0., X_COL, X_COL, X_COL },
	{ .5, 1., 0., 0., 0., X_COL, X_COL, X_COL },
	{ .5, 0., 1., 1., 1., X_COL, X_COL, X_COL },
	{ .5, 1., 1., 1., 0., X_COL, X_COL, X_COL },
	{ 1., 0., 0., 0., 1., Z_COL, Z_COL, Z_COL },
	{ .5, 0., 0., 1., 1., Z_COL, Z_COL, Z_COL },
	{ 1., 1., 0., 0., 0., Z_COL, Z_COL, Z_COL },
	{ 1., 1., 0., 0., 0., Z_COL, Z_COL, Z_COL },
	{ .5, 0., 0., 1., 1., Z_COL, Z_COL, Z_COL },
	{ .5, 1., 0., 1., 0., Z_COL, Z_COL, Z_COL },
	{ .5, 0., 1., 0., 1., Z_COL, Z_COL, Z_COL },
	{ 1., 0., 1., 1., 1., Z_COL, Z_COL, Z_COL },
	{ .5, 1., 1., 0., 0., Z_COL, Z_COL, Z_COL },
	{ .5, 1., 1., 0., 0., Z_COL, Z_COL, Z_COL },
	{ 1., 0., 1., 1., 1., Z_COL, Z_COL, Z_COL },
	{ 1., 1., 1., 1., 0., Z_COL, Z_COL, Z_COL },
};

struct vertex_desc vertices_slab_bk[] = {
	{ 0., 0., 0., 0., 1., B_COL, B_COL, B_COL, 2 },
	{ 1., 0., 0., 1., 1., B_COL, B_COL, B_COL, 2 },
	{ 0., 0., .5, 0., 0., B_COL, B_COL, B_COL, 2 },
	{ 0., 0., .5, 0., 0., B_COL, B_COL, B_COL, 2 },
	{ 1., 0., 0., 1., 1., B_COL, B_COL, B_COL, 2 },
	{ 1., 0., .5, 1., 0., B_COL, B_COL, B_COL, 2 },
	{ 1., 1., .5, 0., 1., T_COL, T_COL, T_COL, 1 },
	{ 1., 1., 0., 1., 1., T_COL, T_COL, T_COL, 1 },
	{ 0., 1., .5, 0., 0., T_COL, T_COL, T_COL, 1 },
	{ 0., 1., .5, 0., 0., T_COL, T_COL, T_COL, 1 },
	{ 1., 1., 0., 1., 1., T_COL, T_COL, T_COL, 1 },
	{ 0., 1., 0., 1., 0., T_COL, T_COL, T_COL, 1 },
	{ 0., 0., 0., 0., 1., X_COL, X_COL, X_COL },
	{ 0., 0., .5, 1., 1., X_COL, X_COL, X_COL },
	{ 0., 1., 0., 0., 0., X_COL, X_COL, X_COL },
	{ 0., 1., 0., 0., 0., X_COL, X_COL, X_COL },
	{ 0., 0., .5, 1., 1., X_COL, X_COL, X_COL },
	{ 0., 1., .5, 1., 0., X_COL, X_COL, X_COL },
	{ 1., 0., .5, 0., 1., X_COL, X_COL, X_COL },
	{ 1., 0., 0., 1., 1., X_COL, X_COL, X_COL },
	{ 1., 1., .5, 0., 0., X_COL, X_COL, X_COL },
	{ 1., 1., .5, 0., 0., X_COL, X_COL, X_COL },
	{ 1., 0., 0., 1., 1., X_COL, X_COL, X_COL },
	{ 1., 1., 0., 1., 0., X_COL, X_COL, X_COL },
	{ 0., 0., .5, 0., 1., Z_COL, Z_COL, Z_COL },
	{ 1., 0., .5, 1., 1., Z_COL, Z_COL, Z_COL },
	{ 0., 1., .5, 0., 0., Z_COL, Z_COL, Z_COL },
	{ 0., 1., .5, 0., 0., Z_COL, Z_COL, Z_COL },
	{ 1., 0., .5, 1., 1., Z_COL, Z_COL, Z_COL },
	{ 1., 1., .5, 1., 0., Z_COL, Z_COL, Z_COL },
};

struct vertex_desc vertices_slab_ft[] = {
	{ 0., 0., .5, 0., 1., B_COL, B_COL, B_COL, 2 },
	{ 1., 0., .5, 1., 1., B_COL, B_COL, B_COL, 2 },
	{ 0., 0., 1., 0., 0., B_COL, B_COL, B_COL, 2 },
	{ 0., 0., 1., 0., 0., B_COL, B_COL, B_COL, 2 },
	{ 1., 0., .5, 1., 1., B_COL, B_COL, B_COL, 2 },
	{ 1., 0., 1., 1., 0., B_COL, B_COL, B_COL, 2 },
	{ 1., 1., 1., 0., 1., T_COL, T_COL, T_COL, 1 },
	{ 1., 1., .5, 1., 1., T_COL, T_COL, T_COL, 1 },
	{ 0., 1., 1., 0., 0., T_COL, T_COL, T_COL, 1 },
	{ 0., 1., 1., 0., 0., T_COL, T_COL, T_COL, 1 },
	{ 1., 1., .5, 1., 1., T_COL, T_COL, T_COL, 1 },
	{ 0., 1., .5, 1., 0., T_COL, T_COL, T_COL, 1 },
	{ 0., 0., .5, 0., 1., X_COL, X_COL, X_COL },
	{ 0., 0., 1., 1., 1., X_COL, X_COL, X_COL },
	{ 0., 1., .5, 0., 0., X_COL, X_COL, X_COL },
	{ 0., 1., .5, 0., 0., X_COL, X_COL, X_COL },
	{ 0., 0., 1., 1., 1., X_COL, X_COL, X_COL },
	{ 0., 1., 1., 1., 0., X_COL, X_COL, X_COL },
	{ 1., 0., 1., 0., 1., X_COL, X_COL, X_COL },
	{ 1., 0., .5, 1., 1., X_COL, X_COL, X_COL },
	{ 1., 1., 1., 0., 0., X_COL, X_COL, X_COL },
	{ 1., 1., 1., 0., 0., X_COL, X_COL, X_COL },
	{ 1., 0., .5, 1., 1., X_COL, X_COL, X_COL },
	{ 1., 1., .5, 1., 0., X_COL, X_COL, X_COL },
	{ 1., 0., .5, 0., 1., Z_COL, Z_COL, Z_COL },
	{ 0., 0., .5, 1., 1., Z_COL, Z_COL, Z_COL },
	{ 1., 1., .5, 0., 0., Z_COL, Z_COL, Z_COL },
	{ 1., 1., .5, 0., 0., Z_COL, Z_COL, Z_COL },
	{ 0., 0., .5, 1., 1., Z_COL, Z_COL, Z_COL },
	{ 0., 1., .5, 1., 0., Z_COL, Z_COL, Z_COL },
};

struct vertex_desc vertices_stairs_dl[] = {
	{ 1., .5, 1., 0., 1., T_COL, T_COL, T_COL, 1 },
	{ 1., .5, 0., 1., 1., T_COL, T_COL, T_COL, 1 },
	{ .5, .5, 1., 0., .5, T_COL, T_COL, T_COL, 1 },
	{ .5, .5, 1., 0., .5, T_COL, T_COL, T_COL, 1 },
	{ 1., .5, 0., 1., 1., T_COL, T_COL, T_COL, 1 },
	{ .5, .5, 0., 1., .5, T_COL, T_COL, T_COL, 1 },
	{ .5, 1., 1., 0., .5, T_COL, T_COL, T_COL, 1 },
	{ .5, 1., 0., 1., .5, T_COL, T_COL, T_COL, 1 },
	{ 0., 1., 1., 0., 0., T_COL, T_COL, T_COL, 1 },
	{ 0., 1., 1., 0., 0., T_COL, T_COL, T_COL, 1 },
	{ .5, 1., 0., 1., .5, T_COL, T_COL, T_COL, 1 },
	{ 0., 1., 0., 1., 0., T_COL, T_COL, T_COL, 1 },
	{ 1., 0., 1., 0., .5, X_COL, X_COL, X_COL },
	{ 1., 0., 0., 1., .5, X_COL, X_COL, X_COL },
	{ 1., .5, 1., 0., 0., X_COL, X_COL, X_COL },
	{ 1., .5, 1., 0., 0., X_COL, X_COL, X_COL },
	{ 1., 0., 0., 1., .5, X_COL, X_COL, X_COL },
	{ 1., .5, 0., 1., 0., X_COL, X_COL, X_COL },
	{ .5, .5, 1., 0., 1., X_COL, X_COL, X_COL },
	{ .5, .5, 0., 1., 1., X_COL, X_COL, X_COL },
	{ .5, 1., 1., 0., .5, X_COL, X_COL, X_COL },
	{ .5, 1., 1., 0., .5, X_COL, X_COL, X_COL },
	{ .5, .5, 0., 1., 1., X_COL, X_COL, X_COL },
	{ .5, 1., 0., 1., .5, X_COL, X_COL, X_COL },
	{ 1., 0., 0., 1., 1., Z_COL, Z_COL, Z_COL },
	{ 0., 0., 0., 0., 1., Z_COL, Z_COL, Z_COL },
	{ 0., 1., 0., 0., 0., Z_COL, Z_COL, Z_COL },
	{ 1., 0., 0., 1., 1., Z_COL, Z_COL, Z_COL },
	{ .5, .5, 0., .5, .5, Z_COL, Z_COL, Z_COL },
	{ 1., .5, 0., 1., .5, Z_COL, Z_COL, Z_COL },
	{ .5, .5, 0., .5, .5, Z_COL, Z_COL, Z_COL },
	{ 0., 1., 0., 0., 0., Z_COL, Z_COL, Z_COL },
	{ .5, 1., 0., .5, 0., Z_COL, Z_COL, Z_COL },
	{ 0., 0., 1., 0., 1., Z_COL, Z_COL, Z_COL },
	{ 1., 0., 1., 1., 1., Z_COL, Z_COL, Z_COL },
	{ 0., 1., 1., 0., 0., Z_COL, Z_COL, Z_COL },
	{ .5, .5, 1., .5, .5, Z_COL, Z_COL, Z_COL },
	{ 1., 0., 1., 1., 1., Z_COL, Z_COL, Z_COL },
	{ 1., .5, 1., 1., .5, Z_COL, Z_COL, Z_COL },
	{ 0., 1., 1., 0., 0., Z_COL, Z_COL, Z_COL },
	{ .5, .5, 1., .5, .5, Z_COL, Z_COL, Z_COL },
	{ .5, 1., 1., .5, 0., Z_COL, Z_COL, Z_COL },
};

struct vertex_desc vertices_stairs_dr[] = {
	{ 0., .5, 0., 1., 1., T_COL, T_COL, T_COL, 1 },
	{ 0., .5, 1., 0., 1., T_COL, T_COL, T_COL, 1 },
	{ .5, .5, 1., 0., .5, T_COL, T_COL, T_COL, 1 },
	{ 0., .5, 0., 1., 1., T_COL, T_COL, T_COL, 1 },
	{ .5, .5, 1., 0., .5, T_COL, T_COL, T_COL, 1 },
	{ .5, .5, 0., 1., .5, T_COL, T_COL, T_COL, 1 },
	{ .5, 1., 0., 1., .5, T_COL, T_COL, T_COL, 1 },
	{ .5, 1., 1., 0., .5, T_COL, T_COL, T_COL, 1 },
	{ 1., 1., 1., 0., 0., T_COL, T_COL, T_COL, 1 },
	{ .5, 1., 0., 1., .5, T_COL, T_COL, T_COL, 1 },
	{ 1., 1., 1., 0., 0., T_COL, T_COL, T_COL, 1 },
	{ 1., 1., 0., 1., 0., T_COL, T_COL, T_COL, 1 },
	{ 0., 0., 0., 1., .5, X_COL, X_COL, X_COL },
	{ 0., 0., 1., 0., .5, X_COL, X_COL, X_COL },
	{ 0., .5, 1., 0., 0., X_COL, X_COL, X_COL },
	{ 0., 0., 0., 1., .5, X_COL, X_COL, X_COL },
	{ 0., .5, 1., 0., 0., X_COL, X_COL, X_COL },
	{ 0., .5, 0., 1., 0., X_COL, X_COL, X_COL },
	{ .5, .5, 0., 1., 1., X_COL, X_COL, X_COL },
	{ .5, .5, 1., 0., 1., X_COL, X_COL, X_COL },
	{ .5, 1., 1., 0., .5, X_COL, X_COL, X_COL },
	{ .5, .5, 0., 1., 1., X_COL, X_COL, X_COL },
	{ .5, 1., 1., 0., .5, X_COL, X_COL, X_COL },
	{ .5, 1., 0., 1., .5, X_COL, X_COL, X_COL },
	{ 1., 0., 0., 0., 1., Z_COL, Z_COL, Z_COL },
	{ 0., 0., 0., 1., 1., Z_COL, Z_COL, Z_COL },
	{ 1., 1., 0., 0., 0., Z_COL, Z_COL, Z_COL },
	{ .5, .5, 0., .5, .5, Z_COL, Z_COL, Z_COL },
	{ 0., 0., 0., 1., 1., Z_COL, Z_COL, Z_COL },
	{ 0., .5, 0., 1., .5, Z_COL, Z_COL, Z_COL },
	{ 1., 1., 0., 0., 0., Z_COL, Z_COL, Z_COL },
	{ .5, .5, 0., .5, .5, Z_COL, Z_COL, Z_COL },
	{ .5, 1., 0., .5, 0., Z_COL, Z_COL, Z_COL },
	{ 0., 0., 1., 1., 1., Z_COL, Z_COL, Z_COL },
	{ 1., 0., 1., 0., 1., Z_COL, Z_COL, Z_COL },
	{ 1., 1., 1., 0., 0., Z_COL, Z_COL, Z_COL },
	{ 0., 0., 1., 1., 1., Z_COL, Z_COL, Z_COL },
	{ .5, .5, 1., .5, .5, Z_COL, Z_COL, Z_COL },
	{ 0., .5, 1., 1., .5, Z_COL, Z_COL, Z_COL },
	{ .5, .5, 1., .5, .5, Z_COL, Z_COL, Z_COL },
	{ 1., 1., 1., 0., 0., Z_COL, Z_COL, Z_COL },
	{ .5, 1., 1., .5, 0., Z_COL, Z_COL, Z_COL },
};

struct vertex_desc vertices_stairs_db[] = {
	{ 0., .5, 1., 1., 1., T_COL, T_COL, T_COL, 1 },
	{ 1., .5, 1., 0., 1., T_COL, T_COL, T_COL, 1 },
	{ 1., .5, .5, 0., .5, T_COL, T_COL, T_COL, 1 },
	{ 0., .5, 1., 1., 1., T_COL, T_COL, T_COL, 1 },
	{ 1., .5, .5, 0., .5, T_COL, T_COL, T_COL, 1 },
	{ 0., .5, .5, 1., .5, T_COL, T_COL, T_COL, 1 },
	{ 0., 1., .5, 1., .5, T_COL, T_COL, T_COL, 1 },
	{ 1., 1., .5, 0., .5, T_COL, T_COL, T_COL, 1 },
	{ 1., 1., 0., 0., 0., T_COL, T_COL, T_COL, 1 },
	{ 0., 1., .5, 1., .5, T_COL, T_COL, T_COL, 1 },
	{ 1., 1., 0., 0., 0., T_COL, T_COL, T_COL, 1 },
	{ 0., 1., 0., 1., 0., T_COL, T_COL, T_COL, 1 },
	{ 0., 0., 1., 1., .5, Z_COL, Z_COL, Z_COL },
	{ 1., 0., 1., 0., .5, Z_COL, Z_COL, Z_COL },
	{ 1., .5, 1., 0., 0., Z_COL, Z_COL, Z_COL },
	{ 0., 0., 1., 1., .5, Z_COL, Z_COL, Z_COL },
	{ 1., .5, 1., 0., 0., Z_COL, Z_COL, Z_COL },
	{ 0., .5, 1., 1., 0., Z_COL, Z_COL, Z_COL },
	{ 0., .5, .5, 1., 1., Z_COL, Z_COL, Z_COL },
	{ 1., .5, .5, 0., 1., Z_COL, Z_COL, Z_COL },
	{ 1., 1., .5, 0., .5, Z_COL, Z_COL, Z_COL },
	{ 0., .5, .5, 1., 1., Z_COL, Z_COL, Z_COL },
	{ 1., 1., .5, 0., .5, Z_COL, Z_COL, Z_COL },
	{ 0., 1., .5, 1., .5, Z_COL, Z_COL, Z_COL },
	{ 0., 0., 0., 0., 1., X_COL, X_COL, X_COL },
	{ 0., 0., 1., 1., 1., X_COL, X_COL, X_COL },
	{ 0., 1., 0., 0., 0., X_COL, X_COL, X_COL },
	{ 0., .5, .5, .5, .5, X_COL, X_COL, X_COL },
	{ 0., 0., 1., 1., 1., X_COL, X_COL, X_COL },
	{ 0., .5, 1., 1., .5, X_COL, X_COL, X_COL },
	{ 0., 1., 0., 0., 0., X_COL, X_COL, X_COL },
	{ 0., .5, .5, .5, .5, X_COL, X_COL, X_COL },
	{ 0., 1., .5, .5, 0., X_COL, X_COL, X_COL },
	{ 1., 0., 1., 1., 1., X_COL, X_COL, X_COL },
	{ 1., 0., 0., 0., 1., X_COL, X_COL, X_COL },
	{ 1., 1., 0., 0., 0., X_COL, X_COL, X_COL },
	{ 1., 0., 1., 1., 1., X_COL, X_COL, X_COL },
	{ 1., .5, .5, .5, .5, X_COL, X_COL, X_COL },
	{ 1., .5, 1., 1., .5, X_COL, X_COL, X_COL },
	{ 1., .5, .5, .5, .5, X_COL, X_COL, X_COL },
	{ 1., 1., 0., 0., 0., X_COL, X_COL, X_COL },
	{ 1., 1., .5, .5, 0., X_COL, X_COL, X_COL },
};

struct vertex_desc vertices_stairs_df[] = {
	{ 1., .5, 0., 0., 1., T_COL, T_COL, T_COL, 1 },
	{ 0., .5, 0., 1., 1., T_COL, T_COL, T_COL, 1 },
	{ 1., .5, .5, 0., .5, T_COL, T_COL, T_COL, 1 },
	{ 1., .5, .5, 0., .5, T_COL, T_COL, T_COL, 1 },
	{ 0., .5, 0., 1., 1., T_COL, T_COL, T_COL, 1 },
	{ 0., .5, .5, 1., .5, T_COL, T_COL, T_COL, 1 },
	{ 1., 1., .5, 0., .5, T_COL, T_COL, T_COL, 1 },
	{ 0., 1., .5, 1., .5, T_COL, T_COL, T_COL, 1 },
	{ 1., 1., 1., 0., 0., T_COL, T_COL, T_COL, 1 },
	{ 1., 1., 1., 0., 0., T_COL, T_COL, T_COL, 1 },
	{ 0., 1., .5, 1., .5, T_COL, T_COL, T_COL, 1 },
	{ 0., 1., 1., 1., 0., T_COL, T_COL, T_COL, 1 },
	{ 1., 0., 0., 0., .5, Z_COL, Z_COL, Z_COL },
	{ 0., 0., 0., 1., .5, Z_COL, Z_COL, Z_COL },
	{ 1., .5, 0., 0., 0., Z_COL, Z_COL, Z_COL },
	{ 1., .5, 0., 0., 0., Z_COL, Z_COL, Z_COL },
	{ 0., 0., 0., 1., .5, Z_COL, Z_COL, Z_COL },
	{ 0., .5, 0., 1., 0., Z_COL, Z_COL, Z_COL },
	{ 1., .5, .5, 0., 1., Z_COL, Z_COL, Z_COL },
	{ 0., .5, .5, 1., 1., Z_COL, Z_COL, Z_COL },
	{ 1., 1., .5, 0., .5, Z_COL, Z_COL, Z_COL },
	{ 1., 1., .5, 0., .5, Z_COL, Z_COL, Z_COL },
	{ 0., .5, .5, 1., 1., Z_COL, Z_COL, Z_COL },
	{ 0., 1., .5, 1., .5, Z_COL, Z_COL, Z_COL },
	{ 0., 0., 0., 1., 1., X_COL, X_COL, X_COL },
	{ 0., 0., 1., 0., 1., X_COL, X_COL, X_COL },
	{ 0., 1., 1., 0., 0., X_COL, X_COL, X_COL },
	{ 0., 0., 0., 1., 1., X_COL, X_COL, X_COL },
	{ 0., .5, .5, .5, .5, X_COL, X_COL, X_COL },
	{ 0., .5, 0., 1., .5, X_COL, X_COL, X_COL },
	{ 0., .5, .5, .5, .5, X_COL, X_COL, X_COL },
	{ 0., 1., 1., 0., 0., X_COL, X_COL, X_COL },
	{ 0., 1., .5, .5, 0., X_COL, X_COL, X_COL },
	{ 1., 0., 1., 0., 1., X_COL, X_COL, X_COL },
	{ 1., 0., 0., 1., 1., X_COL, X_COL, X_COL },
	{ 1., 1., 1., 0., 0., X_COL, X_COL, X_COL },
	{ 1., .5, .5, .5, .5, X_COL, X_COL, X_COL },
	{ 1., 0., 0., 1., 1., X_COL, X_COL, X_COL },
	{ 1., .5, 0., 1., .5, X_COL, X_COL, X_COL },
	{ 1., 1., 1., 0., 0., X_COL, X_COL, X_COL },
	{ 1., .5, .5, .5, .5, X_COL, X_COL, X_COL },
	{ 1., 1., .5, .5, 0., X_COL, X_COL, X_COL },
};

struct vertex_desc vertices_stairs_ul[] = {
	{ 1., .5, 0., 1., 1., B_COL, B_COL, B_COL, 2 },
	{ 1., .5, 1., 0., 1., B_COL, B_COL, B_COL, 2 },
	{ .5, .5, 1., 0., .5, B_COL, B_COL, B_COL, 2 },
	{ 1., .5, 0., 1., 1., B_COL, B_COL, B_COL, 2 },
	{ .5, .5, 1., 0., .5, B_COL, B_COL, B_COL, 2 },
	{ .5, .5, 0., 1., .5, B_COL, B_COL, B_COL, 2 },
	{ .5, 0., 0., 1., .5, B_COL, B_COL, B_COL, 2 },
	{ .5, 0., 1., 0., .5, B_COL, B_COL, B_COL, 2 },
	{ 0., 0., 1., 0., 0., B_COL, B_COL, B_COL, 2 },
	{ .5, 0., 0., 1., .5, B_COL, B_COL, B_COL, 2 },
	{ 0., 0., 1., 0., 0., B_COL, B_COL, B_COL, 2 },
	{ 0., 0., 0., 1., 0., B_COL, B_COL, B_COL, 2 },
	{ 1., 1., 0., 1., .5, X_COL, X_COL, X_COL },
	{ 1., 1., 1., 0., .5, X_COL, X_COL, X_COL },
	{ 1., .5, 1., 0., 0., X_COL, X_COL, X_COL },
	{ 1., 1., 0., 1., .5, X_COL, X_COL, X_COL },
	{ 1., .5, 1., 0., 0., X_COL, X_COL, X_COL },
	{ 1., .5, 0., 1., 0., X_COL, X_COL, X_COL },
	{ .5, .5, 0., 1., 1., X_COL, X_COL, X_COL },
	{ .5, .5, 1., 0., 1., X_COL, X_COL, X_COL },
	{ .5, 0., 1., 0., .5, X_COL, X_COL, X_COL },
	{ .5, .5, 0., 1., 1., X_COL, X_COL, X_COL },
	{ .5, 0., 1., 0., .5, X_COL, X_COL, X_COL },
	{ .5, 0., 0., 1., .5, X_COL, X_COL, X_COL },
	{ 0., 1., 0., 0., 1., Z_COL, Z_COL, Z_COL },
	{ 1., 1., 0., 1., 1., Z_COL, Z_COL, Z_COL },
	{ 0., 0., 0., 0., 0., Z_COL, Z_COL, Z_COL },
	{ .5, .5, 0., .5, .5, Z_COL, Z_COL, Z_COL },
	{ 1., 1., 0., 1., 1., Z_COL, Z_COL, Z_COL },
	{ 1., .5, 0., 1., .5, Z_COL, Z_COL, Z_COL },
	{ 0., 0., 0., 0., 0., Z_COL, Z_COL, Z_COL },
	{ .5, .5, 0., .5, .5, Z_COL, Z_COL, Z_COL },
	{ .5, 0., 0., .5, 0., Z_COL, Z_COL, Z_COL },
	{ 1., 1., 1., 1., 1., Z_COL, Z_COL, Z_COL },
	{ 0., 1., 1., 0., 1., Z_COL, Z_COL, Z_COL },
	{ 0., 0., 1., 0., 0., Z_COL, Z_COL, Z_COL },
	{ 1., 1., 1., 1., 1., Z_COL, Z_COL, Z_COL },
	{ .5, .5, 1., .5, .5, Z_COL, Z_COL, Z_COL },
	{ 1., .5, 1., 1., .5, Z_COL, Z_COL, Z_COL },
	{ .5, .5, 1., .5, .5, Z_COL, Z_COL, Z_COL },
	{ 0., 0., 1., 0., 0., Z_COL, Z_COL, Z_COL },
	{ .5, 0., 1., .5, 0., Z_COL, Z_COL, Z_COL },
};

struct vertex_desc vertices_stairs_ur[] = {
	{ 0., .5, 1., 0., 1., B_COL, B_COL, B_COL, 2 },
	{ 0., .5, 0., 1., 1., B_COL, B_COL, B_COL, 2 },
	{ .5, .5, 1., 0., .5, B_COL, B_COL, B_COL, 2 },
	{ .5, .5, 1., 0., .5, B_COL, B_COL, B_COL, 2 },
	{ 0., .5, 0., 1., 1., B_COL, B_COL, B_COL, 2 },
	{ .5, .5, 0., 1., .5, B_COL, B_COL, B_COL, 2 },
	{ .5, 0., 1., 0., .5, B_COL, B_COL, B_COL, 2 },
	{ .5, 0., 0., 1., .5, B_COL, B_COL, B_COL, 2 },
	{ 1., 0., 1., 0., 0., B_COL, B_COL, B_COL, 2 },
	{ 1., 0., 1., 0., 0., B_COL, B_COL, B_COL, 2 },
	{ .5, 0., 0., 1., .5, B_COL, B_COL, B_COL, 2 },
	{ 1., 0., 0., 1., 0., B_COL, B_COL, B_COL, 2 },
	{ 0., 1., 1., 0., .5, X_COL, X_COL, X_COL },
	{ 0., 1., 0., 1., .5, X_COL, X_COL, X_COL },
	{ 0., .5, 1., 0., 0., X_COL, X_COL, X_COL },
	{ 0., .5, 1., 0., 0., X_COL, X_COL, X_COL },
	{ 0., 1., 0., 1., .5, X_COL, X_COL, X_COL },
	{ 0., .5, 0., 1., 0., X_COL, X_COL, X_COL },
	{ .5, .5, 1., 0., 1., X_COL, X_COL, X_COL },
	{ .5, .5, 0., 1., 1., X_COL, X_COL, X_COL },
	{ .5, 0., 1., 0., .5, X_COL, X_COL, X_COL },
	{ .5, 0., 1., 0., .5, X_COL, X_COL, X_COL },
	{ .5, .5, 0., 1., 1., X_COL, X_COL, X_COL },
	{ .5, 0., 0., 1., .5, X_COL, X_COL, X_COL },
	{ 0., 1., 0., 1., 1., Z_COL, Z_COL, Z_COL },
	{ 1., 1., 0., 0., 1., Z_COL, Z_COL, Z_COL },
	{ 1., 0., 0., 0., 0., Z_COL, Z_COL, Z_COL },
	{ 0., 1., 0., 1., 1., Z_COL, Z_COL, Z_COL },
	{ .5, .5, 0., .5, .5, Z_COL, Z_COL, Z_COL },
	{ 0., .5, 0., 1., .5, Z_COL, Z_COL, Z_COL },
	{ .5, .5, 0., .5, .5, Z_COL, Z_COL, Z_COL },
	{ 1., 0., 0., 0., 0., Z_COL, Z_COL, Z_COL },
	{ .5, 0., 0., .5, 0., Z_COL, Z_COL, Z_COL },
	{ 1., 1., 1., 0., 1., Z_COL, Z_COL, Z_COL },
	{ 0., 1., 1., 1., 1., Z_COL, Z_COL, Z_COL },
	{ 1., 0., 1., 0., 0., Z_COL, Z_COL, Z_COL },
	{ .5, .5, 1., .5, .5, Z_COL, Z_COL, Z_COL },
	{ 0., 1., 1., 1., 1., Z_COL, Z_COL, Z_COL },
	{ 0., .5, 1., 1., .5, Z_COL, Z_COL, Z_COL },
	{ 1., 0., 1., 0., 0., Z_COL, Z_COL, Z_COL },
	{ .5, .5, 1., .5, .5, Z_COL, Z_COL, Z_COL },
	{ .5, 0., 1., .5, 0., Z_COL, Z_COL, Z_COL },
};

struct vertex_desc vertices_stairs_ub[] = {
	{ 1., .5, 1., 0., 1., B_COL, B_COL, B_COL, 2 },
	{ 0., .5, 1., 1., 1., B_COL, B_COL, B_COL, 2 },
	{ 1., .5, .5, 0., .5, B_COL, B_COL, B_COL, 2 },
	{ 1., .5, .5, 0., .5, B_COL, B_COL, B_COL, 2 },
	{ 0., .5, 1., 1., 1., B_COL, B_COL, B_COL, 2 },
	{ 0., .5, .5, 1., .5, B_COL, B_COL, B_COL, 2 },
	{ 1., 0., .5, 0., .5, B_COL, B_COL, B_COL, 2 },
	{ 0., 0., .5, 1., .5, B_COL, B_COL, B_COL, 2 },
	{ 1., 0., 0., 0., 0., B_COL, B_COL, B_COL, 2 },
	{ 1., 0., 0., 0., 0., B_COL, B_COL, B_COL, 2 },
	{ 0., 0., .5, 1., .5, B_COL, B_COL, B_COL, 2 },
	{ 0., 0., 0., 1., 0., B_COL, B_COL, B_COL, 2 },
	{ 1., 1., 1., 0., .5, Z_COL, Z_COL, Z_COL },
	{ 0., 1., 1., 1., .5, Z_COL, Z_COL, Z_COL },
	{ 1., .5, 1., 0., 0., Z_COL, Z_COL, Z_COL },
	{ 1., .5, 1., 0., 0., Z_COL, Z_COL, Z_COL },
	{ 0., 1., 1., 1., .5, Z_COL, Z_COL, Z_COL },
	{ 0., .5, 1., 1., 0., Z_COL, Z_COL, Z_COL },
	{ 1., .5, .5, 0., 1., Z_COL, Z_COL, Z_COL },
	{ 0., .5, .5, 1., 1., Z_COL, Z_COL, Z_COL },
	{ 1., 0., .5, 0., .5, Z_COL, Z_COL, Z_COL },
	{ 1., 0., .5, 0., .5, Z_COL, Z_COL, Z_COL },
	{ 0., .5, .5, 1., 1., Z_COL, Z_COL, Z_COL },
	{ 0., 0., .5, 1., .5, Z_COL, Z_COL, Z_COL },
	{ 0., 1., 1., 1., 1., X_COL, X_COL, X_COL },
	{ 0., 1., 0., 0., 1., X_COL, X_COL, X_COL },
	{ 0., 0., 0., 0., 0., X_COL, X_COL, X_COL },
	{ 0., 1., 1., 1., 1., X_COL, X_COL, X_COL },
	{ 0., .5, .5, .5, .5, X_COL, X_COL, X_COL },
	{ 0., .5, 1., 1., .5, X_COL, X_COL, X_COL },
	{ 0., .5, .5, .5, .5, X_COL, X_COL, X_COL },
	{ 0., 0., 0., 0., 0., X_COL, X_COL, X_COL },
	{ 0., 0., .5, .5, 0., X_COL, X_COL, X_COL },
	{ 1., 1., 0., 0., 1., X_COL, X_COL, X_COL },
	{ 1., 1., 1., 1., 1., X_COL, X_COL, X_COL },
	{ 1., 0., 0., 0., 0., X_COL, X_COL, X_COL },
	{ 1., .5, .5, .5, .5, X_COL, X_COL, X_COL },
	{ 1., 1., 1., 1., 1., X_COL, X_COL, X_COL },
	{ 1., .5, 1., 1., .5, X_COL, X_COL, X_COL },
	{ 1., 0., 0., 0., 0., X_COL, X_COL, X_COL },
	{ 1., .5, .5, .5, .5, X_COL, X_COL, X_COL },
	{ 1., 0., .5, .5, 0., X_COL, X_COL, X_COL },
};

struct vertex_desc vertices_stairs_uf[] = {
	{ 0., .5, 0., 1., 1., B_COL, B_COL, B_COL, 2 },
	{ 1., .5, 0., 0., 1., B_COL, B_COL, B_COL, 2 },
	{ 1., .5, .5, 0., .5, B_COL, B_COL, B_COL, 2 },
	{ 0., .5, 0., 1., 1., B_COL, B_COL, B_COL, 2 },
	{ 1., .5, .5, 0., .5, B_COL, B_COL, B_COL, 2 },
	{ 0., .5, .5, 1., .5, B_COL, B_COL, B_COL, 2 },
	{ 0., 0., .5, 1., .5, B_COL, B_COL, B_COL, 2 },
	{ 1., 0., .5, 0., .5, B_COL, B_COL, B_COL, 2 },
	{ 1., 0., 1., 0., 0., B_COL, B_COL, B_COL, 2 },
	{ 0., 0., .5, 1., .5, B_COL, B_COL, B_COL, 2 },
	{ 1., 0., 1., 0., 0., B_COL, B_COL, B_COL, 2 },
	{ 0., 0., 1., 1., 0., B_COL, B_COL, B_COL, 2 },
	{ 0., 1., 0., 1., .5, Z_COL, Z_COL, Z_COL },
	{ 1., 1., 0., 0., .5, Z_COL, Z_COL, Z_COL },
	{ 1., .5, 0., 0., 0., Z_COL, Z_COL, Z_COL },
	{ 0., 1., 0., 1., .5, Z_COL, Z_COL, Z_COL },
	{ 1., .5, 0., 0., 0., Z_COL, Z_COL, Z_COL },
	{ 0., .5, 0., 1., 0., Z_COL, Z_COL, Z_COL },
	{ 0., .5, .5, 1., 1., Z_COL, Z_COL, Z_COL },
	{ 1., .5, .5, 0., 1., Z_COL, Z_COL, Z_COL },
	{ 1., 0., .5, 0., .5, Z_COL, Z_COL, Z_COL },
	{ 0., .5, .5, 1., 1., Z_COL, Z_COL, Z_COL },
	{ 1., 0., .5, 0., .5, Z_COL, Z_COL, Z_COL },
	{ 0., 0., .5, 1., .5, Z_COL, Z_COL, Z_COL },
	{ 0., 1., 1., 0., 1., X_COL, X_COL, X_COL },
	{ 0., 1., 0., 1., 1., X_COL, X_COL, X_COL },
	{ 0., 0., 1., 0., 0., X_COL, X_COL, X_COL },
	{ 0., .5, .5, .5, .5, X_COL, X_COL, X_COL },
	{ 0., 1., 0., 1., 1., X_COL, X_COL, X_COL },
	{ 0., .5, 0., 1., .5, X_COL, X_COL, X_COL },
	{ 0., 0., 1., 0., 0., X_COL, X_COL, X_COL },
	{ 0., .5, .5, .5, .5, X_COL, X_COL, X_COL },
	{ 0., 0., .5, .5, 0., X_COL, X_COL, X_COL },
	{ 1., 1., 0., 1., 1., X_COL, X_COL, X_COL },
	{ 1., 1., 1., 0., 1., X_COL, X_COL, X_COL },
	{ 1., 0., 1., 0., 0., X_COL, X_COL, X_COL },
	{ 1., 1., 0., 1., 1., X_COL, X_COL, X_COL },
	{ 1., .5, .5, .5, .5, X_COL, X_COL, X_COL },
	{ 1., .5, 0., 1., .5, X_COL, X_COL, X_COL },
	{ 1., .5, .5, .5, .5, X_COL, X_COL, X_COL },
	{ 1., 0., 1., 0., 0., X_COL, X_COL, X_COL },
	{ 1., 0., .5, .5, 0., X_COL, X_COL, X_COL },
};

struct vertex_desc vertices_pane_x[] = {
	{ .46875, 0., 0., 0., 1., B_COL, B_COL, B_COL, 2 },
	{ .53125, 0., 0., 1., 1., B_COL, B_COL, B_COL, 2 },
	{ .46875, 0., 1., 0., 0., B_COL, B_COL, B_COL, 2 },
	{ .46875, 0., 1., 0., 0., B_COL, B_COL, B_COL, 2 },
	{ .53125, 0., 0., 1., 1., B_COL, B_COL, B_COL, 2 },
	{ .53125, 0., 1., 1., 0., B_COL, B_COL, B_COL, 2 },

	{ .53125, 1., 1., 0., 1., T_COL, T_COL, T_COL, 1 },
	{ .53125, 1., 0., 1., 1., T_COL, T_COL, T_COL, 1 },
	{ .46875, 1., 1., 0., 0., T_COL, T_COL, T_COL, 1 },
	{ .46875, 1., 1., 0., 0., T_COL, T_COL, T_COL, 1 },
	{ .53125, 1., 0., 1., 1., T_COL, T_COL, T_COL, 1 },
	{ .46875, 1., 0., 1., 0., T_COL, T_COL, T_COL, 1 },

	{ .46875, 0., 0., 0., 1., X_COL, X_COL, X_COL },
	{ .46875, 0., 1., 1., 1., X_COL, X_COL, X_COL },
	{ .46875, 1., 0., 0., 0., X_COL, X_COL, X_COL },
	{ .46875, 1., 0., 0., 0., X_COL, X_COL, X_COL },
	{ .46875, 0., 1., 1., 1., X_COL, X_COL, X_COL },
	{ .46875, 1., 1., 1., 0., X_COL, X_COL, X_COL },

	{ .53125, 0., 1., 0., 1., X_COL, X_COL, X_COL },
	{ .53125, 0., 0., 1., 1., X_COL, X_COL, X_COL },
	{ .53125, 1., 1., 0., 0., X_COL, X_COL, X_COL },
	{ .53125, 1., 1., 0., 0., X_COL, X_COL, X_COL },
	{ .53125, 0., 0., 1., 1., X_COL, X_COL, X_COL },
	{ .53125, 1., 0., 1., 0., X_COL, X_COL, X_COL },

	{ .53125, 0., 0., 0., 1., Z_COL, Z_COL, Z_COL },
	{ .46875, 0., 0., 1., 1., Z_COL, Z_COL, Z_COL },
	{ .53125, 1., 0., 0., 0., Z_COL, Z_COL, Z_COL },
	{ .53125, 1., 0., 0., 0., Z_COL, Z_COL, Z_COL },
	{ .46875, 0., 0., 1., 1., Z_COL, Z_COL, Z_COL },
	{ .46875, 1., 0., 1., 0., Z_COL, Z_COL, Z_COL },

	{ .46875, 0., 1., 0., 1., Z_COL, Z_COL, Z_COL },
	{ .53125, 0., 1., 1., 1., Z_COL, Z_COL, Z_COL },
	{ .46875, 1., 1., 0., 0., Z_COL, Z_COL, Z_COL },
	{ .46875, 1., 1., 0., 0., Z_COL, Z_COL, Z_COL },
	{ .53125, 0., 1., 1., 1., Z_COL, Z_COL, Z_COL },
	{ .53125, 1., 1., 1., 0., Z_COL, Z_COL, Z_COL },
};

struct vertex_desc vertices_pane_y[] = {
	{ 0., .46875, 0., 0., 1., B_COL, B_COL, B_COL, 2 },
	{ 1., .46875, 0., 1., 1., B_COL, B_COL, B_COL, 2 },
	{ 0., .46875, 1., 0., 0., B_COL, B_COL, B_COL, 2 },
	{ 0., .46875, 1., 0., 0., B_COL, B_COL, B_COL, 2 },
	{ 1., .46875, 0., 1., 1., B_COL, B_COL, B_COL, 2 },
	{ 1., .46875, 1., 1., 0., B_COL, B_COL, B_COL, 2 },

	{ 1., .53125, 1., 0., 1., T_COL, T_COL, T_COL, 1 },
	{ 1., .53125, 0., 1., 1., T_COL, T_COL, T_COL, 1 },
	{ 0., .53125, 1., 0., 0., T_COL, T_COL, T_COL, 1 },
	{ 0., .53125, 1., 0., 0., T_COL, T_COL, T_COL, 1 },
	{ 1., .53125, 0., 1., 1., T_COL, T_COL, T_COL, 1 },
	{ 0., .53125, 0., 1., 0., T_COL, T_COL, T_COL, 1 },

	{ 0., .46875, 0., 0., 1., X_COL, X_COL, X_COL },
	{ 0., .46875, 1., 1., 1., X_COL, X_COL, X_COL },
	{ 0., .53125, 0., 0., 0., X_COL, X_COL, X_COL },
	{ 0., .53125, 0., 0., 0., X_COL, X_COL, X_COL },
	{ 0., .46875, 1., 1., 1., X_COL, X_COL, X_COL },
	{ 0., .53125, 1., 1., 0., X_COL, X_COL, X_COL },

	{ 1., .46875, 1., 0., 1., X_COL, X_COL, X_COL },
	{ 1., .46875, 0., 1., 1., X_COL, X_COL, X_COL },
	{ 1., .53125, 1., 0., 0., X_COL, X_COL, X_COL },
	{ 1., .53125, 1., 0., 0., X_COL, X_COL, X_COL },
	{ 1., .46875, 0., 1., 1., X_COL, X_COL, X_COL },
	{ 1., .53125, 0., 1., 0., X_COL, X_COL, X_COL },

	{ 1., .46875, 0., 0., 1., Z_COL, Z_COL, Z_COL },
	{ 0., .46875, 0., 1., 1., Z_COL, Z_COL, Z_COL },
	{ 1., .53125, 0., 0., 0., Z_COL, Z_COL, Z_COL },
	{ 1., .53125, 0., 0., 0., Z_COL, Z_COL, Z_COL },
	{ 0., .46875, 0., 1., 1., Z_COL, Z_COL, Z_COL },
	{ 0., .53125, 0., 1., 0., Z_COL, Z_COL, Z_COL },

	{ 0., .46875, 1., 0., 1., Z_COL, Z_COL, Z_COL },
	{ 1., .46875, 1., 1., 1., Z_COL, Z_COL, Z_COL },
	{ 0., .53125, 1., 0., 0., Z_COL, Z_COL, Z_COL },
	{ 0., .53125, 1., 0., 0., Z_COL, Z_COL, Z_COL },
	{ 1., .46875, 1., 1., 1., Z_COL, Z_COL, Z_COL },
	{ 1., .53125, 1., 1., 0., Z_COL, Z_COL, Z_COL },
};

struct vertex_desc vertices_pane_z[] = {
	{ 0., 0., .46875, 0., 1., B_COL, B_COL, B_COL, 2 },
	{ 1., 0., .46875, 1., 1., B_COL, B_COL, B_COL, 2 },
	{ 0., 0., .53125, 0., 0., B_COL, B_COL, B_COL, 2 },
	{ 0., 0., .53125, 0., 0., B_COL, B_COL, B_COL, 2 },
	{ 1., 0., .46875, 1., 1., B_COL, B_COL, B_COL, 2 },
	{ 1., 0., .53125, 1., 0., B_COL, B_COL, B_COL, 2 },

	{ 1., 1., .53125, 0., 1., T_COL, T_COL, T_COL, 1 },
	{ 1., 1., .46875, 1., 1., T_COL, T_COL, T_COL, 1 },
	{ 0., 1., .53125, 0., 0., T_COL, T_COL, T_COL, 1 },
	{ 0., 1., .53125, 0., 0., T_COL, T_COL, T_COL, 1 },
	{ 1., 1., .46875, 1., 1., T_COL, T_COL, T_COL, 1 },
	{ 0., 1., .46875, 1., 0., T_COL, T_COL, T_COL, 1 },

	{ 0., 0., .46875, 0., 1., X_COL, X_COL, X_COL },
	{ 0., 0., .53125, 1., 1., X_COL, X_COL, X_COL },
	{ 0., 1., .46875, 0., 0., X_COL, X_COL, X_COL },
	{ 0., 1., .46875, 0., 0., X_COL, X_COL, X_COL },
	{ 0., 0., .53125, 1., 1., X_COL, X_COL, X_COL },
	{ 0., 1., .53125, 1., 0., X_COL, X_COL, X_COL },

	{ 1., 0., .53125, 0., 1., X_COL, X_COL, X_COL },
	{ 1., 0., .46875, 1., 1., X_COL, X_COL, X_COL },
	{ 1., 1., .53125, 0., 0., X_COL, X_COL, X_COL },
	{ 1., 1., .53125, 0., 0., X_COL, X_COL, X_COL },
	{ 1., 0., .46875, 1., 1., X_COL, X_COL, X_COL },
	{ 1., 1., .46875, 1., 0., X_COL, X_COL, X_COL },

	{ 1., 0., .46875, 0., 1., Z_COL, Z_COL, Z_COL },
	{ 0., 0., .46875, 1., 1., Z_COL, Z_COL, Z_COL },
	{ 1., 1., .46875, 0., 0., Z_COL, Z_COL, Z_COL },
	{ 1., 1., .46875, 0., 0., Z_COL, Z_COL, Z_COL },
	{ 0., 0., .46875, 1., 1., Z_COL, Z_COL, Z_COL },
	{ 0., 1., .46875, 1., 0., Z_COL, Z_COL, Z_COL },

	{ 0., 0., .53125, 0., 1., Z_COL, Z_COL, Z_COL },
	{ 1., 0., .53125, 1., 1., Z_COL, Z_COL, Z_COL },
	{ 0., 1., .53125, 0., 0., Z_COL, Z_COL, Z_COL },
	{ 0., 1., .53125, 0., 0., Z_COL, Z_COL, Z_COL },
	{ 1., 0., .53125, 1., 1., Z_COL, Z_COL, Z_COL },
	{ 1., 1., .53125, 1., 0., Z_COL, Z_COL, Z_COL },
};

struct vertex_buffer *vertex_buffer(int vbo_count)
{
	struct vertex_buffer *r = calloc(1, sizeof(*r));
	r->vbo_count = vbo_count;
	r->vbo_names = calloc(vbo_count, sizeof(*r->vbo_names));
	r->vbo_sizes = calloc(vbo_count, sizeof(*r->vbo_sizes));
	glGenBuffers(r->vbo_count, r->vbo_names);
	assert(glGetError() == GL_NO_ERROR);
	return r;
}

void vertex_buffer_destroy(struct vertex_buffer *r)
{
	assert(glGetError() == GL_NO_ERROR);
	glDeleteBuffers(r->vbo_count, r->vbo_names);
	free(r->vbo_names);
	free(r->vbo_sizes);
	free(r);
}

void vertex_buffer_enable(struct vertex_buffer *r)
{
	assert(glGetError() == GL_NO_ERROR);
}

void vertex_buffer_draw_slice(struct vertex_buffer *r, GLenum mode, size_t buf,
		size_t first, size_t count)
{
	assert(buf < r->vbo_count);
	assert(first + count <= r->vbo_sizes[buf]);
	glBindBuffer(GL_ARRAY_BUFFER, r->vbo_names[buf]);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(struct vertex), (char *)0);
	glTexCoordPointer(4, GL_FLOAT, sizeof(struct vertex), (char *)offsetof(struct vertex, u0));
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(struct vertex), (char *)offsetof(struct vertex, r));
	glDrawArrays(mode, first, count);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	assert(glGetError() == GL_NO_ERROR);
}

void vertex_buffer_draw(struct vertex_buffer *r, GLenum mode, size_t buf)
{
	vertex_buffer_draw_slice(r, mode, buf, 0, r->vbo_sizes[buf]);
}

void vertex_buffer_disable(struct vertex_buffer *r)
{
	assert(glGetError() == GL_NO_ERROR);
}

void vertex_buffer_update(struct vertex_buffer *r, size_t buf, const void *data, size_t size)
{
	assert(glGetError() == GL_NO_ERROR);
	assert(buf < r->vbo_count);
	glBindBuffer(GL_ARRAY_BUFFER, r->vbo_names[buf]);
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(struct vertex), data, GL_STATIC_DRAW);
	r->vbo_sizes[buf] = size;
	assert(glGetError() == GL_NO_ERROR);
}


