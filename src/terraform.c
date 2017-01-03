
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

int terraform(int64_t seed, struct chunk *c)
{
	uint64_t u, v;
	int x, y, z;
	int height, heat, humidity;

	for (x = 0, u = c->x; x < CHUNK_W; ++x, ++u) {
		for (z = 0, v = c->z; z < CHUNK_D; ++z, ++v) {
			height = CHUNK_H * (0.5 + noise_3d(seed, u, v, 100, 4) * 0.1);
//			height = CHUNK_H / 2;
			for (y = 0; y < height; ++y)
				CHUNK_AT(c, mat, x, y, z) = 1;
				CHUNK_AT(c, shape, x, y, z) = 1;
		}
	}
}

