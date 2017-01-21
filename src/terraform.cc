/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "terraform.h"

#include "simplex.h"

/*----------------------------------------------------------------------------
 * More complex noise, adding several octaves. 2D version
 *----------------------------------------------------------------------------*/
double noise_2d(double a, double b, double zoom, int steps)
{
    double k = .5;
    double ret = .5;

    a /= zoom;
    b /= zoom;

    while (steps--) {
	ret += noise_simplex_2d(a, b) * k;
	a *= 2;
	b *= 2;
	k *= .5;
    }

    return ret;
}

/*----------------------------------------------------------------------------
 * More complex noise, adding several octaves. 2D version
 *----------------------------------------------------------------------------*/
double noise_3d(double a, double b, double c, double zoom, int steps)
{
    double k = .5;
    double ret = .5;

    a /= zoom;
    b /= zoom;
    c /= zoom;

    while (steps--) {
	ret += noise_simplex_3d(a, b, c) * k;
	a *= 2;
	b *= 2;
	c *= 2;
	k *= .5;
    }

    return ret;
}

/*----------------------------------------------------------------------------
 * More complex noise, adding several octaves. 2D version
 *----------------------------------------------------------------------------*/
double noise_4d(double a, double b, double c, double d, double zoom, int steps)
{
    double k = .5;
    double ret = .5;

    a /= zoom;
    b /= zoom;
    c /= zoom;
    d /= zoom;

    while (steps--) {
	ret += noise_simplex_4d(a, b, c, d) * k;
	a *= 2;
	b *= 2;
	c *= 2;
	d *= 2;
	k *= .5;
    }

    return ret;
}

int terraform(int64_t seed, Chunk *c)
{
	v2ll p;
	uint64_t u, v;
	int x, y, z;
	int height, heat, humidity;

	p = c->get_p();

	for (x = 0, u = p.x; x < CHUNK_W; ++x, ++u) {
		for (z = 0, v = p.y; z < CHUNK_D; ++z, ++v) {
			// height = CHUNK_H * (0.5 + noise_3d(seed, u, v, 100, 4) * 0.1);
			height = CHUNK_H / 2;
			for (y = 0; y < height - 4; ++y) {
				c->set_mat(v3ll(x, y, z), MAT_LIMESTONE);
				c->set_shape(v3ll(x, y, z), SHAPE_BLOCK_DN);
			}
			for (; y < height - 1; ++y) {
				c->set_mat(v3ll(x, y, z), MAT_DIRT);
				c->set_shape(v3ll(x, y, z), SHAPE_BLOCK_DN);
			}
			for (; y < height; ++y) {
				c->set_mat(v3ll(x, y, z), MAT_GRASS);
				c->set_shape(v3ll(x, y, z), SHAPE_BLOCK_DN);
			}
		}
	}
	return 0;
}

