#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	int fd, ret;
	char v, *p, *q;

	if (argc != 2) {
		fprintf(stderr, "usage: keepfd path_to_file\n");
		return -1;
	}

	fd = open(argv[1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	if (fd == -1) {
		perror("open");
		return -1;
	}

	ret = ftruncate(fd, 68336);
	if (ret == -1) {
		perror("truncate");
		return -1;
	}

	p = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (p == MAP_FAILED) {
		perror("mmap");
		return -1;
	}
	*p = 1;
	printf("p=%p\n", p);
	fflush(stdout);

	q = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 4096 * 9);
	if (q == MAP_FAILED) {
		perror("mmap q");
		return -1;
	}
	printf("q=%p\n", q);
	fflush(stdout);

	close(fd);
	unlink(argv[1]);

	// sleep 10 seconds so we can do checkpoint/restore
	sleep(10);

	// should be executed in the restored container
	v = *p;
	printf("v=%d\n", v);
	fflush(stdout);

	*q = 10;
	printf("v=%d\n", *q);

	return 0;
}
