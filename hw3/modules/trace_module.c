#include "../module.h"
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/statfs.h>
#include <linux/fs_stack.h>
#include <linux/uaccess.h>
#include <linux/exportfs.h>

extern int module_register(struct module_reg *module_reg);
extern int module_deregister(struct module_reg *module_reg);
char* write_file_buff = NULL;
struct file *filp_out;
void write_to_file(struct file* myfile, char* buf, int size);
int vacant = PAGE_SIZE;

long trace_open(const char __user * path, int flags, umode_t mode)
{
	char* record = (char*)kmalloc(sizeof(char)*PAGE_SIZE,GFP_KERNEL);
        if(record != NULL){
                record[0] = '\0';
                sprintf(record, "PID: %d, TRACE OPERATION: OPEN, FILE/DIR: %s\n",current->pid, path);
                if(vacant <= strlen(record)){
                        write_to_file(filp_out,write_file_buff,strlen(write_file_buff));
                        vacant = PAGE_SIZE;
                        write_file_buff[0] = '\0';
                }
                strncpy(write_file_buff + PAGE_SIZE - vacant, record,strlen(record));
                vacant -= strlen(record);
                kfree(record);
        }
	return 0;
}

long trace_read(unsigned int fd,const char __user * path_name, size_t size)
{
	char* record = (char*)kmalloc(sizeof(char)*PAGE_SIZE,GFP_KERNEL);
        if(record != NULL){
                record[0] = '\0';
                sprintf(record, "PID: %d, TRACE OPERATION: READ, FILE/DIR: %s\n",current->pid, path_name);
                //printk("\n FINAL Record- %s",record);
                if(vacant <= strlen(record)){
                        //printk("WRITING TO BUFFER %s\n", write_file_buff);
                        write_to_file(filp_out,write_file_buff,strlen(write_file_buff));
                        vacant = PAGE_SIZE;
                        write_file_buff[0] = '\0';
                }

                strncpy(write_file_buff + PAGE_SIZE - vacant, record,strlen(record));
                vacant -= strlen(record);
                kfree(record);
        }

	return 0;
}

long trace_close(unsigned int fd)
{
	char* record = (char*)kmalloc(sizeof(char)*PAGE_SIZE,GFP_KERNEL);
        if(record != NULL){
                record[0] = '\0';
                sprintf(record, "PID: %d, TRACE OPERATION: CLOSE\n",current->pid);
                //printk("\n FINAL Record- %s",record);
                if(vacant <= strlen(record)){
                        //printk("WRITING TO BUFFER %s\n", write_file_buff);
                        write_to_file(filp_out,write_file_buff,strlen(write_file_buff));
                        vacant = PAGE_SIZE;
                        write_file_buff[0] = '\0';
                }

                strncpy(write_file_buff + PAGE_SIZE - vacant, record,strlen(record));
                vacant -= strlen(record);
                kfree(record);
        }

	return 0;
}

long trace_write(unsigned int fd, const char __user * path_name, size_t size){
	char* record = (char*)kmalloc(sizeof(char)*PAGE_SIZE,GFP_KERNEL);
        if(record != NULL){
                record[0] = '\0';
                sprintf(record, "PID: %d, TRACE OPERATION: WRITE, FILE/DIR: %s\n",current->pid, path_name);
                if(vacant <= strlen(record)){
                        write_to_file(filp_out,write_file_buff,strlen(write_file_buff));
                        vacant = PAGE_SIZE;
                        write_file_buff[0] = '\0';
                }

                strncpy(write_file_buff + PAGE_SIZE - vacant, record,strlen(record));
                vacant -= strlen(record);
                kfree(record);
        }

	return 0;
}

