
#include "types.h"

const char *obj_names[256] = {
	[OBJ_BLOCK] = "block",
	[OBJ_SLAB] = "slab",
	[OBJ_STAIRS] = "stairs",
	[OBJ_PANE] = "pane",
	[OBJ_FLUID] = "fluid",
};

const char *mat_names[256] = {
	[MAT_DIRT] = "dirt",
	[MAT_GRASS] = "grass",
	[MAT_LIMESTONE] = "limestone",
	[MAT_LIMESTONE_COBBLE] = "limestone_cobble",
	[MAT_LIMESTONE_LBRICK] = "limestone_lbrick",
	[MAT_LIMESTONE_SBRICK] = "limestone_sbrick",
	[MAT_LIMESTONE_TILE] = "limestone_tile",
	[MAT_LIMESTONE_COLUMN] = "limestone_column",
	[MAT_LIMESTONE_BLOCK] = "limestone_block",
	[MAT_LIMESTONE_SAND] = "limestone_sand",
	[MAT_LIMESTONE_CRATE] = "limestone_crate",
	[MAT_LIMESTONE_BENCH] = "limestone_bench",
	[MAT_LIMESTONE_PIPE] = "limestone_pipe",
	[MAT_SANDSTONE] = "sandstone",
	[MAT_SANDSTONE_COBBLE] = "sandstone_cobble",
	[MAT_SANDSTONE_LBRICK] = "sandstone_lbrick",
	[MAT_SANDSTONE_SBRICK] = "sandstone_sbrick",
	[MAT_SANDSTONE_TILE] = "sandstone_tile",
	[MAT_SANDSTONE_COLUMN] = "sandstone_column",
	[MAT_SANDSTONE_BLOCK] = "sandstone_block",
	[MAT_SANDSTONE_SAND] = "sandstone_sand",
	[MAT_SANDSTONE_CRATE] = "sandstone_crate",
	[MAT_SANDSTONE_BENCH] = "sandstone_bench",
	[MAT_SANDSTONE_PIPE] = "sandstone_pipe",
	[MAT_MARBLE] = "marble",
	[MAT_MARBLE_COBBLE] = "marble_cobble",
	[MAT_MARBLE_LBRICK] = "marble_lbrick",
	[MAT_MARBLE_SBRICK] = "marble_sbrick",
	[MAT_MARBLE_TILE] = "marble_tile",
	[MAT_MARBLE_COLUMN] = "marble_column",
	[MAT_MARBLE_BLOCK] = "marble_block",
	[MAT_MARBLE_SAND] = "marble_sand",
	[MAT_MARBLE_CRATE] = "marble_crate",
	[MAT_MARBLE_BENCH] = "marble_bench",
	[MAT_MARBLE_PIPE] = "marble_pipe",
	[MAT_GRANITE] = "granite",
	[MAT_GRANITE_COBBLE] = "granite_cobble",
	[MAT_GRANITE_LBRICK] = "granite_lbrick",
	[MAT_GRANITE_SBRICK] = "granite_sbrick",
	[MAT_GRANITE_TILE] = "granite_tile",
	[MAT_GRANITE_COLUMN] = "granite_column",
	[MAT_GRANITE_BLOCK] = "granite_block",
	[MAT_GRANITE_SAND] = "granite_sand",
	[MAT_GRANITE_CRATE] = "granite_crate",
	[MAT_GRANITE_BENCH] = "granite_bench",
	[MAT_GRANITE_PIPE] = "granite_pipe",
	[MAT_BASALT] = "basalt",
	[MAT_BASALT_COBBLE] = "basalt_cobble",
	[MAT_BASALT_LBRICK] = "basalt_lbrick",
	[MAT_BASALT_SBRICK] = "basalt_sbrick",
	[MAT_BASALT_TILE] = "basalt_tile",
	[MAT_BASALT_COLUMN] = "basalt_column",
	[MAT_BASALT_BLOCK] = "basalt_block",
	[MAT_BASALT_SAND] = "basalt_sand",
	[MAT_BASALT_CRATE] = "basalt_crate",
	[MAT_BASALT_BENCH] = "basalt_bench",
	[MAT_BASALT_PIPE] = "basalt_pipe",
	[MAT_SLATE] = "slate",
	[MAT_SLATE_COBBLE] = "slate_cobble",
	[MAT_SLATE_LBRICK] = "slate_lbrick",
	[MAT_SLATE_SBRICK] = "slate_sbrick",
	[MAT_SLATE_TILE] = "slate_tile",
	[MAT_SLATE_COLUMN] = "slate_column",
	[MAT_SLATE_BLOCK] = "slate_block",
	[MAT_SLATE_SAND] = "slate_sand",
	[MAT_SLATE_CRATE] = "slate_crate",
	[MAT_SLATE_BENCH] = "slate_bench",
	[MAT_SLATE_PIPE] = "slate_pipe",
	[MAT_CLAY] = "clay",
	[MAT_CLAY_COBBLE] = "clay_cobble",
	[MAT_CLAY_LBRICK] = "clay_lbrick",
	[MAT_CLAY_SBRICK] = "clay_sbrick",
	[MAT_CLAY_TILE] = "clay_tile",
	[MAT_CLAY_COLUMN] = "clay_column",
	[MAT_CLAY_BLOCK] = "clay_block",
	[MAT_CLAY_SAND] = "clay_sand",
	[MAT_CLAY_CRATE] = "clay_crate",
	[MAT_CLAY_BENCH] = "clay_bench",
	[MAT_CLAY_PIPE] = "clay_pipe",
	[MAT_WOOD1] = "wood1",
	[MAT_WOOD1_CRATE] = "wood1_crate",
	[MAT_WOOD1_BENCH] = "wood1_bench",
	[MAT_WOOD1_PIPE] = "wood1_pipe",
	[MAT_WOOD1_LOG] = "wood1_log",
	[MAT_WOOD2] = "wood2",
	[MAT_WOOD2_CRATE] = "wood2_crate",
	[MAT_WOOD2_BENCH] = "wood2_bench",
	[MAT_WOOD2_PIPE] = "wood2_pipe",
	[MAT_WOOD2_LOG] = "wood2_log",
	[MAT_WOOD3] = "wood3",
	[MAT_WOOD3_CRATE] = "wood3_crate",
	[MAT_WOOD3_BENCH] = "wood3_bench",
	[MAT_WOOD3_PIPE] = "wood3_pipe",
	[MAT_WOOD3_LOG] = "wood3_log",
	[MAT_WOOD4] = "wood4",
	[MAT_WOOD4_CRATE] = "wood4_crate",
	[MAT_WOOD4_BENCH] = "wood4_bench",
	[MAT_WOOD4_PIPE] = "wood4_pipe",
	[MAT_WOOD4_LOG] = "wood4_log",
	[MAT_WOOD5] = "wood5",
	[MAT_WOOD5_CRATE] = "wood5_crate",
	[MAT_WOOD5_BENCH] = "wood5_bench",
	[MAT_WOOD5_PIPE] = "wood5_pipe",
	[MAT_WOOD5_LOG] = "wood5_log",
	[MAT_WOOD6] = "wood6",
	[MAT_WOOD6_CRATE] = "wood6_crate",
	[MAT_WOOD6_BENCH] = "wood6_bench",
	[MAT_WOOD6_PIPE] = "wood6_pipe",
	[MAT_WOOD6_LOG] = "wood6_log",
	[MAT_WOOD7] = "wood7",
	[MAT_WOOD7_CRATE] = "wood7_crate",
	[MAT_WOOD7_BENCH] = "wood7_bench",
	[MAT_WOOD7_PIPE] = "wood7_pipe",
	[MAT_WOOD7_LOG] = "wood7_log",
};

