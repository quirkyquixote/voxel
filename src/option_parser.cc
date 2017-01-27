/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "option_parser.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "log.h"

int BoolValue::parse(char **p)
{
	val = true;
	return 0;
}

int IntValue::parse(char **p)
{
	char *end;
	val = strtol(*p, &end, 10);
	if (*end != 0) {
		log_error("%s: not an integer", *p);
		exit(EXIT_FAILURE);
	}
	return 1;
}

int StringValue::parse(char **p)
{
	if (**p == '-') {
		log_error("%s: expected string", *p);
		exit(EXIT_FAILURE);
	}
	val = *p;
	return 1;
}

int Option::parse(char **p) const
{
	char *s = (*p) + 1;
	if (s[0] == '-') {
		if (strcmp(s + 1, lo) != 0)
			return false;
	} else {
		if (s[0] != sh)
			return false;
	}
	return 1 + val->parse(p);
}

void Option::print_help()
{
	int i = 0;
	i += printf("  ");
	if (sh) {
		i += printf("-%c", sh);
		if (lo)
			i += printf(", --%s", lo);
	} else {
		if (lo)
			i += printf("--%s", lo);
	}
	while (i < 24) {
		printf(" ");
		++i;
	}
	printf("%s\n", help);
}

int parse_arguments(int argc, char *argv[], const std::vector<Option> &options)
{
	int i = 1;
	while (i < argc) {
		bool found = false;
		for (auto &o : options) {
			int j = o.parse(argv + i);
			if (j > 0) {
				i += j;
				found = true;
				break;
			}
		}
		if (found == false) {
			log_error("Bad option: %s", argv[i]);
			exit(EXIT_FAILURE);
		}
	}
	return i;
}

