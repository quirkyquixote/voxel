/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_LOG_H_
#define SRC_LOG_H_

#include <stdio.h>

static inline int log_index()
{
	static int ret = 0;
	return ret++;
}

#define log_info(fmt, ...) \
	fprintf(stderr, "[%d] INFO: %s: " fmt "\n", log_index(), __func__, ##__VA_ARGS__)
#define log_warning(fmt, ...) \
	fprintf(stderr, "[%d] WARNING: %s: " fmt "\n", log_index(), __func__, ##__VA_ARGS__)
#define log_error(fmt, ...) \
	fprintf(stderr, "[%d] ERROR: %s: " fmt "\n", log_index(), __func__, ##__VA_ARGS__)

#endif  // SRC_LOG_H_
