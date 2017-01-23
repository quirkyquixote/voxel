#!/bin/bash

echo "/* Copyright 3017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_BOX3_H_
#define SRC_BOX3_H_

#include <cmath>

#include \"v3.h\"

template<typename T> struct box3 {
	struct iterator {
		v3<T> cur;
		box3<T> box;
		iterator(const v3<T> &cur, const box3<T> &box)
			: cur(cur), box(box) { }
		inline void operator++()
		{
			if (cur.z == box.z1) {
				cur.z = box.z0;
				if (cur.y == box.y1) {
					cur.y = box.y0;
					++cur.x;
				} else {
					++cur.y;
				}
			} else {
				++cur.z;
			}
		}
		inline bool operator!=(const iterator &rhs) { return cur != rhs.cur; }
		inline v3<T> &operator*() { return cur; }
	};

	T x0, y0, z0, x1, y1, z1;
	box3() : x0(0), y0(0), z0(0), x1(0), y1(0), z1(0) {}
	box3(T x0, T y0, T z0, T x1, T y1, T z1)
		: x0(x0), y0(y0), z0(z0), x1(x1), y1(y1), z1(z1) {}
	template<typename U> box3(const box3<U> &b)
		: x0(b.x0), y0(b.y0), z0(b.z0), x1(b.x1), y1(b.y1), z1(b.z1) {}

	iterator begin() const { return iterator(v3<T>(x0, y0, z0), *this); }
	iterator end() const { return iterator(v3<T>(x1 + 1, y0, z0), *this); }";

for op in "+" "-" "*" "/" "%" "&" "|" "^" "<<" ">>"; do echo "
	inline box3<T> operator${op}=(const box3<T> &rhs)
	{
		x0 ${op}= rhs.x0;
		y0 ${op}= rhs.y0;
		z0 ${op}= rhs.z0;
		x1 ${op}= rhs.x1;
		y1 ${op}= rhs.y1;
		z1 ${op}= rhs.z1;
		return *this;
	}

	inline box3<T> operator${op}=(const v3<T> &rhs)
	{
		x0 ${op}= rhs.x;
		y0 ${op}= rhs.y;
		z0 ${op}= rhs.z;
		x1 ${op}= rhs.x;
		y1 ${op}= rhs.y;
		z1 ${op}= rhs.z;
		return *this;
	}

	inline box3<T> operator${op}=(T rhs)
	{
		x0 ${op}= rhs;
		y0 ${op}= rhs;
		z0 ${op}= rhs;
		x1 ${op}= rhs;
		y1 ${op}= rhs;
		z1 ${op}= rhs;
		return *this;
	}";
done

echo "};

typedef box3<float> box3f;
typedef box3<double> box3d;
typedef box3<char> box3c;
typedef box3<short> box3s;
typedef box3<long> box3l;
typedef box3<long long> box3ll;";

for op in "+" "-" "*" "/" "%" "&" "|" "^" "<<" ">>"; do echo "
template<typename T> inline box3<T> operator${op}(const box3<T> &lhs, T rhs)
{
	return box3<T>(lhs.x0 ${op} rhs, lhs.y0 ${op} rhs, lhs.z0 ${op} rhs,
			lhs.x1 ${op} rhs, lhs.y1 ${op} rhs, lhs.z1 ${op} rhs.z);
}

template<typename T> inline box3<T> operator${op}(T lhs, const box3<T> &rhs)
{
	return box3<T>(lhs ${op} rhs.x0, lhs ${op} rhs.y0, lhs ${op} rhs.z0,
			lhs ${op} rhs.x1, lhs ${op} rhs.y1, lhs ${op} rhs.z1);
}

template<typename T> inline box3<T> operator${op}(const box3<T> &lhs, const v3<T> &rhs)
{
	return box3<T>(lhs.x0 ${op} rhs.x, lhs.y0 ${op} rhs.y, lhs.z0 ${op} rhs.z,
			lhs.x1 ${op} rhs.x, lhs.y1 ${op} rhs.y, lhs.z1 ${op} rhs.z);
}

template<typename T> inline box3<T> operator${op}(const v3<T> &lhs, const box3<T> &rhs)
{
	return box3<T>(lhs.x ${op} rhs.x0, lhs.y ${op} rhs.y0, lhs.z ${op} rhs.z0,
			lhs.x ${op} rhs.x1, lhs.y ${op} rhs.y1, lhs.z ${op} rhs.z1);
}

template<typename T> inline box3<T> operator${op}(const box3<T> &lhs, const box3<T> &rhs)
{
	return box3<T>(lhs.x0 ${op} rhs.x0, lhs.y0 ${op} rhs.y0, lhs.z0 ${op} rhs.z0,
			lhs.x1 ${op} rhs.x1, lhs.y1 ${op} rhs.y1, lhs.z1 ${op} rhs.z1);
}";
done

echo "
template<typename T> box3<T> fix(const box3<T> &b)
{
	return box3<T>(
		b.x0 < b.x1 ? b.x0 : b.x1,
		b.y0 < b.y1 ? b.y0 : b.y1,
		b.z0 < b.z1 ? b.z0 : b.z1,
		b.x0 > b.x1 ? b.x0 : b.x1,
		b.y0 > b.y1 ? b.y0 : b.y1,
		b.z0 > b.z1 ? b.z0 : b.z1);
}

template<typename T> box3<T> grow(const box3<T> &b, T k)
{
	return box3<T>(b.x0 - k, b.y0 - k, b.z0 - k,
			b.x1 + k, b.y1 + k, b.z1 + k);
}

template<typename T> bool overlap(const box3<T> &b1, const box3<T> &b3)
{
	return (b3.x0 <= b1.x1 && b3.x1 >= b1.x0 &&
			b3.y0 <= b1.y1 && b3.y1 >= b1.y0 &&
			b3.z0 <= b1.z1 && b3.z1 >= b1.z0);
}

template<typename T> box3<T> floor(const box3<T> &b)
{
	return box3<T>(std::floor(b.x0), std::floor(b.y0), std::floor(b.z0),
			std::floor(b.x1), std::floor(b.y1), std::floor(b.z1));
}

#endif  // SRC_BOX3_H_
";
