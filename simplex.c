
#include <math.h>
#include "simplex.h"

/*----------------------------------------------------------------------------
 * Permutation table. 
 * Repeated twice to remove the need for index wrapping.
 *----------------------------------------------------------------------------*/
static const int PERM[] = {
    151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225,
    140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148,
    247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11,
    32, 57, 177, 33, 88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168,
    68, 175, 74, 165, 71, 134, 139, 48, 27, 166, 77, 146, 158, 231, 83,
    111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245,
    40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76,
    132, 187, 208, 89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86,
    164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123,
    5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47,
    16, 58, 17, 182, 189, 28, 42, 223, 183, 170, 213, 119, 248, 152, 2,
    44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9, 129, 22, 39,
    253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218,
    246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162,
    241, 81, 51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31, 181,
    199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150,
    254, 138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128,
    195, 78, 66, 215, 61, 156, 180,

    151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225,
    140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148,
    247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11,
    32, 57, 177, 33, 88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168,
    68, 175, 74, 165, 71, 134, 139, 48, 27, 166, 77, 146, 158, 231, 83,
    111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245,
    40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76,
    132, 187, 208, 89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86,
    164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123,
    5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47,
    16, 58, 17, 182, 189, 28, 42, 223, 183, 170, 213, 119, 248, 152, 2,
    44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9, 129, 22, 39,
    253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218,
    246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162,
    241, 81, 51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31, 181,
    199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150,
    254, 138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128,
    195, 78, 66, 215, 61, 156, 180,
};

/*----------------------------------------------------------------------------
 * Gradient in 3D. Used for 2D too.
 *----------------------------------------------------------------------------*/
static const int GRAD3[][3] = {
    {1, 1, 0}, {-1, 1, 0}, {1, -1, 0}, {-1, -1, 0},
    {1, 0, 1}, {-1, 0, 1}, {1, 0, -1}, {-1, 0, -1},
    {0, 1, 1}, {0, -1, 1}, {0, 1, -1}, {0, -1, -1}
};

/*----------------------------------------------------------------------------
 * Gradient in 4D.
 *----------------------------------------------------------------------------*/
static const int GRAD4[][4] = {
    {0, 1, 1, 1}, {0, 1, 1, -1}, {0, 1, -1, 1}, {0, 1, -1, -1},
    {0, -1, 1, 1}, {0, -1, 1, -1}, {0, -1, -1, 1}, {0, -1, -1, -1},
    {1, 0, 1, 1}, {1, 0, 1, -1}, {1, 0, -1, 1}, {1, 0, -1, -1},
    {-1, 0, 1, 1}, {-1, 0, 1, -1}, {-1, 0, -1, 1}, {-1, 0, -1, -1},
    {1, 1, 0, 1}, {1, 1, 0, -1}, {1, -1, 0, 1}, {1, -1, 0, -1},
    {-1, 1, 0, 1}, {-1, 1, 0, -1}, {-1, -1, 0, 1}, {-1, -1, 0, -1},
    {1, 1, 1, 0}, {1, 1, -1, 0}, {1, -1, 1, 0}, {1, -1, -1, 0},
    {-1, 1, 1, 0}, {-1, 1, -1, 0}, {-1, -1, 1, 0}, {-1, -1, -1, 0}
};

/*----------------------------------------------------------------------------
 * 2D dot product.
 *----------------------------------------------------------------------------*/
static inline double
dot2(const int g[], double x, double y)
{
    return g[0] * x + g[1] * y;
}

/*----------------------------------------------------------------------------
 * 3D dot product.
 *----------------------------------------------------------------------------*/
static inline double
dot3(const int g[], double x, double y, double z)
{
    return g[0] * x + g[1] * y + g[2] * z;
}

/*----------------------------------------------------------------------------
 * 4D dot product.
 *----------------------------------------------------------------------------*/
static inline double
dot4(const int g[], double x, double y, double z, double w)
{
    return g[0] * x + g[1] * y + g[2] * z + g[3] * w;
}

/*----------------------------------------------------------------------------
 * 2D simplex noise.
 *----------------------------------------------------------------------------*/
