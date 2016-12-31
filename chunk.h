

#ifndef VX_CHUNK_H_
#define VX_CHUNK_H_

#include <stdint.h>

#define VX_CHUNK_W 16
#define VX_CHUNK_D 16
#define VX_CHUNK_H 256

#define VX_CHUNK_SIZE (VX_CHUNK_W * VX_CHUNK_D * VX_CHUNK_H)

struct cell {
	int8_t material;
	int8_t shape;
	int16_t extra;
};

struct cell_entity {
	struct cell_entity *next;
};

struct chunk {
	int64_t x, y, z;
	int8_t *mat;
	int8_t *shape;
	int8_t *light1;
	int8_t *light2;
	struct cell_entity *entities;
};

struct chunk *chunk_new(void);

void chunk_destroy(struct chunk *c);

int chunk_load(struct chunk *c, int fd);

int chunk_save(struct chunk *c, int fd);

#endif


