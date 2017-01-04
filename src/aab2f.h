
#ifndef VOXEL_AAB2F_H_
#define VOXEL_AAB2F_H_

struct aab2f {
	float x0, y0;
	float x1, y1;
};

static inline struct aab2f aab2f(float x0, float y0, float x1, float y1)
{
	return (struct aab2f){ x0, y0, x1, y1 };
}

static inline int aab2f_overlap(struct aab2f *b1, struct aab2f *b2)
{
	return b2->x0 <= b1->x1 && b2->x1 >= b1->x0 &&
		b2->y0 <= b1->y1 && b2->y1 >= b1->y0;
}

#endif