double
noise_simplex_2d(double x, double y)
{
    // Skewing and unskewing factors. They are... complicated.
    //   skew = 0.5 * (sqrt (3.0) - 1.0); 
    //   unskew = (3.0 - sqrt (3.0)) / 6.0;

    static const double skew = 3.660254037844386e-01;
    static const double unskew = 2.1132486540519e-01;

    // Noise contributions from the three corners

    double n0, n1, n2;

    // Skew the input space to determine which simplex cell we're in

    double s = (x + y) * skew;	// Hairy factor for 2D
    int i = floorl(x + s);
    int j = floorl(y + s);
    double t = (i + j) * unskew;

    double cell_x = i - t;	// Unskew the cell origin back to (x,y) space
    double cell_y = j - t;

    double x0 = x - cell_x;	// The x,y distances from the cell origin
    double y0 = y - cell_y;

    // For the 2D case, the simplex shape is an equilateral triangle and there
    // are two per cell. Determine which one we are in.

    int i1, j1;			// Offsets for second (middle) corner of simplex in (i,j) coords

    if (x0 > y0) {		// lower triangle, XY order: (0,0)->(1,0)->(1,1)
	i1 = 1;
	j1 = 0;
    } else {			// upper triangle, YX order: (0,0)->(0,1)->(1,1)
	i1 = 0;
	j1 = 1;
    }

    // A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
    // a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
    // c = (3-sqrt(3))/6

    double x1 = x0 - i1 + unskew;	// Offsets for middle corner in (x,y) unskewed coords
    double y1 = y0 - j1 + unskew;

    double x2 = x0 - 1.0 + 2.0 * unskew;	// Offsets for last corner in (x,y) unskewed coords
    double y2 = y0 - 1.0 + 2.0 * unskew;

    // Work out the hashed gradient indices of the three simplex corners

    int ii = i & 255;
    int jj = j & 255;
    int gi0 = PERM[ii + PERM[jj]] % 12;
    int gi1 = PERM[ii + i1 + PERM[jj + j1]] % 12;
    int gi2 = PERM[ii + 1 + PERM[jj + 1]] % 12;

    // Calculate the contribution from the three corners

    double t0 = 0.5 - x0 * x0 - y0 * y0;
    if (t0 < 0) {
	n0 = 0.0;
    } else {
	t0 *= t0;
	n0 = t0 * t0 * dot2(GRAD3[gi0], x0, y0);	// (x,y) of GRAD3 used for 2D gradient
    }

    double t1 = 0.5 - x1 * x1 - y1 * y1;
    if (t1 < 0) {
	n1 = 0.0;
    } else {
	t1 *= t1;
	n1 = t1 * t1 * dot2(GRAD3[gi1], x1, y1);
    }

    double t2 = 0.5 - x2 * x2 - y2 * y2;
    if (t2 < 0) {
	n2 = 0.0;
    } else {
	t2 *= t2;
	n2 = t2 * t2 * dot2(GRAD3[gi2], x2, y2);
    }

    // Add contributions from each corner to get the noise value.
    // The result is scaled to return values in the interval [-1,1].

    return 70.0 * (n0 + n1 + n2);
}





/*----------------------------------------------------------------------------
 * 3D simplex noise
 *----------------------------------------------------------------------------*/
