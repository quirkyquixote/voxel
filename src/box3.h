
#ifndef VOXEL_AAB3_H_
#define VOXEL_AAB3_H_

struct box3f {
	float x0, y0, z0, x1, y1, z1;
};

struct box3d {
	double x0, y0, z0, x1, y1, z1;
};

struct box3c {
	char x0, y0, z0, x1, y1, z1;
};

struct box3s {
	short x0, y0, z0, x1, y1, z1;
};

struct box3l {
	long x0, y0, z0, x1, y1, z1;
};

struct box3ll {
	long long x0, y0, z0, x1, y1, z1;
};

static inline struct box3f box3f(float x0, float y0, float z0, float x1, float y1, float z1)
{
	return (struct box3f){ x0, y0, z0, x1, y1, z1 };
}

static inline struct box3d box3d(double x0, double y0, double z0, double x1, double y1, double z1)
{
	return (struct box3d){ x0, y0, z0, x1, y1, z1 };
}

static inline struct box3c box3c(char x0, char y0, char z0, char x1, char y1, char z1)
{
	return (struct box3c){ x0, y0, z0, x1, y1, z1 };
}

static inline struct box3s box3s(short x0, short y0, short z0, short x1, short y1, short z1)
{
	return (struct box3s){ x0, y0, z0, x1, y1, z1 };
}

static inline struct box3l box3l(long x0, long y0, long z0, long x1, long y1, long z1)
{
	return (struct box3l){ x0, y0, z0, x1, y1, z1 };
}

static inline struct box3ll box3ll(long long x0, long long y0, long long z0, long long x1, long long y1, long long z1)
{
	return (struct box3ll){ x0, y0, z0, x1, y1, z1 };
}

#define box3_fix(bb) (typeof(bb)){ \
	bb.x0 < bb.x1 ? bb.x0 : bb.x1, \
	bb.y0 < bb.y1 ? bb.y0 : bb.y1, \
	bb.z0 < bb.z1 ? bb.z0 : bb.z1, \
	bb.x0 > bb.x1 ? bb.x0 : bb.x1, \
	bb.y0 > bb.y1 ? bb.y0 : bb.y1, \
	bb.z0 > bb.z1 ? bb.z0 : bb.z1 }

#define box3_grow(b,k) (typeof(b)){ b.x0 - k, b.y0 - k, b.z0 - k, b.x0 + k, b.y0 + k, b.z0 + k }

#define box3_overlap(b1, b2) \
	(b2.x0 <= b1.x1 && b2.x1 >= b1.x0 &&\
	 b2.y0 <= b1.y1 && b2.y1 >= b1.y0 &&\
	 b2.z0 <= b1.z1 && b2.z1 >= b1.z0)

#define box3_foreach(_iter,_bbox) \
	for (_iter.x = _bbox.x0; _iter.x < _bbox.x1; ++_iter.x) \
	for (_iter.y = _bbox.y0; _iter.y < _bbox.y1; ++_iter.y) \
	for (_iter.z = _bbox.z0; _iter.z < _bbox.z1; ++_iter.z)

#endif
