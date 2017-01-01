/*
 * Simplex noise generator
 *
 * Written by Luis Sanz
 * Based in the code from 'Simplex noise demystified' from Stefan Gustavson
 */

#ifndef VX_SIMPLEX_H_
#define VX_SIMPLEX_H_

/* Simplex 2D noise */
double noise_simplex_2d(double x, double y);

/* Simplex 3D noise */
double noise_simplex_3d(double x, double y, double z);

/* Simplex 4D noise */
double noise_simplex_4d(double x, double y, double z, double w);

#endif