const char *shape_names[256] = {
	[SHAPE_NONE] = "none",
	[SHAPE_BLOCK_DN] = "block_dn",
	[SHAPE_BLOCK_UP] = "block_up",
	[SHAPE_BLOCK_LF] = "block_lf",
	[SHAPE_BLOCK_RT] = "block_rt",
	[SHAPE_BLOCK_BK] = "block_bk",
	[SHAPE_BLOCK_FT] = "block_ft",
	[SHAPE_SLAB_DN] = "slab_dn",
	[SHAPE_SLAB_UP] = "slab_up",
	[SHAPE_SLAB_LF] = "slab_lf",
	[SHAPE_SLAB_RT] = "slab_rt",
	[SHAPE_SLAB_BK] = "slab_bk",
	[SHAPE_SLAB_FT] = "slab_ft",
	[SHAPE_PANE_X] = "pane_x",
	[SHAPE_PANE_Y] = "pane_y",
	[SHAPE_PANE_Z] = "pane_z",
	[SHAPE_STAIRS_DF] = "stairs_df",
	[SHAPE_STAIRS_DL] = "stairs_dl",
	[SHAPE_STAIRS_DB] = "stairs_db",
	[SHAPE_STAIRS_DR] = "stairs_dr",
	[SHAPE_STAIRS_UF] = "stairs_uf",
	[SHAPE_STAIRS_UL] = "stairs_ul",
	[SHAPE_STAIRS_UB] = "stairs_ub",
	[SHAPE_STAIRS_UR] = "stairs_ur",
};

