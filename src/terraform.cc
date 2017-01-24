/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "terraform.h"

#include "simplex.h"
#include "box2.h"

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
	v2ll p0 = c->get_p();
	box2ll bb(p0.x, p0.y, p0.x + Chunk::W - 1, p0.y + Chunk::D - 1);

	for (auto p : bb) {
		float height = .5 + noise_simplex_4d(seed, 0, p.x / 2000., p.y / 2000.) * .2;
		float noise1 = .5 + noise_simplex_4d(seed, 10, p.x / 2000., p.y / 2000.) * .2;
		float noise2 = noise_simplex_4d(seed, 20, p.x / 100., p.y / 100.);
		float noise3 = noise_simplex_4d(seed, 20, p.x / 20., p.y / 20.);
		int ground = Chunk::H * height + noise1 * (25 * noise2 + 5 * noise3);
		v3ll q(p.x - p0.x, 0, p.y - p0.y);
		while(q.y < ground - 4) {
			c->set_mat(q, MAT_LIMESTONE);
			c->set_shape(q, SHAPE_BLOCK_DN);
			++q.y;
		}
		while (q.y < ground - 1) {
			c->set_mat(q, MAT_DIRT);
			c->set_shape(q, SHAPE_BLOCK_DN);
			++q.y;
		}
		while (q.y < ground) {
			c->set_mat(q, MAT_GRASS);
			c->set_shape(q, SHAPE_BLOCK_DN);
			++q.y;
		}
		while (q.y < Chunk::H) {
			c->set_mat(q, 0);
			c->set_shape(q, SHAPE_NONE);
			++q.y;
		}
	}
	return 0;
}

