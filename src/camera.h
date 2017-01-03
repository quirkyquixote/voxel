

#ifndef VOXEL_CAMERA_H_
#define VOXEL_CAMERA_H_

#include "v3.h"

struct camera {
    struct v3f pos;		// position of the camera
    struct v3f angles;		// euler angles for the camera
    struct v3f nlf;		// normal of the left clipping plane
    struct v3f nrt;		// normal of the right clipping plane
    struct v3f nup;		// normal of the top clipping plane
    struct v3f ndn;		// normal of the bottom clipping plane
    float fovy;
    float distance;		// shards farther than this will not be drawn. Also determines fog
};

struct camera *camera(float fovy, float distance);
void camera_destroy(struct camera *c);
void camera_update(struct camera *c, float w, float h);
int camera_visible(struct camera *c, struct v3f p, float r);

#endif
