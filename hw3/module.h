#ifndef _MODULE_H_
#define _MODULE_H_

#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/init.h>		/* Needed for the macros */
#include <linux/slab.h>
#include <stdarg.h>

typedef enum {
	SYS_OPEN,
	SYS_CLOSE,
	SYS_READ,
	SYS_WRITE,
	SYS_RENAME,
	SYS_RMDIR,
	SYS_MKDIR,
	SYS_LINK,
	SYS_SYMLINK,
	SYS_UNLINK
} system_call_type;

typedef enum {
	TRACE_MODULE = 1,
	PROTECT_MODULE,
	ANTIVIRUS_MODULE
} module_type;

struct module_reg {
	struct module *module;
	module_type type;
	int (*sys_op) (int, ...);
};

struct linked_list_module {
	struct module_reg *module;
	struct linked_list_module *next;
};

extern struct linked_list_module *head_node;
#endif
