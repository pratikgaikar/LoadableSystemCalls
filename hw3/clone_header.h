#ifndef _CH_H_
#define _CH_H_

struct ioctl_arg {
	int vector;
	int pid;
	int module_type;
};

struct my_args {
	unsigned long clone_flags;
	unsigned long stack_start;
	unsigned long stack_size;
	int *parent_tidptr;
	int *child_tidptr;
	unsigned long tls;
	int16_t module_id;
	int vector;
};

#endif
