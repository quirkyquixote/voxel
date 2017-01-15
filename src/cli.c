
#include "cli.h"

struct cli *cli(void)
{
	struct cli *cl = calloc(1, sizeof(*cl));
	cl->buf = str();
	cl->history = array(sizeof(char *));
	return cl;
}

void cli_append(struct cli *c, const char *str)
{
	if (c->visible != c->buf->str)
		str_assign(c->buf, c->visible, strlen(c->visible));
	str_insert(c->buf, c->cur_char, str, strlen(str));
	c->cur_char += strlen(str);
	c->visible = c->buf->str;
}

int cli_delete(struct cli *c)
{
	if (c->cur_char == 0)
		return 0;
	if (c->visible != c->buf->str)
		str_assign(c->buf, c->visible, strlen(c->visible));
	str_erase_char(c->buf, c->cur_char - 1);
	--c->cur_char;
	c->visible = c->buf->str;
	return 1;
}

int cli_prev_char(struct cli *c)
{
	if (c->cur_char == 0)
		return 0;
	--c->cur_char;
	return 1;
}

int cli_next_char(struct cli *c)
{
	if (c->visible[c->cur_char] == 0)
		return 0;
	++c->cur_char;
	return 1;
}

int cli_prev_line(struct cli *c)
{
	if (c->cur_line == 0)
		return 0;
	--c->cur_line;
	array_get(c->history, c->cur_line, &c->visible);
	c->cur_char = strlen(c->visible);
	return 1;
}

int cli_next_line(struct cli *c)
{
	if (c->cur_line == c->history->size)
		return 0;
	++c->cur_line;
	if (c->cur_line == c->history->size)
		c->visible = c->buf->str;
	else
		array_get(c->history, c->cur_line, &c->visible);
	c->cur_char = strlen(c->visible);
	return 1;
}

int cli_push(struct cli *c)
{
	char *s;
	if (strlen(c->visible) == 0)
		return -1;
	s = strdup(c->visible);
	array_push(c->history, &s);
	str_assign(c->buf, "", 0);
	c->visible = c->buf->str;
	c->cur_line = c->history->size;
	c->cur_char = 0;
	return 0;
}
