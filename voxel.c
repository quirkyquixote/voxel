

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

#include "chunk.h"
#include "terraform.h"

int main(int argc, char *argv[])
{
	struct world *w;
	union sz_tag *root;
	int fd;

	w = world_new();
	terraform(0, 0, 0, w->chunks[0][0]);

	if (world_save(w, &root) != 0)
		return -1;
	fd = creat("world.dat", 0600);
	if (fd < 0) {
		perror("world");
		return -1;
	}
	if (sz_write(fd, root) != 0)
		return -1;
	close(fd);
	sz_destroy(root);

	if (chunk_save(w->chunks[0][0], &root) != 0)
		return -1;
	fd = creat("chunk.dat", 0600);
	if (fd < 0) {
		perror("world");
		return -1;
	}
	if (sz_write(fd, root) != 0)
		return -1;
	close(fd);
	sz_destroy(root);
	return 0;
}
