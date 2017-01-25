/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "terraform.h"

#include <limits.h>

#include <libnoise/noise.h>
#include "noiseutils.h"

#include "box2.h"

void terraform(int64_t seed, Chunk *c)
{
	v2ll p0 = c->get_p();
	box2f bb(0, 0, Chunk::W, Chunk::D);
	bb += v2f(p0);
	bb /= 500.f;

	noise::module::RidgedMulti baseMountainTerrain;
	noise::module::ScaleBias mountainTerrain;
	mountainTerrain.SetSourceModule (0, baseMountainTerrain);
	mountainTerrain.SetScale (0.5);
	mountainTerrain.SetBias (0.5);

	noise::module::Billow baseFlatTerrain;
	baseFlatTerrain.SetFrequency (2.0);
	noise::module::ScaleBias flatTerrain;
	flatTerrain.SetSourceModule (0, baseFlatTerrain);
	flatTerrain.SetScale (0.125);
	flatTerrain.SetBias (0.125);

	noise::module::Perlin terrainType;
	terrainType.SetFrequency (0.5);
	terrainType.SetPersistence (0.25);

	noise::module::Select finalTerrain;
	finalTerrain.SetSourceModule (0, flatTerrain);
	finalTerrain.SetSourceModule (1, mountainTerrain);
	finalTerrain.SetControlModule (terrainType);
	finalTerrain.SetBounds (0.0, 1000.0);
	finalTerrain.SetEdgeFalloff (0.125);

	noise::utils::NoiseMap heightMap;
	noise::utils::NoiseMapBuilderPlane heightMapBuilder;
	heightMapBuilder.SetSourceModule (finalTerrain);
	heightMapBuilder.SetDestNoiseMap (heightMap);
	heightMapBuilder.SetDestSize (16, 16);
	heightMapBuilder.SetBounds (bb.x0, bb.x1, bb.y0, bb.y1);
	heightMapBuilder.Build ();

	for (auto p : box2ll(0, 0, 15, 15)) {
		v3ll q(p.x, 0, p.y);
		float f = heightMap.GetValue(p.x, p.y);
		int h = 128 + f * 64;
		int mat;
		if (f < .1)
			mat = MAT_SANDSTONE_SAND;
		if (f < .5)
			mat = MAT_GRASS;
		else if (f < .6)
			mat = MAT_LIMESTONE_SAND;
		else if (f < .75)
			mat = MAT_LIMESTONE;
		else
			mat = MAT_MARBLE;
	//	log_info("%lld,%lld = %d (%s)", p.x + p0.x, p.y + p0.y, h, mat_names[mat]);
		while(q.y < h) {
			c->set_mat(q, mat);
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