long trace_rmdir(const char __user *filename) 
{
	char* record = (char*)kmalloc(sizeof(char)*PAGE_SIZE,GFP_KERNEL);
	if(record != NULL){
                record[0] = '\0';
                sprintf(record, "PID: %d, TRACE OPERATION: RMDIR, FILE/DIR: %s\n",current->pid, filename);
		printk("\n FINAL Record- %s",record);
                if(vacant <= strlen(record)){
                        write_to_file(filp_out,write_file_buff,strlen(write_file_buff));
                        vacant = PAGE_SIZE;
                        write_file_buff[0] = '\0';
                }

                strncpy(write_file_buff + PAGE_SIZE - vacant, record,strlen(record));
                vacant -= strlen(record);
		kfree(record);
        }
        
	return 0;
}

long  trace_link(const char __user *oldname,const char __user *newname){
	char* record = (char*)kmalloc(sizeof(char)*PAGE_SIZE,GFP_KERNEL);
	if(record != NULL){
                record[0] = '\0';
                sprintf(record, "PID: %d, TRACE OPERATION: LINK, OLD FILE/DIR: %s, NEW FILE/DIR: %s\n",current->pid, oldname, newname);
                if(vacant <= strlen(record)){
                        write_to_file(filp_out,write_file_buff,strlen(write_file_buff));
                        vacant = PAGE_SIZE;
                        write_file_buff[0] = '\0';
                }

                strncpy(write_file_buff + PAGE_SIZE - vacant, record,strlen(record));
                vacant -= strlen(record);
        	kfree(record);
	}
        
	return 0;
}

long  trace_symlink(const char __user *old, const char __user *new){
	char* record = (char*)kmalloc(sizeof(char)*PAGE_SIZE,GFP_KERNEL);
	if(record != NULL){
                record[0] = '\0';
                sprintf(record, "PID: %d, TRACE OPERATION: SYMLINK, OLD FILE/DIR: %s, NEW FILE/DIR: %s\n",current->pid, old, new);
                if(vacant <= strlen(record)){
                        write_to_file(filp_out,write_file_buff,strlen(write_file_buff));
                        vacant = PAGE_SIZE;
                        write_file_buff[0] = '\0';
                }

                strncpy(write_file_buff + PAGE_SIZE - vacant, record,strlen(record));
                vacant -= strlen(record);
        	kfree(record);
	}
        
	return 0;
}

long  trace_unlink(const char __user *pathname){
	char* record = (char*)kmalloc(sizeof(char)*PAGE_SIZE,GFP_KERNEL);
        if(record != NULL){
                record[0] = '\0';
                sprintf(record, "PID: %d, TRACE OPERATION: UNLINK, FILE/DIR: %s\n",current->pid, pathname);
                if(vacant <= strlen(record)){
                        write_to_file(filp_out,write_file_buff,strlen(write_file_buff));
                        vacant = PAGE_SIZE;
                        write_file_buff[0] = '\0';
                }

                strncpy(write_file_buff + PAGE_SIZE - vacant, record,strlen(record));
                vacant -= strlen(record);
        	kfree(record);
	}
        
	return 0;
}

long  trace_rename(const char __user *oldname, const char __user *newname){
	char* record = (char*)kmalloc(sizeof(char)*PAGE_SIZE,GFP_KERNEL);
	if(record != NULL){
                record[0] = '\0';
                sprintf(record, "PID: %d, TRACE OPERATION: RENAME, OLD FILE/DIR: %s, NEW FILE/DIR: %s\n",current->pid, oldname, newname);
                if(vacant <= strlen(record)){
                        write_to_file(filp_out,write_file_buff,strlen(write_file_buff));
                        vacant = PAGE_SIZE;
                        write_file_buff[0] = '\0';
                }

                strncpy(write_file_buff + PAGE_SIZE - vacant, record,strlen(record));
                vacant -= strlen(record);
		kfree(record);
        }
	return 0;
}

