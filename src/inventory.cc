/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

 
#include "inventory.h"

#include <string.h>

int inventory_add(Inventory *a, Item s)
{
	int acc, max;

	acc = 0;
	for (Item &s2 : *a) {
		if (s2.num > 0 && s2.mat == s.mat && s2.obj == s.obj) {
			max = 64 - s2.num;
			if (s.num <= max) {
				acc += s.num;
				s2.num += s.num;
				return acc;
			} else {
				acc += max;
				s2.num += max;
				s.num -= max;
			}
		}
	}
	for (Item &s2 : *a) {
		if (s2.num == 0) {
			acc += s.num;
			s2 = s;
			return acc;
		}
	}
	return acc;
}

int inventory_add_to_item(Inventory *a, int i, Item s)
{
	int max;
	Item s2 = (*a)[i];
	if (s2.num == 0) {
		(*a)[i] = s;
		return s.num;
	} else if (s2.mat == s.mat && s2.obj == s.obj) {
		max = 64 - s2.num;
		if (s.num <= max) {
			s2.num += s.num;
			(*a)[i] = s2;
			return s.num;
		} else {
			s2.num += max;
			(*a)[i] = s2;
			return max;
		}
	} else {
		return 0;
	}
}

int inventory_remove(Inventory *a, Item s)
{
	int acc, max;

	for (Item &s2 : *a) {
		if (s2.num > 0 && s2.mat == s.mat && s2.obj == s.obj) {
			max = s2.num;
			if (s.num <= max) {
				acc += s.num;
				s2.num -= s.num;
				return acc;
			} else {
				acc += max;
				s2.num -= max;
				s.num -= max;
			}
		}
	}
	return acc;
}

int inventory_remove_from_item(Inventory *a, int i, Item s)
{
	int max;

	Item s2 = (*a)[i];
	if (s2.num > 0 && s2.mat == s.mat && s2.obj == s.obj) {
		max = s2.num;
		if (s.num <= max) {
			s2.num -= s.num;
			return s.num;
		} else {
			s2.num -= max;
			return max;
		}
	} else {
		return 0;
	}
}


