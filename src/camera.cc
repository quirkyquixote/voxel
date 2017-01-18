

#include "camera.h"

#include <stdlib.h>

#define GL_GLEXT_PROTOTYPES 1
#include "SDL2/SDL_opengl.h"
#include "GL/glext.h"
#include "GL/glu.h"
#undef GL_GLEXT_PROTOTYPES

static const GLfloat bkgrcolor[] = { .5, .75, 1, 1 };

Camera::Camera()
{
}

Camera::~Camera()
{
}

bool Camera::is_visible(const v3f &x, float r)
{
	v3f q = x - p;
	return (length(q) + r <= max_distance
			&& dot(nup, q) < r && dot(ndn, q) < r
			&& dot(nlf, q) < r && dot(nrt, q) < r);
}

void Camera::load_gl_matrices()
{
	float b = fovy * 0.5 * M_PI / 180.;
	float cb = cos(b);
	float sb = sin(b);

	float a = asin(sb * aspect_ratio);
	float ca = cos(a);
	float sa = sin(a);

	v3f target;

	nlf = v3f(-ca, 0, sa);
	nlf = rotx(nlf, r.x);
	nlf = roty(nlf, r.y);

	nrt = v3f(ca, 0, sa);
	nrt = rotx(nrt, r.x);
	nrt = roty(nrt, r.y);

	nup = v3f(0, cb, sb);
	nup = rotx(nup, r.x);
	nup = roty(nup, r.y);

	ndn = v3f(0, -cb, sb);
	ndn = rotx(ndn, r.x);
	ndn = roty(ndn, r.y);

	target = v3f(0, 0, -max_distance);
	target = rotx(target, r.x);
	target = roty(target, r.y);
	target = target + p;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(p.x, p.y, p.z, target.x, target.y, target.z, 0, 1, 0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, aspect_ratio, .1, max_distance);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glClearColor(bkgrcolor[0], bkgrcolor[1], bkgrcolor[2], bkgrcolor[3]);
	/*
	   glFogi(GL_FOG_MODE, GL_LINEAR);
	   glFogfv(GL_FOG_COLOR, bkgrcolor);
	   glHint(GL_FOG_HINT, GL_DONT_CARE);
	   glFogf(GL_FOG_START, distance / 2);
	   glFogf(GL_FOG_END, distance);
	   glEnable(GL_FOG);
	   */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

