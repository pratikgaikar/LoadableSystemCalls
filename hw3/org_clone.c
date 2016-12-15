#include <asm/unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <linux/sched.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>	
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
		
#define STACK_SIZE (65536)
#define _GNU_SOURCE

int clone_body(void *arg)
{
	rmdir("rmdir CHILD");
	sleep(30);
	exit(0);
}

int main(int argc, char **argv)
{

	char *buff = NULL;
	void *stack = NULL;
	void *stack_start;
	int flag = 0;
	int opt = 0;

	/* OPTIND */
	while ((opt = getopt(argc, argv, "pd")) != -1) {
		switch (opt) {
		case 'd':
			/* FOR CLONE FLAG SET */
			/* WILL TAKE DEFAULT VECTOR */
			flag = 4096;
			break;
		case 'p':
			/* FOR CLONE FLAG RESET */
			flag = 0;
			break;
		default:	/* '?' */
			fprintf(stderr, "Usage: [-p] [-d] only allowed\n");
			goto out;
		}
	}

	if (argc != 2) {
		fprintf(stderr, "Only Flag option is expected\n");
		goto out;
	}

	buff = malloc(52 * sizeof(char));
	if (buff == NULL) {
		printf("UNABLE TO ALLOCATE MEMORY TO BUFFER\n");
		goto out;
	}

	stack = (void *)malloc(65536);
	if (stack == NULL) {
		printf("UNABLE TO ALLOCATE MEMORY TO BUFFER\n");
		goto out;
	}

	stack_start = stack + 6553;
	sleep(30);

	clone(&clone_body, stack_start, CLONE_FS | flag, NULL);

	rmdir("rmdir PARENT");

	sleep(40);

 out:
	if (buff)
		free(buff);
	if (stack)
		free(stack);
	exit(0);
}
