
#ifndef VOXEL_AAB3F_H_
#define VOXEL_AAB3F_H_

struct aab3f {
	float x0, y0, z0;
	float x1, y1, z1;
};

static inline struct aab3f aab3f(float x0, float y0, float z0, float x1, float y1, float z1)
{
	return (struct aab3f){ x0, y0, z0, x1, y1, z1 };
}

static inline int aab3f_overlap(struct aab3f *b1, struct aab3f *b2)
{
	return b2->x0 <= b1->x1 && b2->x1 >= b1->x0 &&
		b2->y0 <= b1->y1 && b2->y1 >= b1->y0 &&
		b2->z0 <= b1->z1 && b2->z1 >= b1->z0;
}

#endif
