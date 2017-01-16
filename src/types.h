
#ifndef VOXEL_TYPES_H_
#define VOXEL_TYPES_H_

#include <string.h>
#include <stdint.h>

enum {
	OBJ_BLOCK,	/* full block */
	OBJ_SLAB,	/* half block */
	OBJ_STAIRS,	/* stairs */
	OBJ_PANE,	/* thin pane in the middle */
	OBJ_FLUID,	/* object for flowsims (debug only) */
	OBJ_COUNT
};

enum {
	MAT_DIRT,
	MAT_GRASS,
	MAT_LIMESTONE = 0x10,
	MAT_LIMESTONE_COBBLE,
	MAT_LIMESTONE_LBRICK,
	MAT_LIMESTONE_SBRICK,
	MAT_LIMESTONE_TILE,
	MAT_LIMESTONE_COLUMN,
	MAT_LIMESTONE_BLOCK,
	MAT_LIMESTONE_SAND,
	MAT_LIMESTONE_CRATE,
	MAT_LIMESTONE_BENCH,
	MAT_LIMESTONE_PIPE,
	MAT_LIMESTONE_BOARD,
	MAT_SANDSTONE = 0x20,
	MAT_SANDSTONE_COBBLE,
	MAT_SANDSTONE_LBRICK,
	MAT_SANDSTONE_SBRICK,
	MAT_SANDSTONE_TILE,
	MAT_SANDSTONE_COLUMN,
	MAT_SANDSTONE_BLOCK,
	MAT_SANDSTONE_SAND,
	MAT_SANDSTONE_CRATE,
	MAT_SANDSTONE_BENCH,
	MAT_SANDSTONE_PIPE,
	MAT_SANDSTONE_BOARD,
	MAT_MARBLE = 0x30,
	MAT_MARBLE_COBBLE,
	MAT_MARBLE_LBRICK,
	MAT_MARBLE_SBRICK,
	MAT_MARBLE_TILE,
	MAT_MARBLE_COLUMN,
	MAT_MARBLE_BLOCK,
	MAT_MARBLE_SAND,
	MAT_MARBLE_CRATE,
	MAT_MARBLE_BENCH,
	MAT_MARBLE_PIPE,
	MAT_MARBLE_BOARD,
	MAT_GRANITE = 0x40,
	MAT_GRANITE_COBBLE,
	MAT_GRANITE_LBRICK,
	MAT_GRANITE_SBRICK,
	MAT_GRANITE_TILE,
	MAT_GRANITE_COLUMN,
	MAT_GRANITE_BLOCK,
	MAT_GRANITE_SAND,
	MAT_GRANITE_CRATE,
	MAT_GRANITE_BENCH,
	MAT_GRANITE_PIPE,
	MAT_GRANITE_BOARD,
	MAT_BASALT = 0x50,
	MAT_BASALT_COBBLE,
	MAT_BASALT_LBRICK,
	MAT_BASALT_SBRICK,
	MAT_BASALT_TILE,
	MAT_BASALT_COLUMN,
	MAT_BASALT_BLOCK,
	MAT_BASALT_SAND,
	MAT_BASALT_CRATE,
	MAT_BASALT_BENCH,
	MAT_BASALT_PIPE,
	MAT_BASALT_BOARD,
	MAT_SLATE = 0x60,
	MAT_SLATE_COBBLE,
	MAT_SLATE_LBRICK,
	MAT_SLATE_SBRICK,
	MAT_SLATE_TILE,
	MAT_SLATE_COLUMN,
	MAT_SLATE_BLOCK,
	MAT_SLATE_SAND,
	MAT_SLATE_CRATE,
	MAT_SLATE_BENCH,
	MAT_SLATE_PIPE,
	MAT_SLATE_BOARD,
	MAT_CLAY = 0x70,
	MAT_CLAY_COBBLE,
	MAT_CLAY_LBRICK,
	MAT_CLAY_SBRICK,
	MAT_CLAY_TILE,
	MAT_CLAY_COLUMN,
	MAT_CLAY_BLOCK,
	MAT_CLAY_SAND,
	MAT_CLAY_CRATE,
	MAT_CLAY_BENCH,
	MAT_CLAY_PIPE,
	MAT_CLAY_BOARD,
	MAT_WOOD1 = 0x80,
	MAT_WOOD1_CRATE,
	MAT_WOOD1_BENCH,
	MAT_WOOD1_PIPE,
	MAT_WOOD1_LOG,
	MAT_WOOD1_BOARD,
	MAT_WOOD2 = 0x90,
	MAT_WOOD2_CRATE,
	MAT_WOOD2_BENCH,
	MAT_WOOD2_PIPE,
	MAT_WOOD2_LOG,
	MAT_WOOD2_BOARD,
	MAT_WOOD3 = 0xA0,
	MAT_WOOD3_CRATE,
	MAT_WOOD3_BENCH,
	MAT_WOOD3_PIPE,
	MAT_WOOD3_LOG,
	MAT_WOOD3_BOARD,
	MAT_WOOD4 = 0xB0,
	MAT_WOOD4_CRATE,
	MAT_WOOD4_BENCH,
	MAT_WOOD4_PIPE,
	MAT_WOOD4_LOG,
	MAT_WOOD4_BOARD,
	MAT_WOOD5 = 0xC0,
	MAT_WOOD5_CRATE,
	MAT_WOOD5_BENCH,
	MAT_WOOD5_PIPE,
	MAT_WOOD5_LOG,
	MAT_WOOD5_BOARD,
	MAT_WOOD6 = 0xD0,
	MAT_WOOD6_CRATE,
	MAT_WOOD6_BENCH,
	MAT_WOOD6_PIPE,
	MAT_WOOD6_LOG,
	MAT_WOOD6_BOARD,
	MAT_WOOD7 = 0xE0,
	MAT_WOOD7_CRATE,
	MAT_WOOD7_BENCH,
	MAT_WOOD7_PIPE,
	MAT_WOOD7_LOG,
	MAT_WOOD7_BOARD,
	MAT_COUNT,
};

