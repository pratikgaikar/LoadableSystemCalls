#include <linux/linkage.h>
#include <linux/moduleloader.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include "main.h"
#include "clone_header.h"

asmlinkage extern long (*sysptr)(void *input_ptr);

asmlinkage long sys_clone2(void *input_ptr)
{
	unsigned long clone_flags;
        unsigned long stack_start;
        unsigned long stack_size;
        int *parent_tidptr;
        int *child_tidptr;
        unsigned long tls, copy_return;
        int16_t module_id;
	struct my_args *input_struct = NULL;
	int error = 0;
	long pid = 0;
	struct ioctl_arg *arg = NULL;

	arg = kmalloc(sizeof(struct ioctl_arg),GFP_KERNEL);

	if(arg == NULL)
	{
		printk(KERN_INFO "\nUnable to allocate memory!");
                error = -ENOMEM;
                goto out;
	}

	if(input_ptr == NULL){
		printk(KERN_INFO "\nInput pointer to system call is NULL!");
		error = -EFAULT;
		goto out;
	}
	
	input_struct = kmalloc(sizeof(struct my_args),GFP_KERNEL);
	if(input_struct == NULL){
		printk(KERN_INFO "\nUnable to allocate memory!");
		error = -ENOMEM;
		goto out;
	}

	copy_return = copy_from_user(input_struct,input_ptr,sizeof(struct my_args));
	if(copy_return > 0){
		printk(KERN_INFO "\nUnable to copy from user space to kernel space");
		error = -EFAULT;
		goto out;
	}

	clone_flags = input_struct->clone_flags;
        stack_start = input_struct->stack_start;
        stack_size = input_struct->stack_size;
        parent_tidptr = input_struct->parent_tidptr;
        child_tidptr = input_struct->child_tidptr;
        tls = input_struct->tls;
        module_id = input_struct->module_id;

	pid = _do_fork(clone_flags, stack_start, stack_size, parent_tidptr, child_tidptr, tls);
	
	arg->pid = pid;
	arg->module_type = module_id;
	arg->vector= input_struct->vector;
	get_ioctl_helper(arg);
	
	out:
	if(input_struct)
		kfree(input_struct);
	if(error < 0)
		return error;
	else
		return 0;
}
static int __init init_sys_clone2(void)
{
	printk("INSTALLED NEW CLONE 2 MODULE\n");
	if (sysptr == NULL)
		sysptr = sys_clone2;
	return 0;
}
static void  __exit exit_sys_clone2(void)
{
	if (sysptr != NULL)
		sysptr = NULL;
	printk("REMOVED CLONE 2 MODULE\n");
}
module_init(init_sys_clone2);
module_exit(exit_sys_clone2);
MODULE_LICENSE("GPL");

