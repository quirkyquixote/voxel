/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_LOG_H_
#define SRC_LOG_H_

#include <stdio.h>

#define log_info(fmt, ...) \
	fprintf(stderr, "INFO: %s: " fmt "\n", __func__, ##__VA_ARGS__)
#define log_warning(fmt, ...) \
	fprintf(stderr, "WARNING: %s: " fmt "\n", __func__, ##__VA_ARGS__)
#define log_error(fmt, ...) \
	fprintf(stderr, "ERROR: %s: " fmt "\n", __func__, ##__VA_ARGS__)

#endif  // SRC_LOG_H_
