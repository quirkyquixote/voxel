
#ifndef VOXEL_TONE_MAPPER_H_
#define VOXEL_TONE_MAPPER_H_

#define GL_GLEXT_PROTOTYPES 1
#include "SDL2/SDL_opengl.h"
#include "GL/glext.h"
#undef GL_GLEXT_PROTOTYPES

#include "v3.h"
#include "v2.h"

class ToneMapper {
public:
	ToneMapper(float dt, size_t samples);
	~ToneMapper();

	void update(float sky, float spot);
	inline GLuint get_texture() const { return texture; }

private:
	float k;               /* luminosity adjustment in one step */
	float luminance;       /* current luminance value */
	float time;            /* current time */

	size_t samples;          /* number of samples in mapping texture */
	float max_intensity;     /* maximum intensity of light */
	float gamma;             /* gamma correction */
	float a;                 /* constant to map colors */
	float b;                 /* constant to map colors */
	float dt;                /* duration of step */

	v3f sky_color;
	v3f spot_color_0;
	v3f spot_color_1;

	v3f *mapped_sky_color;
	v3f *mapped_spot_color;
	v3f *mapped_color;

	float sky_brightness;
	float spot_brightness;

	GLuint texture;
};

static inline v2f texcoord_from_light(int l)
{
	return v2f((l & 0xf) / 15., (l >> 4) / 15.);
}

#endif
