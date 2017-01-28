/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_EXCEPTION_H_
#define SRC_EXCEPTION_H_

#include <cstdio>
#include <cstdarg>

class Exception {
 public:
	Exception()
	{
		strcpy(str, "");
	}

	Exception(const char *fmt, ...)
	{
		va_list ap;
		va_start(ap, fmt);
		vsnprintf(str, sizeof(str), fmt, ap);
		va_end(ap);
	}

	const char *get_str() const { return str; }

 private:
	char str[256];
};

#endif
