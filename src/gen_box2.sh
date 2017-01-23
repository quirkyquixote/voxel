#!/bin/bash

echo "/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_BOX2_H_
#define SRC_BOX2_H_

#include <cmath>

#include \"v2.h\"

template<typename T> struct box2 {
	struct iterator {
		v2<T> cur;
		box2<T> box;
		iterator(const v2<T> &cur, const box2<T> &box)
			: cur(cur), box(box) { }
		inline void operator++()
		{
			if (cur.y == box.y1) {
				cur.y = box.y0;
				++cur.x;
			} else {
				++cur.y;
			}
		}
		inline bool operator!=(const iterator &rhs) { return cur != rhs.cur; }
		inline v2<T> &operator*() { return cur; }
	};

	T x0, y0, x1, y1;
	box2() : x0(0), y0(0), x1(0), y1(0) {}
	box2(T x0, T y0, T x1, T y1)
		: x0(x0), y0(y0), x1(x1), y1(y1) {}
	template<typename U> box2(const box2<U> &b)
		: x0(b.x0), y0(b.y0), x1(b.x1), y1(b.y1) {}

	iterator begin() const { return iterator(v2<T>(x0, y0), *this); }
	iterator end() const { return iterator(v2<T>(x1 + 1, y0), *this); }";

for op in "+" "-" "*" "/" "%" "&" "|" "^" "<<" ">>"; do echo "
	inline box2<T> operator${op}=(const box2<T> &rhs)
	{
		x0 ${op}= rhs.x0;
		y0 ${op}= rhs.y0;
		x1 ${op}= rhs.x1;
		y1 ${op}= rhs.y1;
		return *this;
	}

	inline box2<T> operator${op}=(const v2<T> &rhs)
	{
		x0 ${op}= rhs.x;
		y0 ${op}= rhs.y;
		x1 ${op}= rhs.x;
		y1 ${op}= rhs.y;
		return *this;
	}

	inline box2<T> operator${op}=(T rhs)
	{
		x0 ${op}= rhs;
		y0 ${op}= rhs;
		x1 ${op}= rhs;
		y1 ${op}= rhs;
		return *this;
	}";
done

echo "};

typedef box2<float> box2f;
typedef box2<double> box2d;
typedef box2<char> box2c;
typedef box2<short> box2s;
typedef box2<long> box2l;
typedef box2<long long> box2ll;";

for op in "+" "-" "*" "/" "%" "&" "|" "^" "<<" ">>"; do echo "
template<typename T> inline box2<T> operator${op}(const box2<T> &lhs, T rhs)
{
	return box2<T>(lhs.x0 ${op} rhs, lhs.y0 ${op} rhs,
			lhs.x1 ${op} rhs, lhs.y1 ${op} rhs);
}

template<typename T> inline box2<T> operator${op}(T lhs, const box2<T> &rhs)
{
	return box2<T>(lhs ${op} rhs.x0, lhs ${op} rhs.y0,
			lhs ${op} rhs.x1, lhs ${op} rhs.y1);
}

template<typename T> inline box2<T> operator${op}(const box2<T> &lhs, const v2<T> &rhs)
{
	return box2<T>(lhs.x0 ${op} rhs.x, lhs.y0 ${op} rhs.y,
			lhs.x1 ${op} rhs.x, lhs.y1 ${op} rhs.y);
}

template<typename T> inline box2<T> operator${op}(const v2<T> &lhs, const box2<T> &rhs)
{
	return box2<T>(lhs.x ${op} rhs.x0, lhs.y ${op} rhs.y0,
			lhs.x ${op} rhs.x1, lhs.y ${op} rhs.y1);
}

template<typename T> inline box2<T> operator${op}(const box2<T> &lhs, const box2<T> &rhs)
{
	return box2<T>(lhs.x0 ${op} rhs.x0, lhs.y0 ${op} rhs.y0,
			lhs.x1 ${op} rhs.x1, lhs.y1 ${op} rhs.y1);
}";
done

for op in "floor" "ceil" "rint"; do echo "
template<typename T> inline box2<T> ${op}(const box2<T> &lhs)
{
	return box2<T>(std::${op}(lhs.x0), std::${op}(lhs.y0),
			std::${op}(lhs.x1), std::${op}(lhs.y1));
}";
done

echo "
template<typename T> box2<T> fix(const box2<T> &b)
{
	return box2<T>(
		b.x0 < b.x1 ? b.x0 : b.x1,
		b.y0 < b.y1 ? b.y0 : b.y1,
		b.x0 > b.x1 ? b.x0 : b.x1,
		b.y0 > b.y1 ? b.y0 : b.y1);
}

template<typename T> bool overlap(const box2<T> &b1, const box2<T> &b2)
{
	return (b2.x0 <= b1.x1 && b2.x1 >= b1.x0 &&
			b2.y0 <= b1.y1 && b2.y1 >= b1.y0);
}

#endif  // SRC_BOX2_H_
";
