#include <sys/wait.h>
#include <sys/utsname.h>
#include <string.h>
#include <asm/unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <linux/sched.h>
#include "clone_header.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#define STACK_SIZE (1024 * 1024)

/* User code, expects params, system vector and
 * module ID*/

int main(int argc, char *argv[])
{
	long pid;
	char *stack = NULL;	/* Start of stack buffer */
	char *stackTop;		/* End of stack buffer */
	int parent_addr, child_addr;
	struct my_args *ptr = NULL;

	/*Check for the number of arguments */
	if (argc != 3) {
		fprintf(stderr, "2 PARAMETERS ARE EXPECTED\n");
		exit(EXIT_FAILURE);

	}

	ptr = malloc(sizeof(struct my_args));
	if (ptr == NULL) {
		printf("MEMEORY ALLOCATION FAILED\n");
		goto out;
	}

	/* Allocate stack for child */
	stack = malloc(STACK_SIZE);
	if (stack == NULL) {
		printf("MEMORY ALLOCATION FAILED\n");
		goto out;
	}
	stackTop = stack + STACK_SIZE;	/* Assume stack grows downward */

	ptr->clone_flags = 1;
	ptr->stack_start = *stackTop;
	ptr->stack_size = STACK_SIZE;
	ptr->parent_tidptr = &parent_addr;
	ptr->child_tidptr = &child_addr;
	ptr->tls = 1;

	if (((atoi(argv[1])) <= 0)) {
		if ((strcmp(argv[1], "0") == 0)) {
			ptr->vector = atoi(argv[1]);
		} else {
			fprintf(stderr, "PLEASE PROVIDE INTEGER PARAMETERS\n");
			goto out;
		}
	}

	if (((atoi(argv[2])) <= 0)) {
		if ((strcmp(argv[2], "0") == 0)) {
			ptr->module_id = atoi(argv[2]);
		} else {
			fprintf(stderr, "PLEASE PROVIDE INTEGER PARAMETERS\n");
			goto out;
		}
	}

	ptr->vector = atoi(argv[1]);
	ptr->module_id = atoi(argv[2]);

	sleep(40);

	pid = syscall(329, ptr);
	if (pid == -1) {
		goto out;
	}
	sleep(40);

	rmdir("TEST NECLONE");
 out:
	if (ptr)
		free(ptr);
	if (stack)
		free(stack);
	return 0;
}