const char *face_names[] = {
	[FACE_FT] = "front",
	[FACE_LF] = "left",
	[FACE_BK] = "back",
	[FACE_RT] = "right",
	[FACE_UP] = "up",
	[FACE_DN] = "down",
};

struct block_traits block_traits[256][256] = {
	[MAT_LIMESTONE_BENCH] = {
		[SHAPE_BLOCK_DN] = {
			.capacity = 9,
			.is_workbench = 1,
			.is_inventory = 1,
		},
	},
	[MAT_SANDSTONE_BENCH] = {
		[SHAPE_BLOCK_DN] = {
			.capacity = 9,
			.is_workbench = 1,
			.is_inventory = 1,
		},
	},
	[MAT_MARBLE_BENCH] = {
		[SHAPE_BLOCK_DN] = {
			.capacity = 9,
			.is_workbench = 1,
			.is_inventory = 1,
		},
	},
	[MAT_GRANITE_BENCH] = {
		[SHAPE_BLOCK_DN] = {
			.capacity = 9,
			.is_workbench = 1,
			.is_inventory = 1,
		},
	},
	[MAT_BASALT_BENCH] = {
		[SHAPE_BLOCK_DN] = {
			.capacity = 9,
			.is_workbench = 1,
			.is_inventory = 1,
		},
	},
	[MAT_SLATE_BENCH] = {
		[SHAPE_BLOCK_DN] = {
			.capacity = 9,
			.is_workbench = 1,
			.is_inventory = 1,
		},
	},
	[MAT_CLAY_BENCH] = {
		[SHAPE_BLOCK_DN] = {
			.capacity = 9,
			.is_workbench = 1,
			.is_inventory = 1,
		},
	},
	[MAT_WOOD1_BENCH] = {
		[SHAPE_BLOCK_DN] = {
			.capacity = 9,
			.is_workbench = 1,
			.is_inventory = 1,
		},
	},
	[MAT_WOOD2_BENCH] = {
		[SHAPE_BLOCK_DN] = {
			.capacity = 9,
			.is_workbench = 1,
			.is_inventory = 1,
		},
	},
	[MAT_WOOD3_BENCH] = {
		[SHAPE_BLOCK_DN] = {
			.capacity = 9,
			.is_workbench = 1,
			.is_inventory = 1,
		},
	},
	[MAT_WOOD4_BENCH] = {
		[SHAPE_BLOCK_DN] = {
			.capacity = 9,
			.is_workbench = 1,
			.is_inventory = 1,
		},
	},
	[MAT_WOOD5_BENCH] = {
		[SHAPE_BLOCK_DN] = {
			.capacity = 9,
			.is_workbench = 1,
			.is_inventory = 1,
		},
	},
	[MAT_WOOD6_BENCH] = {
		[SHAPE_BLOCK_DN] = {
			.capacity = 9,
			.is_workbench = 1,
			.is_inventory = 1,
		},
	},
	[MAT_WOOD7_BENCH] = {
		[SHAPE_BLOCK_DN] = {
			.capacity = 9,
			.is_workbench = 1,
			.is_inventory = 1,
		},
	},
	[MAT_LIMESTONE_CRATE] = {
		[SHAPE_BLOCK_DN] = {
			.capacity = 16,
			.is_inventory = 1,
		},
	},
	[MAT_SANDSTONE_CRATE] = {
		[SHAPE_BLOCK_DN] = {
			.capacity = 16,
			.is_inventory = 1,
		},
	},
	[MAT_MARBLE_CRATE] = {
		[SHAPE_BLOCK_DN] = {
			.capacity = 16,
			.is_inventory = 1,
		},
	},
	[MAT_GRANITE_CRATE] = {
		[SHAPE_BLOCK_DN] = {
			.capacity = 16,
			.is_inventory = 1,
		},
	},
	[MAT_BASALT_CRATE] = {
		[SHAPE_BLOCK_DN] = {
			.capacity = 16,
			.is_inventory = 1,
		},
	},
	[MAT_SLATE_CRATE] = {
		[SHAPE_BLOCK_DN] = {
			.capacity = 16,
			.is_inventory = 1,
		},
	},
	[MAT_CLAY_CRATE] = {
		[SHAPE_BLOCK_DN] = {
			.capacity = 16,
			.is_inventory = 1,
		},
	},
	[MAT_WOOD1_CRATE] = {
		[SHAPE_BLOCK_DN] = {
			.capacity = 16,
			.is_inventory = 1,
		},
	},
	[MAT_WOOD2_CRATE] = {
		[SHAPE_BLOCK_DN] = {
			.capacity = 16,
			.is_inventory = 1,
		},
	},
	[MAT_WOOD3_CRATE] = {
		[SHAPE_BLOCK_DN] = {
			.capacity = 16,
			.is_inventory = 1,
		},
	},
	[MAT_WOOD4_CRATE] = {
		[SHAPE_BLOCK_DN] = {
			.capacity = 16,
			.is_inventory = 1,
		},
	},
	[MAT_WOOD5_CRATE] = {
		[SHAPE_BLOCK_DN] = {
			.capacity = 16,
			.is_inventory = 1,
		},
	},
	[MAT_WOOD6_CRATE] = {
		[SHAPE_BLOCK_DN] = {
			.capacity = 16,
			.is_inventory = 1,
		},
	},
	[MAT_WOOD7_CRATE] = {
		[SHAPE_BLOCK_DN] = {
			.capacity = 16,
			.is_inventory = 1,
		},
	},
	[MAT_LIMESTONE_PIPE] = {
		[SHAPE_BLOCK_DN] = { .capacity = 9, },
		[SHAPE_BLOCK_UP] = { .capacity = 9, },
		[SHAPE_BLOCK_LF] = { .capacity = 9, },
		[SHAPE_BLOCK_RT] = { .capacity = 9, },
		[SHAPE_BLOCK_BK] = { .capacity = 9, },
		[SHAPE_BLOCK_FT] = { .capacity = 9, },
	},
	[MAT_SANDSTONE_PIPE] = {
		[SHAPE_BLOCK_DN] = { .capacity = 9, },
		[SHAPE_BLOCK_UP] = { .capacity = 9, },
		[SHAPE_BLOCK_LF] = { .capacity = 9, },
		[SHAPE_BLOCK_RT] = { .capacity = 9, },
		[SHAPE_BLOCK_BK] = { .capacity = 9, },
		[SHAPE_BLOCK_FT] = { .capacity = 9, },
	},
	[MAT_MARBLE_PIPE] = {
		[SHAPE_BLOCK_DN] = { .capacity = 9, },
		[SHAPE_BLOCK_UP] = { .capacity = 9, },
		[SHAPE_BLOCK_LF] = { .capacity = 9, },
		[SHAPE_BLOCK_RT] = { .capacity = 9, },
		[SHAPE_BLOCK_BK] = { .capacity = 9, },
		[SHAPE_BLOCK_FT] = { .capacity = 9, },
	},
	[MAT_GRANITE_PIPE] = {
		[SHAPE_BLOCK_DN] = { .capacity = 9, },
		[SHAPE_BLOCK_UP] = { .capacity = 9, },
		[SHAPE_BLOCK_LF] = { .capacity = 9, },
		[SHAPE_BLOCK_RT] = { .capacity = 9, },
		[SHAPE_BLOCK_BK] = { .capacity = 9, },
		[SHAPE_BLOCK_FT] = { .capacity = 9, },
	},
	[MAT_BASALT_PIPE] = {
		[SHAPE_BLOCK_DN] = { .capacity = 9, },
		[SHAPE_BLOCK_UP] = { .capacity = 9, },
		[SHAPE_BLOCK_LF] = { .capacity = 9, },
		[SHAPE_BLOCK_RT] = { .capacity = 9, },
		[SHAPE_BLOCK_BK] = { .capacity = 9, },
		[SHAPE_BLOCK_FT] = { .capacity = 9, },
	},
	[MAT_SLATE_PIPE] = {
		[SHAPE_BLOCK_DN] = { .capacity = 9, },
		[SHAPE_BLOCK_UP] = { .capacity = 9, },
		[SHAPE_BLOCK_LF] = { .capacity = 9, },
		[SHAPE_BLOCK_RT] = { .capacity = 9, },
		[SHAPE_BLOCK_BK] = { .capacity = 9, },
		[SHAPE_BLOCK_FT] = { .capacity = 9, },
	},
	[MAT_CLAY_PIPE] = {
		[SHAPE_BLOCK_DN] = { .capacity = 9, },
		[SHAPE_BLOCK_UP] = { .capacity = 9, },
		[SHAPE_BLOCK_LF] = { .capacity = 9, },
		[SHAPE_BLOCK_RT] = { .capacity = 9, },
		[SHAPE_BLOCK_BK] = { .capacity = 9, },
		[SHAPE_BLOCK_FT] = { .capacity = 9, },
	},
	[MAT_WOOD1_PIPE] = {
		[SHAPE_BLOCK_DN] = { .capacity = 9, },
		[SHAPE_BLOCK_UP] = { .capacity = 9, },
		[SHAPE_BLOCK_LF] = { .capacity = 9, },
		[SHAPE_BLOCK_RT] = { .capacity = 9, },
		[SHAPE_BLOCK_BK] = { .capacity = 9, },
		[SHAPE_BLOCK_FT] = { .capacity = 9, },
	},
	[MAT_WOOD2_PIPE] = {
		[SHAPE_BLOCK_DN] = { .capacity = 9, },
		[SHAPE_BLOCK_UP] = { .capacity = 9, },
		[SHAPE_BLOCK_LF] = { .capacity = 9, },
		[SHAPE_BLOCK_RT] = { .capacity = 9, },
		[SHAPE_BLOCK_BK] = { .capacity = 9, },
		[SHAPE_BLOCK_FT] = { .capacity = 9, },
	},
	[MAT_WOOD3_PIPE] = {
		[SHAPE_BLOCK_DN] = { .capacity = 9, },
		[SHAPE_BLOCK_UP] = { .capacity = 9, },
		[SHAPE_BLOCK_LF] = { .capacity = 9, },
		[SHAPE_BLOCK_RT] = { .capacity = 9, },
		[SHAPE_BLOCK_BK] = { .capacity = 9, },
		[SHAPE_BLOCK_FT] = { .capacity = 9, },
	},
	[MAT_WOOD4_PIPE] = {
		[SHAPE_BLOCK_DN] = { .capacity = 9, },
		[SHAPE_BLOCK_UP] = { .capacity = 9, },
		[SHAPE_BLOCK_LF] = { .capacity = 9, },
		[SHAPE_BLOCK_RT] = { .capacity = 9, },
		[SHAPE_BLOCK_BK] = { .capacity = 9, },
		[SHAPE_BLOCK_FT] = { .capacity = 9, },
	},
	[MAT_WOOD5_PIPE] = {
		[SHAPE_BLOCK_DN] = { .capacity = 9, },
		[SHAPE_BLOCK_UP] = { .capacity = 9, },
		[SHAPE_BLOCK_LF] = { .capacity = 9, },
		[SHAPE_BLOCK_RT] = { .capacity = 9, },
		[SHAPE_BLOCK_BK] = { .capacity = 9, },
		[SHAPE_BLOCK_FT] = { .capacity = 9, },
	},
	[MAT_WOOD6_PIPE] = {
		[SHAPE_BLOCK_DN] = { .capacity = 9, },
		[SHAPE_BLOCK_UP] = { .capacity = 9, },
		[SHAPE_BLOCK_LF] = { .capacity = 9, },
		[SHAPE_BLOCK_RT] = { .capacity = 9, },
		[SHAPE_BLOCK_BK] = { .capacity = 9, },
		[SHAPE_BLOCK_FT] = { .capacity = 9, },
	},
	[MAT_WOOD7_PIPE] = {
		[SHAPE_BLOCK_DN] = { .capacity = 9, },
	},
};

