
#ifndef VOXEL_TONE_MAPPER_H_
#define VOXEL_TONE_MAPPER_H_

#include <SDL2/SDL_opengl.h>

#include "v3.h"

struct tone_mapper {
	float k;               /* luminosity adjustment in one step */
	float luminance;       /* current luminance value */
	float time;            /* current time */

	size_t samples;          /* number of samples in mapping texture */
	float max_intensity;     /* maximum intensity of light */
	float gamma;             /* gamma correction */
	float a;                 /* constant to map colors */
	float b;                 /* constant to map colors */
	float dt;                /* duration of step */

	struct v3f sky_color;
	struct v3f spot_color_0;
	struct v3f spot_color_1;

	struct v3f *mapped_sky_color;
	struct v3f *mapped_spot_color;
	struct v3f *mapped_color;

	float sky_brightness;
	float spot_brightness;

	GLuint texture;
};

struct tone_mapper *tone_mapper(float dt, size_t samples);
void tone_mapper_destroy(struct tone_mapper *t);
void tone_mapper_update(struct tone_mapper *t, float sky, float spot);

#endif