double
noise_simplex_3d(double x, double y, double z)
{
    // Skewing and unskewing factors.

    static const double skew = 1.0 / 3.0;	// Very nice and simple skew factor for 3D
    static const double unskew = 1.0 / 6.0;	// Very nice and simple unskew factor, too

    // Noise contributions from the four corners

    double n0, n1, n2, n3;

    // Skew the input space to determine which simplex cell we're in

    double s = (x + y + z) * skew;
    int i = floorl(x + s);
    int j = floorl(y + s);
    int k = floorl(z + s);
    double t = (i + j + k) * unskew;

    double cell_x = i - t;	// Unskew the cell origin back to (x,y,z) space
    double cell_y = j - t;
    double cell_z = k - t;

    double x0 = x - cell_x;	// The x,y,z distances from the cell origin
    double y0 = y - cell_y;
    double z0 = z - cell_z;

    // For the 3D case, the simplex shape is a slightly irregular tetrahedron and
    // there are four per cell. Determine which one we are in.

    int i1, j1, k1;		// Offsets for second corner of simplex in (i,j,k) coords
    int i2, j2, k2;		// Offsets for third corner of simplex in (i,j,k) coords

    if (x0 >= y0) {
	if (y0 >= z0) {		// X Y Z order
	    i1 = 1;
	    j1 = 0;
	    k1 = 0;
	    i2 = 1;
	    j2 = 1;
	    k2 = 0;
	} else if (x0 >= z0) {	// X Z Y order
	    i1 = 1;
	    j1 = 0;
	    k1 = 0;
	    i2 = 1;
	    j2 = 0;
	    k2 = 1;
	} else {		// Z X Y order
	    i1 = 0;
	    j1 = 0;
	    k1 = 1;
	    i2 = 1;
	    j2 = 0;
	    k2 = 1;
	}
    } else {
	if (y0 < z0) {		// Z Y X order
	    i1 = 0;
	    j1 = 0;
	    k1 = 1;
	    i2 = 0;
	    j2 = 1;
	    k2 = 1;
	} else if (x0 < z0) {	// Y Z X order
	    i1 = 0;
	    j1 = 1;
	    k1 = 0;
	    i2 = 0;
	    j2 = 1;
	    k2 = 1;
	} else {		// Y X Z order
	    i1 = 0;
	    j1 = 1;
	    k1 = 0;
	    i2 = 1;
	    j2 = 1;
	    k2 = 0;
	}
    }

    //  A step of (1,0,0) in (i,j,k) means a step of (1-c,-c,-c) in (x,y,z),
    //  a step of (0,1,0) in (i,j,k) means a step of (-c,1-c,-c) in (x,y,z), and
    //  a step of (0,0,1) in (i,j,k) means a step of (-c,-c,1-c) in (x,y,z), where
    //  c = 1/6.

    double x1 = x0 - i1 + unskew;	// Offsets for second corner in (x,y,z) coords
    double y1 = y0 - j1 + unskew;
    double z1 = z0 - k1 + unskew;

    double x2 = x0 - i2 + 2.0 * unskew;	// Offsets for third corner in (x,y,z) coords
    double y2 = y0 - j2 + 2.0 * unskew;
    double z2 = z0 - k2 + 2.0 * unskew;

    double x3 = x0 - 1.0 + 3.0 * unskew;	// Offsets for last corner in (x,y,z) coords
    double y3 = y0 - 1.0 + 3.0 * unskew;
    double z3 = z0 - 1.0 + 3.0 * unskew;

    // Work out the hashed gradient indices of the four simplex corners

    int ii = i & 255;
    int jj = j & 255;
    int kk = k & 255;
    int gi0 = PERM[ii + PERM[jj + PERM[kk]]] % 12;
    int gi1 = PERM[ii + i1 + PERM[jj + j1 + PERM[kk + k1]]] % 12;
    int gi2 = PERM[ii + i2 + PERM[jj + j2 + PERM[kk + k2]]] % 12;
    int gi3 = PERM[ii + 1 + PERM[jj + 1 + PERM[kk + 1]]] % 12;

    // Calculate the contribution from the four corners

    double t0 = 0.6 - x0 * x0 - y0 * y0 - z0 * z0;
    if (t0 < 0) {
	n0 = 0.0;
    } else {
	t0 *= t0;
	n0 = t0 * t0 * dot3(GRAD3[gi0], x0, y0, z0);
    }

    double t1 = 0.6 - x1 * x1 - y1 * y1 - z1 * z1;
    if (t1 < 0) {
	n1 = 0.0;
    } else {
	t1 *= t1;
	n1 = t1 * t1 * dot3(GRAD3[gi1], x1, y1, z1);
    }

    double t2 = 0.6 - x2 * x2 - y2 * y2 - z2 * z2;
    if (t2 < 0) {
	n2 = 0.0;
    } else {
	t2 *= t2;
	n2 = t2 * t2 * dot3(GRAD3[gi2], x2, y2, z2);
    }

    double t3 = 0.6 - x3 * x3 - y3 * y3 - z3 * z3;
    if (t3 < 0) {
	n3 = 0.0;
    } else {
	t3 *= t3;
	n3 = t3 * t3 * dot3(GRAD3[gi3], x3, y3, z3);
    }

    // Add contributions from each corner to get the noise value.
    // The result is scaled to stay just inside [-1,1]

    return 32.0 * (n0 + n1 + n2 + n3);
}




/*----------------------------------------------------------------------------
 * 4D simplex noise
 *----------------------------------------------------------------------------*/
