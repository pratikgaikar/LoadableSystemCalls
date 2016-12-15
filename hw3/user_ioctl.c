#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "clone_header.h"
#include <string.h>
#include <sys/ioctl.h>
#define IOCTL_SET 0
#define IOCTL_GET 1
#define PAGE_SIZE 4096

int main(int argc, char *argv[])
{

	int opt, fd;
	char *ioctl_buffer = NULL;
	struct ioctl_arg arg;
	int ret;
	bool set_flag = false, get_flag = false;

	fd = open("/dev/ioctl0", O_RDWR);
	if (fd == -1) {
		printf("UNABLE TO OPEN FILE\n");
		exit(0);
	}

	/* Allocating memory for get IOCTL */
	ioctl_buffer = malloc(PAGE_SIZE);
	if (ioctl_buffer == NULL) {
		printf("UNABLE TO ALLOCATE MEMORY TO BUFFER\n");
		exit(0);
	}

	while ((opt = getopt(argc, argv, "sg")) != -1) {
		switch (opt) {
		case 's':
			/* FOR IOCTL_SET */
			if (argc != 5) {
				fprintf(stderr, "3 ARGUMENTS EXPECTED\n");
				goto out;
			}
			set_flag = true;
			break;
		case 'g':
			/* FOR IOCTL_GET */
			if (argc > 2) {
				fprintf(stderr,
					"NO ARGUMENTS ALLOWED FOR GET_IOCTL\n");
				goto out;
			}
			get_flag = true;
			break;
		default:	/* '?' */
			fprintf(stderr, "Usage: [-s] [-g] only allowed\n");
			goto out;
		}
	}

	if ((set_flag | get_flag) == false) {
		fprintf(stderr, "PROVIDE AN OPTION [-s] [-g]\n");
		goto out;

	}

	/* First time */
	if (set_flag) {

		if (((atoi(argv[optind])) <= 0)) {
			if ((strcmp(argv[optind], "0") == 0)) {
				arg.pid = atoi(argv[optind]);
			} else {
				fprintf(stderr,
					"PLEASE PROVIDE INTEGER PARAMETERS\n");
				goto out;
			}
		}

		if (((atoi(argv[optind + 1])) <= 0)) {
			if ((strcmp(argv[optind + 1], "0") == 0)) {
				arg.vector = atoi(argv[optind + 1]);
			} else {
				fprintf(stderr,
					"PLEASE PROVIDE INTEGER PARAMETERS\n");
				goto out;
			}
		}

		if (((atoi(argv[optind + 2])) <= 0)) {
			if ((strcmp(argv[optind + 2], "0") == 0)) {
				arg.module_type = atoi(argv[optind + 2]);
			} else {
				fprintf(stderr,
					"PLEASE PROVIDE INTEGER PARAMETERS\n");
				goto out;
			}
		}

		arg.pid = atoi(argv[optind]);
		arg.vector = atoi(argv[optind + 1]);
		arg.module_type = atoi(argv[optind + 2]);

		ret = ioctl(fd, IOCTL_SET, &arg);

	} else if (get_flag) {
		ret = ioctl(fd, IOCTL_GET, ioctl_buffer);
		printf("IOCTL GET returned %d\n", ret);
		printf("IOCTL string is--->%s\n", ioctl_buffer);
	}

 out:
	close(fd);
	if (ioctl_buffer)
		free(ioctl_buffer);
	return 0;
}