#define BLOCK_DROPS(m1, m2) \
do { \
	block_traits[m1][SHAPE_BLOCK_DN].drop = slot(OBJ_BLOCK, m2, 1);\
	block_traits[m1][SHAPE_BLOCK_UP].drop = slot(OBJ_BLOCK, m2, 1);\
	block_traits[m1][SHAPE_BLOCK_LF].drop = slot(OBJ_BLOCK, m2, 1);\
	block_traits[m1][SHAPE_BLOCK_RT].drop = slot(OBJ_BLOCK, m2, 1);\
	block_traits[m1][SHAPE_BLOCK_BK].drop = slot(OBJ_BLOCK, m2, 1);\
	block_traits[m1][SHAPE_BLOCK_FT].drop = slot(OBJ_BLOCK, m2, 1);\
} while (0)

#define SLAB_DROPS(m1, m2) \
do { \
	block_traits[m1][SHAPE_SLAB_DN].drop = slot(OBJ_SLAB, m2, 1);\
	block_traits[m1][SHAPE_SLAB_UP].drop = slot(OBJ_SLAB, m2, 1);\
	block_traits[m1][SHAPE_SLAB_LF].drop = slot(OBJ_SLAB, m2, 1);\
	block_traits[m1][SHAPE_SLAB_RT].drop = slot(OBJ_SLAB, m2, 1);\
	block_traits[m1][SHAPE_SLAB_BK].drop = slot(OBJ_SLAB, m2, 1);\
	block_traits[m1][SHAPE_SLAB_FT].drop = slot(OBJ_SLAB, m2, 1);\
} while (0)