enum {
	SHAPE_NONE,
	SHAPE_BLOCK_DN,
	SHAPE_BLOCK_UP,
	SHAPE_BLOCK_LF,
	SHAPE_BLOCK_RT,
	SHAPE_BLOCK_BK,
	SHAPE_BLOCK_FT,
	SHAPE_SLAB_DN,
	SHAPE_SLAB_UP,
	SHAPE_SLAB_LF,
	SHAPE_SLAB_RT,
	SHAPE_SLAB_BK,
	SHAPE_SLAB_FT,
	SHAPE_PANE_X,
	SHAPE_PANE_Y,
	SHAPE_PANE_Z,
	SHAPE_STAIRS_DF,
	SHAPE_STAIRS_DL,
	SHAPE_STAIRS_DB,
	SHAPE_STAIRS_DR,
	SHAPE_STAIRS_UF,
	SHAPE_STAIRS_UL,
	SHAPE_STAIRS_UB,
	SHAPE_STAIRS_UR,
	SHAPE_COUNT,
};

enum {
	FACE_FT,
	FACE_LF,
	FACE_BK,
	FACE_RT,
	FACE_UP,
	FACE_DN,
	FACE_COUNT,
};

struct slot {
	uint8_t obj;
	uint8_t mat;
	uint8_t num;
};

struct block_traits {
	unsigned char capacity;
	struct slot drop;
	int is_workbench : 1;
	int is_inventory : 1;
	int is_board : 1;
};

static inline struct slot slot(int obj, int mat, int num)
{
	return (struct slot){ obj, mat, num };
}

extern const char *obj_names[];
extern const char *mat_names[];
extern const char *face_names[];
extern const char *shape_names[];

extern struct block_traits block_traits[256][256];

static inline int obj_from_name(const char *name)
{
	for (int obj = 0; obj < OBJ_COUNT; ++obj)
		if (obj_names[obj] && strcmp(obj_names[obj], name) == 0)
			return obj;
	return -1;
}

static inline int mat_from_name(const char *name)
{
	for (int mat = 0; mat < MAT_COUNT; ++mat)
		if (mat_names[mat] && strcmp(mat_names[mat], name) == 0)
			return mat;
	return -1;
}

static inline int face_from_name(const char *name)
{
	for (int face = 0; face < FACE_COUNT; ++face)
		if (face_names[face] && strcmp(face_names[face], name) == 0)
			return face;
	return -1;
}

static inline int shape_from_name(const char *name)
{
	for (int shape = 0; shape < SHAPE_COUNT; ++shape)
		if (shape_names[shape] && strcmp(shape_names[shape], name) == 0)
			return shape;
	return -1;
}

void block_traits_init(void);

#endif