long  trace_mkdir(const char __user *pathname, umode_t mode){
	char* record = (char*)kmalloc(sizeof(char)*PAGE_SIZE,GFP_KERNEL);
        if(record != NULL){
                record[0] = '\0';
                sprintf(record, "PID: %d, TRACE OPERATION: MKDIR, OLD FILE/DIR: %s\n",current->pid, pathname);
                //printk("\n FINAL Record- %s",record);
                if(vacant <= strlen(record)){
                        //printk("\nWRITING TO BUFFER %s", write_file_buff);
                        write_to_file(filp_out,write_file_buff,strlen(write_file_buff));
                        vacant = PAGE_SIZE;
                        write_file_buff[0] = '\0';
                }

                strncpy(write_file_buff + PAGE_SIZE - vacant, record,strlen(record));
                vacant -= strlen(record);
		kfree(record);
        }
        
	return 0;
}

/*System call entry point */
int system_call_entry(int num, ...)
{
	va_list arguments;
	int arg1,arg2;
	const char *path_name;
	va_start (arguments, num);
	switch(va_arg(arguments, system_call_type))
	{
		case SYS_OPEN:
			path_name = va_arg(arguments, const char *);
			arg1 = va_arg(arguments,int);
			arg2 = va_arg(arguments,int);
			trace_open(path_name,arg1,arg2);
			break;

		case SYS_CLOSE:
			trace_close(va_arg(arguments,int));
			break;

		case SYS_READ:
			arg1 =  va_arg(arguments,int);
                        path_name = va_arg(arguments, const char *);
                        arg2 = va_arg(arguments,int);
			trace_read(arg1,path_name,arg2);
			break;

		case SYS_WRITE:
			arg1 =  va_arg(arguments,int);
			path_name = va_arg(arguments, const char *);
			arg2 = va_arg(arguments,int);
			trace_write(arg1,path_name,arg2);
			break;

		case SYS_RENAME:
			trace_rename(va_arg(arguments,const char *),va_arg(arguments,const char *));
			break;

		case SYS_RMDIR:
			trace_rmdir(va_arg(arguments,const char *));
			break;

		case SYS_MKDIR:
			path_name = va_arg(arguments, const char *);
			arg1 = va_arg(arguments,int);
			trace_mkdir(path_name,arg1);
			break;

		case SYS_SYMLINK:
			trace_symlink(va_arg(arguments,const char *),va_arg(arguments,const char *));
			break;

		case SYS_LINK:
			 trace_link(va_arg(arguments,const char *),va_arg(arguments,const char *));
			break;
		case SYS_UNLINK:
			trace_unlink(va_arg(arguments,const char *));
			break;

	}
	va_end(arguments);
	return 0;
}

struct module_reg trace = {
	.type =  TRACE_MODULE,
	.sys_op = system_call_entry,
	.module = THIS_MODULE
};

static int __init trace_init(void){
	module_register(&trace);
	filp_out  = filp_open("/tmp/trace_file.txt", O_WRONLY|O_CREAT, 0644);
        write_file_buff = (char*)kmalloc(sizeof(char)*PAGE_SIZE,GFP_KERNEL);
        vacant = PAGE_SIZE;
	printk(KERN_INFO "TRACE MODULE LOADED\n");
	return 0;
}

static void __exit trace_exit(void)
{		
	if(write_file_buff && filp_out && vacant < PAGE_SIZE){
		 write_to_file(filp_out,write_file_buff,strlen(write_file_buff));		
	}
	module_deregister(&trace);
	if(write_file_buff)
                kfree(write_file_buff);
        if(filp_out)
                filp_close(filp_out, NULL);
	printk(KERN_INFO "TRACE MODULE UNLOADED\n");
}

void write_to_file(struct file* myfile, char* buf, int size){
        mm_segment_t oldfs;
        oldfs = get_fs();
        set_fs(KERNEL_DS);
        //printk("\n Buffer%s",buf);
        vfs_write(myfile, buf, size, &myfile->f_pos);
        set_fs(oldfs);
}

MODULE_LICENSE("GPL");

module_init(trace_init);
module_exit(trace_exit);