double
noise_simplex_4d(double x, double y, double z, double w)
{
    // A lookup table to traverse the simplex around a given point in 4D.
    // Details can be found where this table is used, in the 4D noise method.

    static const int simplex[][4] = {
	{0, 1, 2, 3}, {0, 1, 3, 2}, {0, 0, 0, 0}, {0, 2, 3, 1}, {0, 0, 0, 0}, 
	{0, 0, 0, 0}, {0, 0, 0, 0}, {1, 2, 3, 0}, {0, 2, 1, 3}, {0, 0, 0, 0},
	{0, 3, 1, 2}, {0, 3, 2, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
	{1, 3, 2, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
	{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {1, 2, 0, 3},
	{0, 0, 0, 0}, {1, 3, 0, 2}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
	{2, 3, 0, 1}, {2, 3, 1, 0}, {1, 0, 2, 3}, {1, 0, 3, 2}, {0, 0, 0, 0},
	{0, 0, 0, 0}, {0, 0, 0, 0}, {2, 0, 3, 1}, {0, 0, 0, 0}, {2, 1, 3, 0},
	{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
	{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {2, 0, 1, 3}, {0, 0, 0, 0},
	{0, 0, 0, 0}, {0, 0, 0, 0}, {3, 0, 1, 2}, {3, 0, 2, 1}, {0, 0, 0, 0},
	{3, 1, 2, 0}, {2, 1, 0, 3}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
	{3, 1, 0, 2}, {0, 0, 0, 0}, {3, 2, 0, 1}, {3, 2, 1, 0}
    };

    // The skewing and unskewing factors are hairy again for the 4D case
    //   skew = (sqrt (5.0) - 1.0) / 4.0;
    //   unskew = (5.0 - sqrt (5.0)) / 20.0;

    static const double skew = 3.0901699437495e-01;
    static const double unskew = 1.3819660112501e-01;

    // Noise contributions from the five corners

    double n0, n1, n2, n3, n4;

    // Skew the (x,y,z,w) space to determine which cell of 24 simplices we're in

    double s = (x + y + z + w) * skew;
    int i = floorl(x + s);
    int j = floorl(y + s);
    int k = floorl(z + s);
    int l = floorl(w + s);
    double t = (i + j + k + l) * unskew;

    double cell_x = i - t;	// Unskew the cell origin back to (x,y,z,w) space
    double cell_y = j - t;
    double cell_z = k - t;
    double cell_w = l - t;

    double x0 = x - cell_x;	// The x,y,z,w distances from the cell origin
    double y0 = y - cell_y;
    double z0 = z - cell_z;
    double w0 = w - cell_w;

    // For the 4D case, the simplex is a 4D shape I won't even try to describe.
    // To find out which of the 24 possible simplices we're in, we need to
    // determine the magnitude ordering of x0, y0, z0 and w0.
    // The method below is a good way of finding the ordering of x,y,z,w and
    // then find the correct traversal order for the simplex we're in.
    // First, six pair-wise comparisons are performed between each possible pair
    // of the four coordinates, and the results are used to add up binary bits
    // for an integer index.

    int c1 = (x0 > y0) ? 32 : 0;
    int c2 = (x0 > z0) ? 16 : 0;
    int coord = (y0 > z0) ? 8 : 0;
    int c4 = (x0 > w0) ? 4 : 0;
    int c5 = (y0 > w0) ? 2 : 0;
    int c6 = (z0 > w0) ? 1 : 0;
    int c = c1 + c2 + coord + c4 + c5 + c6;

    int i1, j1, k1, l1;		// The integer offsets for the second simplex corner
    int i2, j2, k2, l2;		// The integer offsets for the third simplex corner
    int i3, j3, k3, l3;		// The integer offsets for the fourth simplex corner

    // simplex[c] is a 4-vector with the numbers 0, 1, 2 and 3 in some order.
    // Many values of c will never occur, since e.g. x>y>z>w makes x<z, y<w and x<w
    // impossible. Only the 24 indices which have non-zero entries make any sense.
    // We use a thresholding to set the coordinates in turn from the largest magnitude.
    // The number 3 in the "simplex" array is at the position of the largest coordinate.

    i1 = simplex[c][0] >= 3 ? 1 : 0;
    j1 = simplex[c][1] >= 3 ? 1 : 0;
    k1 = simplex[c][2] >= 3 ? 1 : 0;
    l1 = simplex[c][3] >= 3 ? 1 : 0;

    // The number 2 in the "simplex" array is at the second largest coordinate.

    i2 = simplex[c][0] >= 2 ? 1 : 0;
    j2 = simplex[c][1] >= 2 ? 1 : 0;
    k2 = simplex[c][2] >= 2 ? 1 : 0;
    l2 = simplex[c][3] >= 2 ? 1 : 0;

    // The number 1 in the "simplex" array is at the second smallest coordinate.

    i3 = simplex[c][0] >= 1 ? 1 : 0;
    j3 = simplex[c][1] >= 1 ? 1 : 0;
    k3 = simplex[c][2] >= 1 ? 1 : 0;
    l3 = simplex[c][3] >= 1 ? 1 : 0;

    // The fifth corner has all coordinate offsets = 1, so no need to look that up.

    double x1 = x0 - i1 + unskew;	// Offsets for second corner in (x,y,z,w) coords
    double y1 = y0 - j1 + unskew;
    double z1 = z0 - k1 + unskew;
    double w1 = w0 - l1 + unskew;

    double x2 = x0 - i2 + 2.0 * unskew;	// Offsets for third corner in (x,y,z,w) coords
    double y2 = y0 - j2 + 2.0 * unskew;
    double z2 = z0 - k2 + 2.0 * unskew;
    double w2 = w0 - l2 + 2.0 * unskew;

    double x3 = x0 - i3 + 3.0 * unskew;	// Offsets for fourth corner in (x,y,z,w) coords
    double y3 = y0 - j3 + 3.0 * unskew;
    double z3 = z0 - k3 + 3.0 * unskew;
    double w3 = w0 - l3 + 3.0 * unskew;

    double x4 = x0 - 1.0 + 4.0 * unskew;	// Offsets for last corner in (x,y,z,w) coords
    double y4 = y0 - 1.0 + 4.0 * unskew;
    double z4 = z0 - 1.0 + 4.0 * unskew;
    double w4 = w0 - 1.0 + 4.0 * unskew;

    // Work out the hashed gradient indices of the five simplex corners

    int ii = i & 255;
    int jj = j & 255;
    int kk = k & 255;
    int ll = l & 255;
    int gi0 = PERM[ii + PERM[jj + PERM[kk + PERM[ll]]]] % 32;
    int gi1 = PERM[ii + i1 + PERM[jj + j1 + PERM[kk + k1 + PERM[ll + l1]]]] % 32;
    int gi2 = PERM[ii + i2 + PERM[jj + j2 + PERM[kk + k2 + PERM[ll + l2]]]] % 32;
    int gi3 = PERM[ii + i3 + PERM[jj + j3 + PERM[kk + k3 + PERM[ll + l3]]]] % 32;
    int gi4 = PERM[ii + 1 + PERM[jj + 1 + PERM[kk + 1 + PERM[ll + 1]]]] % 32;

    // Calculate the contribution from the five corners

    double t0 = 0.6 - x0 * x0 - y0 * y0 - z0 * z0 - w0 * w0;

    if (t0 < 0) {
	n0 = 0.0;
    } else {
	t0 *= t0;
	n0 = t0 * t0 * dot4(GRAD4[gi0], x0, y0, z0, w0);
    }

    double t1 = 0.6 - x1 * x1 - y1 * y1 - z1 * z1 - w1 * w1;

    if (t1 < 0) {
	n1 = 0.0;
    } else {
	t1 *= t1;
	n1 = t1 * t1 * dot4(GRAD4[gi1], x1, y1, z1, w1);
    }

    double t2 = 0.6 - x2 * x2 - y2 * y2 - z2 * z2 - w2 * w2;

    if (t2 < 0) {
	n2 = 0.0;
    } else {
	t2 *= t2;
	n2 = t2 * t2 * dot4(GRAD4[gi2], x2, y2, z2, w2);
    }

    double t3 = 0.6 - x3 * x3 - y3 * y3 - z3 * z3 - w3 * w3;

    if (t3 < 0) {
	n3 = 0.0;
    } else {
	t3 *= t3;
	n3 = t3 * t3 * dot4(GRAD4[gi3], x3, y3, z3, w3);
    }

    double t4 = 0.6 - x4 * x4 - y4 * y4 - z4 * z4 - w4 * w4;

    if (t4 < 0) {
	n4 = 0.0;
    } else {
	t4 *= t4;
	n4 = t4 * t4 * dot4(GRAD4[gi4], x4, y4, z4, w4);
    }

    // Sum up and scale the result to cover the range [-1,1]

    return 27.0 * (n0 + n1 + n2 + n3 + n4);
}

