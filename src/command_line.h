/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_COMMAND_LINE_H_
#define SRC_COMMAND_LINE_H_

#include <string>
#include <vector>

class CommandLine {
 public:
	CommandLine();
	~CommandLine();

	void append(const char *str);
	int delete_backward();
	int delete_forward();
	int prev_char();
	int next_char();
	void first_char();
	void last_char();
	int prev_line();
	int next_line();
	int push();

	inline const char *get_visible() const { return visible; }
	inline int get_cur_char() const  { return cur_char; }
	inline int get_cur_line() const  { return cur_line; }

 private:
	std::string buf;
	std::vector<char *> history;
	const char *visible;
	int cur_line;
	int cur_char;
};

#endif  // SRC_COMMAND_LINE_H_
