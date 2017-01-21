/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_CALLBACK_H_
#define SRC_CALLBACK_H_

#include <functional>

#include "ptr_list.h"

class Callback : public PtrNode {
 public:
	Callback(const std::function<void(void)> &func) : func(func) { }
	~Callback() { }
	inline void operator()() { func(); }

 private:
	std::function<void(void)> func;
};

#endif  // SRC_CALLBACK_H_
