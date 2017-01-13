

#include "inventory.h"

#include <string.h>

int inventory_add(struct array *a, struct slot s)
{
	int i, acc, max;
	struct slot s2;

	acc = 0;
	for (i = 0; i < a->size; ++i) {
		array_get(a, i, &s2);
		if (s2.num > 0 && s2.mat == s.mat && s2.obj == s.obj) {
			max = 64 - s2.num;
			if (s.num <= max) {
				acc += s.num;
				s2.num += s.num;
				array_set(a, i, &s2);
				return acc;
			} else {
				acc += max;
				s2.num += max;
				array_set(a, i, &s2);
				s.num -= max;
			}
		}
	}
	for (i = 0; i < a->size; ++i) {
		array_get(a, i, &s2);
		if (s2.num == 0) {
			acc += s.num;
			array_set(a, i, &s);
			return acc;
		}
	}
	return acc;
}

int inventory_add_to_slot(struct array *a, int i, struct slot s)
{
	int max;
	struct slot s2;

	array_get(a, i, &s2);
	if (s2.num == 0) {
		array_set(a, i, &s);
		return s.num;
	} else if (s2.mat == s.mat && s2.obj == s.obj) {
		max = 64 - s2.num;
		if (s.num <= max) {
			s2.num += s.num;
			array_set(a, i, &s2);
			return s.num;
		} else {
			s2.num += max;
			array_set(a, i, &s2);
			return max;
		}
	} else {
		return 0;
	}
}

int inventory_remove(struct array *a, struct slot s)
{
	int i, acc, max;
	struct slot s2;

	for (i = 0; i < a->size; ++i) {
		array_get(a, i, &s2);
		if (s2.num > 0 && s2.mat == s.mat && s2.obj == s.obj) {
			max = s2.num;
			if (s.num <= max) {
				acc += s.num;
				s2.num -= s.num;
				array_set(a, i, &s2);
				return acc;
			} else {
				acc += max;
				s2.num -= max;
				array_set(a, i, &s2);
				s.num -= max;
			}
		}
	}
	return acc;
}

int inventory_remove_from_slot(struct array *a, int i, struct slot s)
{
	int max;
	struct slot s2;

	array_get(a, i, &s2);
	if (s2.num > 0 && s2.mat == s.mat && s2.obj == s.obj) {
		max = s2.num;
		if (s.num <= max) {
			s2.num -= s.num;
			array_set(a, i, &s2);
			return s.num;
		} else {
			s2.num -= max;
			array_set(a, i, &s2);
			return max;
		}
	} else {
		return 0;
	}
}


