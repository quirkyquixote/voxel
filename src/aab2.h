
#ifndef VOXEL_AAB2_H_
#define VOXEL_AAB2_H_

struct aab2f {
	float x0, y0, x1, y1
};

struct aab2d {
	double x0, y0, x1, y1
};

struct aab2c {
	char x0, y0, x1, y1
};

struct aab2s {
	short x0, y0, x1, y1
};

struct aab2l {
	long x0, y0, x1, y1
};

struct aab2ll {
	long long x0, y0, x1, y1
};

static inline struct aab2f aab2f(float x0, float y0, float x1, float y1)
{
	return (struct aab2f){ x0, y0, x1, y1};
}

static inline struct aab2d aab2d(double x0, double y0, double x1, double y1)
{
	return (struct aab2d){ x0, y0, x1, y1};
}

static inline struct aab2c aab2c(char x0, char y0, char x1, char y1)
{
	return (struct aab2c){ x0, y0, x1, y1};
}

static inline struct aab2s aab2s(short x0, short y0, short x1, short y1)
{
	return (struct aab2s){ x0, y0, x1, y1};
}

static inline struct aab2l aab2l(long x0, long y0, long x1, long y1)
{
	return (struct aab2l){ x0, y0, x1, y1};
}

static inline struct aab2ll aab2ll(long long x0, long long y0, long long x1, long long y1)
{
	return (struct aab2ll){ x0, y0, x1, y1};
}

#define aab2_overlap(b1, b2) \
	(b2.x0 <= b1.x1 && b2.x1 >= b1.x0 &&  b2.y0 <= b1.y1 && b2.y1 >= b1.y0)

#endif
