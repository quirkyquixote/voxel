
#ifndef VOXEL_CLI_H_
#define VOXEL_CLI_H_

#include "array.h"
#include "str.h"

struct cli {
	struct str* buf;
	struct array *history;
	char *visible;
	int cur_line;
	int cur_char;
};

struct cli *cli(void);

void cli_append(struct cli *c, const char *str);
int cli_delete_backward(struct cli *c);
int cli_delete_forward(struct cli *c);
int cli_prev_char(struct cli *c);
int cli_next_char(struct cli *c);
void cli_first_char(struct cli *c);
void cli_last_char(struct cli *c);
int cli_prev_line(struct cli *c);
int cli_next_line(struct cli *c);
int cli_push(struct cli *c);

#endif
