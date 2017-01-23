#!/bin/bash

echo "/* Copyright 3017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_V3_H_
#define SRC_V3_H_

#include <cmath>

template<typename T> struct v3 {
	T x, y, z;
	v3() : x(0), y(0), z(0) {}
	v3(T x, T y, T z) : x(x), y(y), z(z) {}
	template<typename U> v3(const v3<U> &rhs)
		: x(rhs.x), y(rhs.y), z(rhs.z) {}

	inline bool operator==(const v3<T> &rhs) const
	{
		return x == rhs.x && y == rhs.y && z == rhs.z;
	}

	inline bool operator!=(const v3<T> &rhs) const
	{
		return x != rhs.x || y != rhs.y || z != rhs.z;
	}";

for op in "+" "-" "*" "/" "%" "&" "|" "^" "<<" ">>"; do echo "
	inline v3<T> operator${op}=(const v3<T> &rhs)
	{
		x ${op}= rhs.x;
		y ${op}= rhs.y;
		z ${op}= rhs.z;
		return *this;
	}

	inline v3<T> operator${op}=(T rhs)
	{
		x ${op}= rhs;
		y ${op}= rhs;
		z ${op}= rhs;
		return *this;
	}";
done

echo "};

typedef v3<float> v3f;
typedef v3<double> v3d;
typedef v3<char> v3c;
typedef v3<short> v3s;
typedef v3<long> v3l;
typedef v3<long long> v3ll;";

for op in "+" "-" "*" "/" "%" "&" "|" "^" "<<" ">>"; do echo "
template<typename T> inline v3<T> operator${op}(const v3<T> &lhs, T rhs)
{
	return v3<T>(lhs.x ${op} rhs, lhs.y ${op} rhs, lhs.z ${op} rhs);
}

template<typename T> inline v3<T> operator${op}(T lhs, const v3<T> &rhs)
{
	return v3<T>(lhs ${op} rhs.x, lhs ${op} rhs.y, lhs ${op} rhs.z);
}

template<typename T> inline v3<T> operator${op}(const v3<T> &lhs, const v3<T> &rhs)
{
	return v3<T>(lhs.x ${op} rhs.x, lhs.y ${op} rhs.y, lhs.z ${op} rhs.z);
}";
done

echo "
template<typename T> inline T dot(const v3<T> &lhs, const v3<T> &rhs)
{
	return (lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z);
}

template<typename T> inline T length(const v3<T> &lhs)
{
	return std::hypot(std::hypot(lhs.x, lhs.y), lhs.z);
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
	T c = std::cos(a);
	T s = std::sin(a);
	return v3<T>(lhs.x, c * lhs.y - s * lhs.z, s * lhs.y + c * lhs.z);
}

template<typename T> inline v3<T> roty(const v3<T> &lhs, T a)
{
	T c = std::cos(a);
	T s = std::sin(a);
	return v3<T>(c * lhs.x - s * lhs.z, lhs.y, s * lhs.x + c * lhs.z);
}

template<typename T> inline v3<T> rotz(const v3<T> &lhs, T a)
{
	T c = std::cos(a);
	T s = std::sin(a);
	return v3<T>(c * lhs.x - s * lhs.y, s * lhs.x + c * lhs.y, lhs.z);
}

template<typename T> inline v3<T> rot(const v3<T> &lhs, const v3<T> &a)
{
	return rotx(roty(rotx(lhs, a.x), a.y), a.z);
}

#endif  // SRC_V3_H_
";
