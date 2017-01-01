
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

int terraform(int64_t seed, int64_t i, int64_t j, struct chunk *c)
{
	uint64_t u, v;
	int x, y, z;
	int height, heat, humidity;

	c->x = i * CHUNK_W;
	c->z = j * CHUNK_D;

	for (x = 0, u = c->x; x < CHUNK_W; ++x, ++u) {
		for (z = 0, v = c->z; z < CHUNK_D; ++z, ++v) {
			height = noise_3d(x, z, seed, 1, 4);
			for (y = 0; y < height; ++y)
				CHUNK_AT(c, mat, x, y, z) = 1;
		}
	}
}

