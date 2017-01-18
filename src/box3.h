
#ifndef VOXEL_AAB3_H_
#define VOXEL_AAB3_H_

template<typename T> struct box3 {
	struct iterator {
		v3<T> cur;
		box3<T> box;
		iterator(v3<T> cur, box3<T> box) : cur(cur), box(box) { }
		inline void operator++()
		{
			++cur.z;
			if (cur.z == box.z1) {
				cur.z = box.z0;
				++cur.y;
				if (cur.y == box.y1) {
					cur.y = box.y0;
					++cur.x;
				}
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
	iterator end() const { return iterator(v3<T>(x1, y0, z0), *this); }
};

template<typename T> struct box3_iterator {
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
	return box3<T>(b.x0 - k, b.y0 - k, b.z0 - k, b.x0 + k, b.y0 + k, b.z0 + k);
}

template<typename T> bool overlap(const box3<T> &b1, const box3<T> &b2)
{
	return (b2.x0 <= b1.x1 && b2.x1 >= b1.x0 &&
			b2.y0 <= b1.y1 && b2.y1 >= b1.y0 &&
			b2.z0 <= b1.z1 && b2.z1 >= b1.z0);
}

#define box3_foreach(_iter,_bbox) \
	for (_iter.x = _bbox.x0; _iter.x < _bbox.x1; ++_iter.x) \
for (_iter.y = _bbox.y0; _iter.y < _bbox.y1; ++_iter.y) \
for (_iter.z = _bbox.z0; _iter.z < _bbox.z1; ++_iter.z)

#endif
