

#ifndef VOXEL_CAMERA_H_
#define VOXEL_CAMERA_H_

#include "v3.h"

class Camera {
public:
	Camera();
	~Camera();

	void load_gl_matrices();
	bool is_visible(const v3f &p, float r);

	inline const v3f &get_p() { return p; }
	inline const v3f &get_r() { return r; }
	inline float get_fovy() { return fovy; }
	inline float get_max_distance() { return max_distance; }
	inline float get_aspect_ratio() { return aspect_ratio; }

	inline void set_p(const v3f &new_p) { p = new_p; }
	inline void set_r(const v3f &new_r) { r = new_r; }
	inline void set_fovy(float f) { fovy = f; }
	inline void set_max_distance(float f) { max_distance = f; }
	inline void set_aspect_ratio(float f) { aspect_ratio = f; }

private:
	v3f p;		// position of the camera
	v3f r;		// euler angles for the camera
	float fovy;
	float max_distance;		// shards farther than this will not be drawn. Also determines fog
	float aspect_ratio;
	v3f nlf;		// normal of the left clipping plane
	v3f nrt;		// normal of the right clipping plane
	v3f nup;		// normal of the top clipping plane
	v3f ndn;		// normal of the bottom clipping plane
};

#endif
