

#include "camera.h"

#include <stdlib.h>

#define GL_GLEXT_PROTOTYPES 1
#include "SDL2/SDL_opengl.h"
#include "GL/glext.h"
#include "GL/glu.h"
#undef GL_GLEXT_PROTOTYPES

static const GLfloat bkgrcolor[] = { 1, .75, .5, 1 };

struct camera *camera(float distance)
{
	struct camera *c = calloc(1, sizeof(*c));
	c->distance = distance;
	return c;
}

void camera_destroy(struct camera *c)
{
	free(c);
}

int camera_visible(struct camera *c, struct v3 p, float r)
{
    p = v3sub(p, c->pos);
    return (v3length(p) + r <= c->distance
	    && v3dot(c->nup, p) < r && v3dot(c->ndn, p) < r
	    && v3dot(c->nlf, p) < r && v3dot(c->nrt, p) < r);
}

void camera_update(struct camera *c, float w, float h)
{
    static const float a = M_PI / 4;
    static const float b = M_PI / 6;

    float ca = cos(a);
    float sa = sin(a);
    float cb = cos(b);
    float sb = sin(b);

    float ratio = w / h;

    struct v3 target;

    c->nlf = v3(-ca, 0, sa);
    c->nlf = v3rotx(c->nlf, c->angles.x);
    c->nlf = v3roty(c->nlf, c->angles.y);

    c->nrt = v3(ca, 0, sa);
    c->nrt = v3rotx(c->nrt, c->angles.x);
    c->nrt = v3roty(c->nrt, c->angles.y);

    c->nup = v3(0, cb, sb);
    c->nup = v3rotx(c->nup, c->angles.x);
    c->nup = v3roty(c->nup, c->angles.y);

    c->ndn = v3(0, -cb, sb);
    c->ndn = v3rotx(c->ndn, c->angles.x);
    c->ndn = v3roty(c->ndn, c->angles.y);

    target = v3(0, 0, -c->distance);
    target = v3rotx(target, c->angles.x);
    target = v3roty(target, c->angles.y);
    target = v3add(target, c->pos);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(c->pos.x, c->pos.y, c->pos.z,
	      target.x, target.y, target.z, 0, 1, 0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f, ratio, .1, 300);

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

