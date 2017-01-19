/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_BOX3_H_
#define SRC_BOX3_H_

template<typename T> struct box3 {
	struct iterator {
		v3<T> cur;
		box3<T> box;
		iterator(v3<T> cur, box3<T> box) : cur(cur), box(box) { }
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
	box3(T x0, T y0, T z0, T x1, T y1, T z1)
		: x0(x0), y0(y0), z0(z0), x1(x1), y1(y1), z1(z1) {}
	box3() : x0(0), y0(0), z0(0), x1(0), y1(0), z1(0) {}

	iterator begin() const { return iterator(v3<T>(x0, y0, z0), *this); }
	iterator end() const { return iterator(v3<T>(x1 + 1, y0, z0), *this); }
};

typedef box3<float> box3f;
typedef box3<double> box3d;
typedef box3<char> box3c;
typedef box3<short> box3s;
typedef box3<long> box3l;
typedef box3<long long> box3ll;

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

template<typename T> box3<T> shift(const box3<T> &b, const v3<T> &p)
{
	return box3<T>(b.x0 + p.x, b.y0 + p.y, b.z0 + p.z,
			b.x1 + p.x, b.y1 + p.y, b.z1 + p.z);
}

template<typename T> bool overlap(const box3<T> &b1, const box3<T> &b2)
{
	return (b2.x0 <= b1.x1 && b2.x1 >= b1.x0 &&
			b2.y0 <= b1.y1 && b2.y1 >= b1.y0 &&
			b2.z0 <= b1.z1 && b2.z1 >= b1.z0);
}

#endif  // SRC_BOX3_H_
