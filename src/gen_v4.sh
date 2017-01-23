#!/bin/bash

echo "/* Copyright 4017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_V4_H_
#define SRC_V4_H_

#include <cmath>

template<typename T> struct v4 {
	T x, y, z, w;
	v4() : x(0), y(0), z(0), w(0) {}
	v4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}
	template<typename U> v4(const v4<U> &rhs)
		: x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w) {}

	inline bool operator==(const v4<T> &rhs) const
	{
		return x == rhs.x && y == rhs.y;
	}

	inline bool operator!=(const v4<T> &rhs) const
	{
		return x != rhs.x || y != rhs.y;
	}";

for op in "+" "-" "*" "/" "%" "&" "|" "^" "<<" ">>"; do echo "
	inline v4<T> operator${op}=(const v4<T> &rhs)
	{
		x ${op}= rhs.x;
		y ${op}= rhs.y;
		z ${op}= rhs.z;
		w ${op}= rhs.w;
		return *this;
	}

	inline v4<T> operator${op}=(T rhs)
	{
		x ${op}= rhs;
		y ${op}= rhs;
		z ${op}= rhs;
		w ${op}= rhs;
		return *this;
	}";
done

echo "};

typedef v4<float> v4f;
typedef v4<double> v4d;
typedef v4<char> v4c;
typedef v4<short> v4s;
typedef v4<long> v4l;
typedef v4<long long> v4ll;";

for op in "+" "-" "*" "/" "%" "&" "|" "^" "<<" ">>"; do echo "
template<typename T> inline v4<T> operator${op}(const v4<T> &lhs, T rhs)
{
	return v4<T>(lhs.x ${op} rhs, lhs.y ${op} rhs, lhs.z ${op} rhs, lhs.w ${op} rhs);
}

template<typename T> inline v4<T> operator${op}(T lhs, const v4<T> &rhs)
{
	return v4<T>(lhs ${op} rhs.x, lhs ${op} rhs.y, lhs ${op} rhs.z, lhs ${op} rhs.w);
}

template<typename T> inline v4<T> operator${op}(const v4<T> &lhs, const v4<T> &rhs)
{
	return v4<T>(lhs.x ${op} rhs.x, lhs.y ${op} rhs.y, lhs.z ${op} rhs.z, lhs.w ${op} rhs.w);
}";
done

echo "
template<typename T> inline T dot(const v4<T> &lhs, const v4<T> &rhs)
{
	return (lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w);
}

template<typename T> inline T length(const v4<T> &lhs)
{
	return std::hypot(std::hypot(std::hypot(lhs.x, lhs.y), lhs.z), lhs.w);
}

template<typename T> inline T dist(const v4<T> &lhs, const v4<T> &rhs)
{
	return length(lhs - rhs);
}

template<typename T> inline v4<T> normalize(const v4<T> &lhs)
{
	return lhs / length(lhs);
}


#endif  // SRC_V4_H_
";
