

#include <unistd.h>

#include "chunk.h"
#include "terraform.h"

int main(int argc, char *argv[])
{
	struct chunk *c;
	union sz_tag *root;

	c = chunk_new();
	terraform(0, 0, 0, c);
	if (chunk_save(c, &root) != 0)
		return -1;
	if (sz_write(STDOUT_FILENO, root) != 0)
		return -1;
	sz_destroy(root);
	return 0;
}
