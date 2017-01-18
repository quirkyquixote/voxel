
#ifndef VOXEL_V3_H_
#define VOXEL_V3_H_

#include <math.h>

template<typename T> struct v3 {
	T x, y, z;
	v3() : x(0), y(0), z(0) {}
	v3(T x, T y, T z) : x(x), y(y), z(z) {}
	template<typename U> v3(const v3<U> &rhs) : x(rhs.x), y(rhs.y), z(rhs.z) {}
};

typedef v3<float> v3f;
typedef v3<double> v3d;
typedef v3<char> v3c;
typedef v3<short> v3s;
typedef v3<long> v3l;
typedef v3<long long> v3ll;

template<typename T> inline bool operator==(const v3<T> &lhs, const v3<T> &rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

template<typename T> inline bool operator!=(const v3<T> &lhs, const v3<T> &rhs)
{
	return lhs.x != rhs.x || lhs.y != rhs.y || lhs.z != rhs.z;
}

template<typename T> inline v3<T> operator+(const v3<T> &lhs, const v3<T> &rhs)
{
	return v3<T>(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

template<typename T> inline v3<T> operator-(const v3<T> &lhs, const v3<T> &rhs)
{
	return v3<T>(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

template<typename T> inline v3<T> operator*(const v3<T> &lhs, T rhs)
{
	return v3<T>(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
}

template<typename T> inline T dot(const v3<T> &lhs, const v3<T> &rhs)
{
	return (lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z);
}

template<typename T> inline T length(const v3<T> &lhs)
{
	return hypot(hypot(lhs.x, lhs.y), lhs.z);
}

template<typename T> inline T dist(const v3<T> &lhs, const v3<T> &rhs)
{
	return length(lhs - rhs);
}

template<typename T> inline v3<T> normalize(const v3<T> &lhs)
{
	return lhs / length(lhs);
}

template<typename T> inline v3<T> rotx(const v3<T> &lhs, T a)
{
	T c = cos(a);
	T s = sin(a);
	return v3<T>(lhs.x, c * lhs.y - s * lhs.z, s * lhs.y + c * lhs.z);
}

template<typename T> inline v3<T> roty(const v3<T> &lhs, T a)
{
	T c = cos(a);
	T s = sin(a);
	return v3<T>(c * lhs.x - s * lhs.z, lhs.y, s * lhs.x + c * lhs.z);
}

template<typename T> inline v3<T> rotz(const v3<T> &lhs, T a)
{
	T c = cos(a);
	T s = sin(a);
	return v3<T>(c * lhs.x - s * lhs.y, s * lhs.x + c * lhs.y, lhs.z);
}

template<typename T> inline v3<T> rot(const v3<T> &lhs, const v3<T> &a)
{
	return rotx(roty(rotx(lhs, a.x), a.y), a.z);
}

#endif
