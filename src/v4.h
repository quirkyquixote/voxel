
#ifndef VOXEL_V4_H_
#define VOXEL_V4_H_

#include <math.h>

template<typename T> struct v4 {
	T x, y, z, w;
	v4() : x(0), y(0), z(0), w(0) {}
	v4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}
	template<typename U> v4(const v4<U> &rhs) : x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w) {}
};

typedef v4<float> v4f;
typedef v4<double> v4d;
typedef v4<char> v4c;
typedef v4<short> v4s;
typedef v4<long> v4l;
typedef v4<long long> v4ll;

template<typename T> inline bool operator==(const v4<T> &lhs, const v4<T> &rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w;
}

template<typename T> inline bool operator!=(const v4<T> &lhs, const v4<T> &rhs)
{
	return lhs.x != rhs.x || lhs.y != rhs.y || lhs.z != rhs.z || lhs.w != rhs.w;
}

template<typename T> inline v4<T> operator+(const v4<T> &lhs, const v4<T> &rhs)
{
	return v4<T>(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);
}

template<typename T> inline v4<T> operator-(const v4<T> &lhs, const v4<T> &rhs)
{
	return v4<T>(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);
}

template<typename T> inline v4<T> operator*(const v4<T> &lhs, T rhs)
{
	return v4<T>(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs);
}

template<typename T> inline T dot(const v4<T> &lhs, const v4<T> &rhs)
{
	return (lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w);
}

template<typename T> inline T length(const v4<T> &lhs)
{
	return hypot(hypot(hypot(lhs.x, lhs.y), lhs.z), lhs.w);
}

template<typename T> inline T dist(const v4<T> &lhs, const v4<T> &rhs)
{
	return length(lhs - rhs);
}

template<typename T> inline v4<T> normalize(const v4<T> &lhs)
{
	return lhs / length(lhs);
}

#endif