#define STAIR_DROPS(m1, m2) \
do { \
	block_traits[m1][SHAPE_STAIRS_DF].drop = slot(OBJ_STAIRS, m2, 1);\
	block_traits[m1][SHAPE_STAIRS_DL].drop = slot(OBJ_STAIRS, m2, 1);\
	block_traits[m1][SHAPE_STAIRS_DB].drop = slot(OBJ_STAIRS, m2, 1);\
	block_traits[m1][SHAPE_STAIRS_DR].drop = slot(OBJ_STAIRS, m2, 1);\
	block_traits[m1][SHAPE_STAIRS_UF].drop = slot(OBJ_STAIRS, m2, 1);\
	block_traits[m1][SHAPE_STAIRS_UL].drop = slot(OBJ_STAIRS, m2, 1);\
	block_traits[m1][SHAPE_STAIRS_UB].drop = slot(OBJ_STAIRS, m2, 1);\
	block_traits[m1][SHAPE_STAIRS_UR].drop = slot(OBJ_STAIRS, m2, 1);\
} while (0)

#define PANE_DROPS(m1, m2) \
do { \
	block_traits[m1][SHAPE_PANE_X].drop = slot(OBJ_PANE, m2, 1);\
	block_traits[m1][SHAPE_PANE_Y].drop = slot(OBJ_PANE, m2, 1);\
	block_traits[m1][SHAPE_PANE_Z].drop = slot(OBJ_PANE, m2, 1);\
} while (0)


