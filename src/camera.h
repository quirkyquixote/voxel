/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */


#ifndef SRC_CAMERA_H_
#define SRC_CAMERA_H_

#define GL_GLEXT_PROTOTYPES 1
#include "SDL2/SDL_opengl.h"
#include "GL/glext.h"
#include "GL/glu.h"
#undef GL_GLEXT_PROTOTYPES

#include "v3.h"
#include "box3.h"

class Camera {
 public:
	Camera();
	~Camera();

	void load_gl_matrices();
	bool is_visible(const v3f &p);
	bool is_visible(const box3f &bb);

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
	float max_distance;     // shards farther than this will not be drawn
	float aspect_ratio;
	GLfloat modelview[16];  // modelview matrix
};

#endif  // SRC_CAMERA_H_
