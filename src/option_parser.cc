/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "option_parser.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "log.h"

namespace option_parser {

template<typename T> int Value<T>::parse(char **p)
{
	log_error("Unimplemented");
	exit(EXIT_FAILURE);
	return 0;
}

template<> int Value<bool>::parse(char **p)
{
	*val = true;
	return 0;
}

template<> int Value<char>::parse(char **p)
{
	if ((*p)[0] == 0 || (*p)[1] != 0) {
		log_error("%s: expected char", *p);
		exit(EXIT_FAILURE);
	}
	*val = (*p)[0];
	return 1;
}

template<> int Value<short>::parse(char **p)
{
	char *end;
	*val = strtol(*p, &end, 10);
	if (*end != 0) {
		log_error("%s: expected short", *p);
		exit(EXIT_FAILURE);
	}
	return 1;
}

template<> int Value<long>::parse(char **p)
{
	char *end;
	*val = strtol(*p, &end, 10);
	if (*end != 0) {
		log_error("%s: expected long", *p);
		exit(EXIT_FAILURE);
	}
	return 1;
}

template<> int Value<long long>::parse(char **p)
{
	char *end;
	*val = strtoll(*p, &end, 10);
	if (*end != 0) {
		log_error("%s: expected long long", *p);
		exit(EXIT_FAILURE);
	}
	return 1;
}

template<> int Value<char *>::parse(char **p)
{
	if (**p == '-') {
		log_error("%s: expected string", *p);
		exit(EXIT_FAILURE);
	}
	*val = *p;
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

int parse(char *argv[], int argc, Option *optv, int optc)
{
	int i = 1;
	while (i < argc && argv[i][0] == '-') {
		bool found = false;
		for (int j = 0; j < optc; ++j) {
			int k = optv[j].parse(argv + i);
			if (k > 0) {
				i += k;
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

void print_help(Option *optv, int optc)
{
	for (int i = 0; i < optc; ++i)
		optv[i].print_help();
}

};  // namespace option_parser
