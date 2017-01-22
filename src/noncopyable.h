/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_NONCOPYABLE_H_
#define SRC_NONCOPYABLE_H_

class NonCopyable {
 public:
	NonCopyable() = default;
	~NonCopyable() = default;

	NonCopyable(const NonCopyable &other) = delete;
	NonCopyable(NonCopyable &&other) = delete;
};

#endif  // SRC_NONCOPYABLE_H_
