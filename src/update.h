/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_UPDATE_H_
#define SRC_UPDATE_H_

#include <vector>

#include "context.h"
#include "inventory.h"

void update(Context *ctx);

void use_inventory(Context *ctx, std::vector<Item> *inv);
void use_workbench(Context *ctx, std::vector<Item> *inv);

#endif  // SRC_UPDATE_H_
