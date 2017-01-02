

#ifndef VOXEL_CAMERA_H_
#define VOXEL_CAMERA_H_

#include "vec.h"

struct camera {
    struct v3 pos;		// position of the camera
    struct v3 angles;		// euler angles for the camera
    struct v3 nlf;		// normal of the left clipping plane
    struct v3 nrt;		// normal of the right clipping plane
    struct v3 nup;		// normal of the top clipping plane
    struct v3 ndn;		// normal of the bottom clipping plane
    float distance;		// shards farther than this will not be drawn. Also determines fog
};

struct camera *camera(float distance);
void camera_destroy(struct camera *c);
void camera_update(struct camera *c, float w, float h);
int camera_visible(struct camera *c, struct v3 p, float r);

#endif
