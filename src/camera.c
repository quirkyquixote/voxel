

#include "camera.h"

#include <stdlib.h>

#define GL_GLEXT_PROTOTYPES 1
#include "SDL2/SDL_opengl.h"
#include "GL/glext.h"
#include "GL/glu.h"
#undef GL_GLEXT_PROTOTYPES

static const GLfloat bkgrcolor[] = { .5, .75, 1, 1 };

struct camera *camera(void)
{
	struct camera *c = calloc(1, sizeof(*c));
	return c;
}

void camera_destroy(struct camera *c)
{
	free(c);
}

int camera_visible(struct camera *c, struct v3f p, float r)
{
	p = v3_sub(p, c->p);
	return (v3_length(p) + r <= c->distance
			&& v3_dot(c->nup, p) < r && v3_dot(c->ndn, p) < r
			&& v3_dot(c->nlf, p) < r && v3_dot(c->nrt, p) < r);
}

void camera_load_gl_matrices(struct camera *c)
{
	float b = c->fovy * 0.5 * M_PI / 180.;
	float cb = cos(b);
	float sb = sin(b);

	float a = asin(sb * c->ratio);
	float ca = cos(a);
	float sa = sin(a);

	struct v3f target;

	c->nlf = v3f(-ca, 0, sa);
	c->nlf = v3_rotx(c->nlf, c->r.x);
	c->nlf = v3_roty(c->nlf, c->r.y);

	c->nrt = v3f(ca, 0, sa);
	c->nrt = v3_rotx(c->nrt, c->r.x);
	c->nrt = v3_roty(c->nrt, c->r.y);

	c->nup = v3f(0, cb, sb);
	c->nup = v3_rotx(c->nup, c->r.x);
	c->nup = v3_roty(c->nup, c->r.y);

	c->ndn = v3f(0, -cb, sb);
	c->ndn = v3_rotx(c->ndn, c->r.x);
	c->ndn = v3_roty(c->ndn, c->r.y);

	target = v3f(0, 0, -c->distance);
	target = v3_rotx(target, c->r.x);
	target = v3_roty(target, c->r.y);
	target = v3_add(target, c->p);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(c->p.x, c->p.y, c->p.z,
			target.x, target.y, target.z, 0, 1, 0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(c->fovy, c->ratio, .1, c->distance);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glClearColor(bkgrcolor[0], bkgrcolor[1], bkgrcolor[2], bkgrcolor[3]);
	/*
	   glFogi(GL_FOG_MODE, GL_LINEAR);
	   glFogfv(GL_FOG_COLOR, bkgrcolor);
	   glHint(GL_FOG_HINT, GL_DONT_CARE);
	   glFogf(GL_FOG_START, c->distance / 2);
	   glFogf(GL_FOG_END, c->distance);
	   glEnable(GL_FOG);
	   */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