#define COMMON_DROPS(m1, m2) \
do { \
	BLOCK_DROPS(m1,m2);\
	SLAB_DROPS(m1, m2);\
	STAIR_DROPS(m1, m2);\
	PANE_DROPS(m1, m2);\
} while (0)

#define STONE_DROPS(mat) \
do {\
	COMMON_DROPS(mat, mat##_COBBLE); \
	COMMON_DROPS(mat##_COBBLE, mat##_COBBLE); \
	COMMON_DROPS(mat##_BLOCK, mat##_BLOCK); \
	COMMON_DROPS(mat##_LBRICK, mat##_LBRICK); \
	COMMON_DROPS(mat##_SBRICK, mat##_SBRICK); \
	COMMON_DROPS(mat##_TILE, mat##_TILE); \
	COMMON_DROPS(mat##_COLUMN, mat##_COLUMN); \
	COMMON_DROPS(mat##_SAND, mat##_SAND); \
	COMMON_DROPS(mat##_BENCH, mat##_BENCH);\
	COMMON_DROPS(mat##_CRATE, mat##_CRATE);\
	COMMON_DROPS(mat##_PIPE, mat##_PIPE);\
} while (0)

#define WOOD_DROPS(mat) \
do {\
	COMMON_DROPS(mat, mat); \
	COMMON_DROPS(mat##_LOG, mat##_LOG); \
	COMMON_DROPS(mat##_BENCH, mat##_BENCH);\
	COMMON_DROPS(mat##_CRATE, mat##_CRATE);\
	COMMON_DROPS(mat##_PIPE, mat##_PIPE);\
} while (0)

void block_traits_init(void)
{
	COMMON_DROPS(MAT_GRASS, MAT_DIRT);
	COMMON_DROPS(MAT_DIRT, MAT_DIRT);
	STONE_DROPS(MAT_LIMESTONE);
	STONE_DROPS(MAT_SANDSTONE);
	STONE_DROPS(MAT_MARBLE);
	STONE_DROPS(MAT_GRANITE);
	STONE_DROPS(MAT_BASALT);
	STONE_DROPS(MAT_SLATE);
	STONE_DROPS(MAT_CLAY);
	WOOD_DROPS(MAT_WOOD1);
	WOOD_DROPS(MAT_WOOD2);
	WOOD_DROPS(MAT_WOOD3);
	WOOD_DROPS(MAT_WOOD4);
	WOOD_DROPS(MAT_WOOD5);
	WOOD_DROPS(MAT_WOOD6);
	WOOD_DROPS(MAT_WOOD7);
}

