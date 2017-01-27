/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_OPTION_PARSER_H_
#define SRC_OPTION_PARSER_H_

#include <string>
#include <vector>

struct Value {
	virtual ~Value() {}
	virtual int parse(char **p) = 0;
};

class BoolValue : public Value {
 public:
	BoolValue() : val(false) {}
	int parse(char **p);
	bool get_val() const { return val; }

 private:
	bool val;
};

class IntValue : public Value {
 public:
	IntValue() : val(0) {}
	int parse(char **p);
	int get_val() const { return val; }

 private:
	int val;
};

class StringValue : public Value {
 public:
	StringValue() {}
	int parse(char **p);
	const char *get_val() const { return val.c_str(); }

 private:
	std::string val;
};

class Option {
 public:
	Option(char sh, const char *lo, const char *help, Value *val)
		: sh(sh), lo(lo), help(help), val(val) { }

	~Option() { }

	int parse(char **p) const;
	void print_help();

 private:
	char sh;
	const char *lo;
	const char *help;
	Value *val;
};

int parse_arguments(int argc, char *argv[], const std::vector<Option> &options);

#endif  // SRC_OPTION_PARSER_H_
