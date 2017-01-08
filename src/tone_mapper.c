
#include "tone_mapper.h"

#include <stdlib.h>

struct tone_mapper *tone_mapper(float dt, size_t samples)
{
	struct tone_mapper *t = calloc(1, sizeof(*t));
	t->luminance = 1;
	t->samples = samples;
	t->max_intensity = 1;
	t->gamma = .66;
	t->a = (.5 / samples);
	t->b = (1 - 2 * t->a) / t->max_intensity;
	t->sky_color = v3f(1, 1, 1);
	t->spot_color_0 = v3f(.94, .63, 0);
	t->spot_color_1 = v3f(.31, .16, 0);

	float a = 4;
	float pow_a_dt = pow(a, dt);
	float pow_a_minus_one_dt = pow(a - 1, dt);
	t->k = (pow_a_dt - pow_a_minus_one_dt) / pow_a_dt;

	t->mapped_sky_color = calloc(samples, sizeof(*t->mapped_sky_color));
	t->mapped_spot_color = calloc(samples, sizeof(*t->mapped_spot_color));
	t->mapped_color = calloc(samples * samples, sizeof(*t->mapped_color));

	glGenTextures(1, &t->texture);
	glBindTexture(GL_TEXTURE_2D, t->texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t->samples, t->samples, 0, GL_RGB,
			GL_FLOAT, t->mapped_color);
	return t;
}

void tone_mapper_destroy(struct tone_mapper *t)
{
	glDeleteTextures(1, &t->texture);
	free(t->mapped_sky_color);
	free(t->mapped_spot_color);
	free(t->mapped_color);
}

void tone_mapper_update(struct tone_mapper *t, float sky, float spot)
{
	// Calculate the mean of light in non-opaque cells around the player item.

	t->time += t->dt;
	struct v3f sky_color = t->sky_color;
	struct v3f spot_color = v3_addx(t->spot_color_0, t->spot_color_1, 1 + sinf(t->time * 5));

	t->sky_brightness = (sky_color.x + sky_color.y + sky_color.z) / 3;
	t->spot_brightness = (spot_color.x + spot_color.y + spot_color.z) / 3;

	float k = 0;

	t->luminance += (sky * t->sky_brightness + spot * t->spot_brightness - t->luminance) * t->k;

	// regenerate textures

	float A = .25 / pow(t->luminance, t->gamma) + .75;
	float scale = t->max_intensity / t->samples;

	sky_color = v3_mul(sky_color, scale);
	spot_color = v3_mul(spot_color, scale);

	for (int i = 0; i < t->samples; ++i) {
		t->mapped_sky_color[i] = v3_mul(sky_color, i);
		t->mapped_spot_color[i] = v3_mul(spot_color, i);
	}

	for (int j = 0, k = 0; j < t->samples; ++j) {
		for (int i = 0; i < t->samples; ++i, ++k) {
			t->mapped_color[k].x =
				A * powf(t->mapped_sky_color[i].x +
						t->mapped_spot_color[j].x,
						t->gamma);
			t->mapped_color[k].y =
				A * powf(t->mapped_sky_color[i].y +
						t->mapped_spot_color[j].y,
						t->gamma);
			t->mapped_color[k].z =
				A * powf(t->mapped_sky_color[i].z +
						t->mapped_spot_color[j].z,
						t->gamma);
		}
	}

	glBindTexture(GL_TEXTURE_2D, t->texture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, t->samples, t->samples, GL_RGB,
			GL_FLOAT, t->mapped_color);
}

