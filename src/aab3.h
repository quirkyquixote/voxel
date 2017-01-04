
#ifndef VOXEL_AAB3_H_
#define VOXEL_AAB3_H_

struct aab3f {
	float x0, y0, z0, x1, y1, z1;
};

struct aab3d {
	double x0, y0, z0, x1, y1, z1;
};

struct aab3c {
	char x0, y0, z0, x1, y1, z1;
};

struct aab3s {
	short x0, y0, z0, x1, y1, z1;
};

struct aab3l {
	long x0, y0, z0, x1, y1, z1;
};

struct aab3ll {
	long long x0, y0, z0, x1, y1, z1;
};

static inline struct aab3f aab3f(float x0, float y0, float z0, float x1, float y1, float z1)
{
	return (struct aab3f){ x0, y0, z0, x1, y1, z1 };
}

static inline struct aab3d aab3d(double x0, double y0, double z0, double x1, double y1, double z1)
{
	return (struct aab3d){ x0, y0, z0, x1, y1, z1 };
}

static inline struct aab3c aab3c(char x0, char y0, char z0, char x1, char y1, char z1)
{
	return (struct aab3c){ x0, y0, z0, x1, y1, z1 };
}

static inline struct aab3s aab3s(short x0, short y0, short z0, short x1, short y1, short z1)
{
	return (struct aab3s){ x0, y0, z0, x1, y1, z1 };
}

static inline struct aab3l aab3l(long x0, long y0, long z0, long x1, long y1, long z1)
{
	return (struct aab3l){ x0, y0, z0, x1, y1, z1 };
}

static inline struct aab3ll aab3ll(long long x0, long long y0, long long z0, long long x1, long long y1, long long z1)
{
	return (struct aab3ll){ x0, y0, z0, x1, y1, z1 };
}

#define aab3_overlap(b1, b2) \
	(b2.x0 <= b1.x1 && b2.x1 >= b1.x0 &&\
	 b2.y0 <= b1.y1 && b2.y1 >= b1.y0 &&\
	 b2.z0 <= b1.z1 && b2.z1 >= b1.z0)

#endif
