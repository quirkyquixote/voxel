/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "terraform.h"

#include <libnoise/noise.h>
#include "noiseutils.h"

#include "box2.h"

void terraform(int64_t seed, Chunk *c)
{
	v2ll p0 = c->get_p();
	box2f bb(p0.x, p0.y, p0.x + Chunk::W, p0.y + Chunk::D);
	bb /= 1000.f;

	noise::module::Perlin myModule;
	noise::utils::NoiseMap heightMap;
	noise::utils::NoiseMapBuilderPlane heightMapBuilder;
	heightMapBuilder.SetSourceModule (myModule);
	heightMapBuilder.SetDestNoiseMap (heightMap);
	heightMapBuilder.SetDestSize (16, 16);
	heightMapBuilder.SetBounds (bb.x0, bb.x1, bb.y0, bb.y1);
	heightMapBuilder.Build ();

	for (auto p : box2ll(0, 0, 15, 15)) {
		v3ll q(p.x, 0, p.y);
		float f = heightMap.GetValue(p.x, p.y);
		log_info("%lld,%lld = %g", p.x, p.y, f);
		int h = 128 + f * 128;
		while(q.y < h) {
			c->set_mat(q, MAT_LIMESTONE);
			c->set_shape(q, SHAPE_BLOCK_DN);
			++q.y;
		}
		while (q.y < Chunk::H) {
			c->set_mat(q, 0);
			c->set_shape(q, SHAPE_NONE);
			++q.y;
		}
	}
}

