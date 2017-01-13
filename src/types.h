
#ifndef VOXEL_TYPES_H_
#define VOXEL_TYPES_H_

enum {
	OBJ_BLOCK,	/* full block */
	OBJ_SLAB,	/* half block */
	OBJ_STAIRS,	/* stairs */
	OBJ_PANE,	/* thin pane in the middle */
	OBJ_FLUID,	/* object for flowsims (debug only) */
	OBJ_COUNT
};

enum {
	MAT_LIMESTONE,
	MAT_DIRT,
	MAT_WOOD,
	MAT_TILE,
	MAT_GRASS,
	MAT_STONEBRICK,
	MAT_WORKBENCH,
	MAT_CRATE,
	MAT_PIPE,
	MAT_COBBLE,
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
};

extern const char *obj_names[];
extern const char *mat_names[];
extern const char *face_names[];
extern const char *shape_names[];

#endif
