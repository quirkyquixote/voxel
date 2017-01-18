/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_RECIPES_H_
#define SRC_RECIPES_H_

#include <vector>

#include "context.h"
#include "inventory.h"

int recipe_match(std::vector<Item> *inv, Item *s);

#endif  // SRC_RECIPES_H_

