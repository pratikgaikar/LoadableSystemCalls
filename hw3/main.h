#ifndef _MAIN_H_
#define _MAIN_H_
#include <linux/cdev.h>
#include <linux/unistd.h>
#include <linux/fs.h>
#include <asm/paravirt.h>
#include <asm/uaccess.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/version.h>
#include <linux/moduleparam.h>
#include <linux/err.h>
#include <linux/device.h>
#include <linux/mutex.h>
#include <asm/uaccess.h>
#include "module.h"
#include "clone_header.h"

#define  DEVICE_NAME "ioctl"
#define PROC_V    "/proc/version"
#define BOOT_PATH "/boot/System.map-"
#define MAX_VERSION_LEN   256

#define IOCTL_SET 0
#define IOCTL_GET 1
struct ioctl_dev {
	struct mutex ioctl_mutex;
	struct cdev cdev;
};

extern unsigned long *syscall_table;
int set_systems_calls(unsigned long *syscall_table);
int destory_new_system_calls(unsigned long *syscall_table);
int get_vector(char *);
char *search(const char *);
int setup_system_call(void);
int setup_ioctl_dev(void);
void ioctl_cleanup(void);
extern struct mutex Lock;
void system_call_cleanup(void);
struct new_sys_vector *add_node(int sv);
int delete_node(u32 sv);
int add_module(struct module_reg *module_reg);
void remove_module(module_type type);
void *get_module(module_type type);
void decrement_module_rc(struct module_reg *module_reg);
void increment_module_rc(struct module_reg *module_reg);
void get_ioctl_helper(struct ioctl_arg *msg);
#endif
