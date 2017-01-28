/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#include "command_line.h"

#include <cstring>

CommandLine::CommandLine()
	: cur_line(0), cur_char(0)
{
	visible = "";
}

CommandLine::~CommandLine()
{
	for (auto &s : history)
		free(s);
}

void CommandLine::append(const char *str)
{
	if (visible != buf.data())
		buf = visible;
	buf.insert(cur_char, str);
	cur_char += strlen(str);
	visible = buf.data();
}

int CommandLine::delete_backward()
{
	if (cur_char == 0)
		return 0;
	if (visible != buf.data())
		buf = visible;
	buf.erase(cur_char - 1, 1);
	--cur_char;
	visible = buf.data();
	return 1;
}

int CommandLine::delete_forward()
{
	if (visible[cur_char] == 0)
		return 0;
	if (visible != buf.data())
		buf = visible;
	buf.erase(cur_char, 1);
	visible = buf.data();
	return 1;
}

int CommandLine::prev_char()
{
	if (cur_char == 0)
		return 0;
	--cur_char;
	return 1;
}

int CommandLine::next_char()
{
	if (visible[cur_char] == 0)
		return 0;
	++cur_char;
	return 1;
}

void CommandLine::first_char()
{
	cur_char = 0;
}

void CommandLine::last_char()
{
	cur_char = strlen(visible);
}

int CommandLine::prev_line()
{
	if (cur_line == 0)
		return 0;
	--cur_line;
	visible = history[cur_line];
	cur_char = strlen(visible);
	return 1;
}

int CommandLine::next_line()
{
	if (cur_line == history.size())
		return 0;
	++cur_line;
	if (cur_line == history.size())
		visible = buf.data();
	else
		visible = history[cur_line];
	cur_char = strlen(visible);
	return 1;
}

int CommandLine::push()
{
	char *s;
	if (strlen(visible) == 0)
		return -1;
	history.push_back(strdup(visible));
	buf = "";
	visible = buf.data();
	cur_line = history.size();
	cur_char = 0;
	return 0;
}
