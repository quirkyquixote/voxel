

#ifndef VOXEL_CAMERA_H_
#define VOXEL_CAMERA_H_

#include "v3f.h"

struct camera {
	struct v3f p;		// position of the camera
	struct v3f r;		// euler angles for the camera
	float fovy;
	float distance;		// shards farther than this will not be drawn. Also determines fog
	float ratio;
	struct v3f nlf;		// normal of the left clipping plane
	struct v3f nrt;		// normal of the right clipping plane
	struct v3f nup;		// normal of the top clipping plane
	struct v3f ndn;		// normal of the bottom clipping plane
};

struct camera *camera(void);
void camera_destroy(struct camera *c);
void camera_load_gl_matrices(struct camera *c);
int camera_visible(struct camera *c, struct v3f p, float r);

static inline void camera_set_position(struct camera *c, struct v3f p)
{
	c->p = p;
}

static inline void camera_set_rotation(struct camera *c, struct v3f r)
{
	c->r = r;
}

static inline void camera_set_fovy(struct camera *c, float fovy)
{
	c->fovy = fovy;
}

static inline void camera_set_max_distance(struct camera *c, float d)
{
	c->distance = d;
}

static inline void camera_set_aspect_ratio(struct camera *c, float r)
{
	c->ratio = r;
}

#endif
