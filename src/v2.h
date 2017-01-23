/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_V2_H_
#define SRC_V2_H_

#include <cmath>

template<typename T> struct v2 {
	T x, y;
	v2() : x(0), y(0) {}
	v2(T x, T y) : x(x), y(y) {}
};

typedef v2<float> v2f;
typedef v2<double> v2d;
typedef v2<char> v2c;
typedef v2<short> v2s;
typedef v2<long> v2l;
typedef v2<long long> v2ll;

template<typename T> inline bool operator==(const v2<T> &lhs, const v2<T> &rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y;
}

template<typename T> inline bool operator!=(const v2<T> &lhs, const v2<T> &rhs)
{
	return lhs.x != rhs.x || lhs.y != rhs.y;
}

template<typename T> inline v2<T> operator+(const v2<T> &lhs, const v2<T> &rhs)
{
	return v2<T>(lhs.x + rhs.x, lhs.y + rhs.y);
}

template<typename T> inline v2<T> operator-(const v2<T> &lhs, const v2<T> &rhs)
{
	return v2<T>(lhs.x - rhs.x, lhs.y - rhs.y);
}

template<typename T> inline v2<T> operator*(const v2<T> &lhs, T rhs)
{
	return v2<T>(lhs.x * rhs, lhs.y * rhs);
}

template<typename T> inline T dot(const v2<T> &lhs, const v2<T> &rhs)
{
	return (lhs.x * rhs.x + lhs.y * rhs.y);
}

template<typename T> inline T length(const v2<T> &lhs)
{
	return std::hypot(lhs.x, lhs.y);
}

template<typename T> inline T dist(const v2<T> &lhs, const v2<T> &rhs)
{
	return length(lhs - rhs);
}

template<typename T> inline v2<T> normalize(const v2<T> &lhs)
{
	return lhs / length(lhs);
}

template<typename T> inline v2<T> rot(const v2<T> &lhs, T a)
{
	T c = std::cos(a);
	T s = std::sin(a);
	return v2<T>(c * lhs.x - s * lhs.y, s * lhs.x + c * lhs.y);
}

#endif  // SRC_V2_H_
