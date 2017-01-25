/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "terraform.h"

#include <algorithm>

#include "simplex.h"
#include "box2.h"

#define SEA_LEVEL 128

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

template<typename T> static inline T interp(T a, T b, T c, T d, const v2f k)
{
	T e = b * k.x + a * (1 - k.x);
	T f = d * k.x + c * (1 - k.x);
	return f * k.y + e * (1 - k.y);
}

struct Biome {
	float a, b, c;
	float noise1;
	float noise2;
	float sdepth;
	float height;

	Biome(const v2ll &p, long long seed)
	{
		// (-1,1) determines if the column is land or ocean
		a = noise_3d(p.x, p.y, seed, 1000, 7);
		if (a < .4)
			a = -1;
		else if (a < .6)
			a = (a - .5) * 10;
		else
			a = 1;
		// (0,1) determines overall height
		b = noise_3d(p.x, p.y, seed + 1000, 1000, 4);
		// (0,1) determines humidity
		c = 1 + a + noise_3d(p.x, p.y, seed + 2000, 1000, 1);
		if (b < .2) {
			/* Swamp */
			noise1 = std::max(6.0, noise_2d(p.x, p.y, 20, 3));
			noise2 = 0;
			sdepth = 4;
			height = 0;
		} else if (b < .6) {
			/* Plains */
			noise1 = -6 * noise_2d(p.x, p.y, 100, 3);
			noise2 = 0;
			sdepth = 4;
			height = .2;
		} else {
			/* Hills */
			noise1 = 0;
			noise2 = 32;
			sdepth = 1;
			height = .8;
		}
	}

	Biome(const Biome *bv, const v2f &p)
	{
		a = interp(bv[0].a, bv[1].a, bv[2].a, bv[3].a, p);
		b = interp(bv[0].b, bv[1].b, bv[2].b, bv[3].b, p);
		c = interp(bv[0].c, bv[1].c, bv[2].c, bv[3].c, p);
		noise1 = interp(bv[0].noise1, bv[1].noise1, bv[2].noise1, bv[3].noise1, p);
		noise2 = interp(bv[0].noise2, bv[1].noise2, bv[2].noise2, bv[3].noise2, p);
		sdepth = interp(bv[0].sdepth, bv[1].sdepth, bv[2].sdepth, bv[3].sdepth, p);
		height = interp(bv[0].height, bv[1].height, bv[2].height, bv[3].height, p);
	}
};

void terraform_column(const Biome &b, const v2ll &p, Chunk *c)
{
	float ground = SEA_LEVEL + b.a * b.b * 24 + b.noise1;

	v3ll q(p.x, 0, p.y);
	while(q.y < ground - b.noise2) {
		c->set_mat(q, MAT_LIMESTONE);
		c->set_shape(q, SHAPE_BLOCK_DN);
		++q.y;
	}
	{
		float d = -1;
		while (q.y < ground) {
			float k = noise_3d(q.x + c->get_p().x, q.y, q.z + c->get_p().y, 100, 3);
			if (k < d) {
				c->set_mat(q, 0);
				c->set_shape(q, SHAPE_NONE);
			} else {
				c->set_mat(q, MAT_LIMESTONE);
				c->set_shape(q, SHAPE_BLOCK_DN);
			}
			++q.y;
			d += 2.0 / b.noise2;
		}
	}
	while (q.y < Chunk::H) {
		c->set_mat(q, 0);
		c->set_shape(q, SHAPE_NONE);
		++q.y;
	}
	{
		int d = 0;
		for (q.y = Chunk::H - 1; q.y >= 0; --q.y) {
			if (c->get_shape(q) == SHAPE_BLOCK_DN) {
				c->set_mat(q, MAT_GRASS);
				if (++d > 2)
					break;
			}
		}
	}
}


int terraform(int64_t seed, Chunk *c)
{
	v2ll p0 = c->get_p();
	box2ll bb(p0.x, p0.y, p0.x + Chunk::W, p0.y + Chunk::D);

	Biome b[4] = {
		Biome(v2ll(bb.x0, bb.y0), seed),
		Biome(v2ll(bb.x1, bb.y0), seed),
		Biome(v2ll(bb.x0, bb.y1), seed),
		Biome(v2ll(bb.x1, bb.y1), seed)
	};

	v2ll p;
	v2f q;
	for (p.x = bb.x0; p.x < bb.x1; ++p.x) {
		q.x = static_cast<float>(p.x - bb.x0) / Chunk::W;
		for (p.y = bb.y0; p.y < bb.y1; ++p.y) {
			q.y = static_cast<float>(p.y - bb.y0) / Chunk::D;
			terraform_column(Biome(b, q), p - p0, c);
		}
	}
	return 0;
}

