
#ifndef VOXEL_AAB2_H_
#define VOXEL_AAB2_H_

struct box2f {
	float x0, y0, x1, y1;
};

struct box2d {
	double x0, y0, x1, y1;
};

struct box2c {
	char x0, y0, x1, y1;
};

struct box2s {
	short x0, y0, x1, y1;
};

struct box2l {
	long x0, y0, x1, y1;
};

struct box2ll {
	long long x0, y0, x1, y1;
};

static inline struct box2f box2f(float x0, float y0, float x1, float y1)
{
	return (struct box2f){ x0, y0, x1, y1};
}

static inline struct box2d box2d(double x0, double y0, double x1, double y1)
{
	return (struct box2d){ x0, y0, x1, y1};
}

static inline struct box2c box2c(char x0, char y0, char x1, char y1)
{
	return (struct box2c){ x0, y0, x1, y1};
}

static inline struct box2s box2s(short x0, short y0, short x1, short y1)
{
	return (struct box2s){ x0, y0, x1, y1};
}

static inline struct box2l box2l(long x0, long y0, long x1, long y1)
{
	return (struct box2l){ x0, y0, x1, y1};
}

static inline struct box2ll box2ll(long long x0, long long y0, long long x1, long long y1)
{
	return (struct box2ll){ x0, y0, x1, y1};
}

#define box2_overlap(b1, b2) \
	(b2.x0 <= b1.x1 && b2.x1 >= b1.x0 &&  b2.y0 <= b1.y1 && b2.y1 >= b1.y0)

#define box2_foreach(_iter,_bbox) \
	for (_iter.x = _bbox.x0; _iter.x < _bbox.x1; ++_iter.x) \
	for (_iter.y = _bbox.y0; _iter.y < _bbox.y1; ++_iter.y)

#endif
