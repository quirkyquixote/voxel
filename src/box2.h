/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_BOX2_H_
#define SRC_BOX2_H_

template<typename T> struct box2 {
	struct iterator {
		v2<T> cur;
		box2<T> box;
		iterator(v2<T> cur, box2<T> box) : cur(cur), box(box) { }
		inline void operator++()
		{
			++cur.y;
			if (cur.y == box.y1) {
				cur.y = box.y0;
				++cur.x;
			}
		}
		inline bool operator!=(const iterator &rhs) { return cur != rhs.cur; }
		inline v2<T> &operator*() { return cur; }
	};

	T x0, y0, x1, y1;
	box2(T x0, T y0, T x1, T y1)
		: x0(x0), y0(y0), x1(x1), y1(y1) {}
	box2() : x0(0), y0(0), x1(0), y1(0) {}

	iterator begin() const { return iterator(v2<T>(x0, y0), *this); }
	iterator end() const { return iterator(v2<T>(x1, y0), *this); }
};

template<typename T> struct box2_iterator {
};

typedef box2<float> box2f;
typedef box2<double> box2d;
typedef box2<char> box2c;
typedef box2<short> box2s;
typedef box2<long> box2l;
typedef box2<long long> box2ll;

template<typename T> box2<T> fix(const box2<T> &b)
{
	return box2<T>(
			b.x0 < b.x1 ? b.x0 : b.x1,
			b.y0 < b.y1 ? b.y0 : b.y1,
			b.x0 > b.x1 ? b.x0 : b.x1,
			b.y0 > b.y1 ? b.y0 : b.y1);
}

template<typename T> box2<T> grow(const box2<T> &b, T k)
{
	return box2<T>(b.x0 - k, b.y0 - k, b.x1 + k, b.y1 + k);
}

template<typename T> box2<T> shift(const box2<T> &b, const v2<T> &p)
{
	return box2<T>(b.x0 + p.x, b.y0 + p.y, b.x1 + p.x, b.y1 + p.y);
}

template<typename T> bool overlap(const box2<T> &b1, const box2<T> &b2)
{
	return (b2.x0 <= b1.x1 && b2.x1 >= b1.x0 &&
			b2.y0 <= b1.y1 && b2.y1 >= b1.y0);
}

#endif  // SRC_BOX2_H_
