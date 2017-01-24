/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "camera.h"

#include <stdlib.h>

#include "log.h"

static const GLfloat bkgrcolor[] = { .5, .75, 1, 1 };

Camera::Camera()
{
}

Camera::~Camera()
{
}

template<typename T> static inline v3<T> operator*(const T *m, const v3<T> &v)
{
	return v3<T>(m[0] * v.x + m[4] * v.y + m[8] * v.z + m[12],
			m[1] * v.x + m[5] * v.y + m[9] * v.z + m[13],
			m[2] * v.x + m[6] * v.y + m[10] * v.z + m[14]);
}


bool Camera::is_visible(const v3f &a)
{
	v3f b = modelview * a;
	return std::abs(b.x / b.z) < 1 && std::abs(b.y / b.z) < 1;
}

bool Camera::is_visible(const box3f &bb)
{
	return is_visible(v3f(bb.x0, bb.y0, bb.z0))
		|| is_visible(v3f(bb.x0, bb.y0, bb.z1))
		|| is_visible(v3f(bb.x0, bb.y1, bb.z0))
		|| is_visible(v3f(bb.x0, bb.y1, bb.z1))
		|| is_visible(v3f(bb.x1, bb.y0, bb.z0))
		|| is_visible(v3f(bb.x1, bb.y0, bb.z1))
		|| is_visible(v3f(bb.x1, bb.y1, bb.z0))
		|| is_visible(v3f(bb.x1, bb.y1, bb.z1));
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
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

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

