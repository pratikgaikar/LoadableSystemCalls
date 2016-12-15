#include "main.h"

asmlinkage long (*original_write) (unsigned int, const char __user *, size_t);
asmlinkage long (*original_read) (unsigned int, char __user *, size_t);
asmlinkage long (*original_open) (const char __user *, int, umode_t);
asmlinkage long (*original_rmdir) (const char __user * filename);
asmlinkage long (*original_link) (const char __user * oldname,
		const char __user * newname);
asmlinkage long (*original_symlink) (const char __user * old,
		const char __user * new);
asmlinkage long (*original_unlink) (const char __user * pathname);
asmlinkage long (*original_rename) (const char __user * oldname,
		const char __user * newname);
asmlinkage long (*original_mkdir) (const char __user * pathname, umode_t mode);
asmlinkage long (*original_exit_group) (int error_code);
asmlinkage long (*original_close)(unsigned int fd);

bool op_allowed(system_call_type type, int sv){

	if((sv & (1 << type)) != 0){
		return true;
	}

	return false;	
}

asmlinkage long new_close(unsigned int fd)
{
	struct module_reg *new_node = NULL;
	int ret = 0;

	if (current->sys_vector != NULL) {
		if(!op_allowed(SYS_CLOSE,current->sys_vector->sv)){
			printk("CLOSE NA\n");
			return -EOPNOTSUPP; 	
		}
		printk("CLOSE\n");
		new_node = (struct module_reg *)current->sys_vector->module_addr;
		ret = new_node->sys_op(2, SYS_CLOSE, fd);		
		if(ret < 0)
                        return ret;

	}
	return original_close(fd);
}

asmlinkage long new_exit_group(int error_code)
{
	if (current->sys_vector != NULL) {
		struct module_reg *new_node = NULL;
		new_node = (struct module_reg *)current->sys_vector->module_addr;
		decrement_module_rc(new_node);
		kfree(current->sys_vector);
		current->sys_vector = NULL;

	}
	return original_exit_group(error_code);
}


asmlinkage long new_link(const char __user * oldname,
		const char __user * newname)
{
	struct module_reg *new_node = NULL;
	int ret = 0;

	if (current->sys_vector != NULL) {

		if(!op_allowed(SYS_LINK,current->sys_vector->sv)){
			printk("LINK NA\n");
			return -EOPNOTSUPP;
		}
		printk("LINK\n");
		new_node = (struct module_reg *)current->sys_vector->module_addr;
		ret = new_node->sys_op(3, SYS_LINK, oldname, newname);
		if(ret < 0)
                        return ret;

	}
	return original_link(oldname, newname);
}

asmlinkage long new_symlink(const char __user * old, const char __user * new)
{
	struct module_reg *new_node = NULL;
	int ret = 0;

	if (current->sys_vector != NULL) {
		if(!op_allowed(SYS_SYMLINK,current->sys_vector->sv)){
			printk("SYMLINK NA\n");
			return -EOPNOTSUPP;
		}
		printk("SYMLINK\n");

		new_node = (struct module_reg *)current->sys_vector->module_addr;
		ret = new_node->sys_op(3, SYS_SYMLINK, old, new);
		if(ret < 0)
                        return ret;

	}
	return original_symlink(old, new);
}

asmlinkage long new_unlink(const char __user * pathname)
{
	struct module_reg *new_node = NULL;
	int ret = 0;

	if (current->sys_vector != NULL) {
		if(!op_allowed(SYS_UNLINK,current->sys_vector->sv)){
			printk("UNLINK NA\n");
			return -EOPNOTSUPP;
		}
		printk("UNLINK\n");
		new_node =(struct module_reg *)current->sys_vector->module_addr;
		ret = new_node->sys_op(2, SYS_UNLINK, pathname);
		if(ret < 0)
                        return ret;

	}
	return original_unlink(pathname);
}

asmlinkage long new_rename(const char __user * oldname,
		const char __user * newname)
{
	struct module_reg *new_node = NULL;
	int ret = 0;

	if (current->sys_vector != NULL) {
		if(!op_allowed(SYS_RENAME,current->sys_vector->sv)){
			printk("RENAME NA\n");
			return -EOPNOTSUPP;
		}
		printk("RENAME\n");
		new_node =(struct module_reg *)current->sys_vector->module_addr;
		ret = new_node->sys_op(3, SYS_RENAME, oldname, newname);
		if(ret < 0)
                        return ret;

	}
	return original_rename(oldname, newname);
}

asmlinkage long new_mkdir(const char __user * pathname, umode_t mode)
{
	struct module_reg *new_node = NULL;
	int ret = 0;	

	if (current->sys_vector != NULL) {
		if(!op_allowed(SYS_MKDIR,current->sys_vector->sv)){
			printk("MKDIR NA\n");
			return -EOPNOTSUPP;
		}
		printk("MKDIR\n");
		new_node =(struct module_reg *)current->sys_vector->module_addr;
		ret = new_node->sys_op(3, SYS_MKDIR, pathname, mode);
		if(ret < 0)
                        return ret;

	}
	return original_mkdir(pathname, mode);
}

asmlinkage long new_write(unsigned int x, const char __user * y, size_t size)
{
	struct module_reg *new_node = NULL;
	int ret = 0;

	if (current->sys_vector != NULL) {

		if(!op_allowed(SYS_WRITE,current->sys_vector->sv)){
			printk("WRITE NA\n");
			return -EOPNOTSUPP;
		}
		printk("WRITE\n");
		new_node =(struct module_reg *)current->sys_vector->module_addr;
		ret = new_node->sys_op(3, SYS_WRITE, x, y, size);
		if(ret < 0)
                        return ret;

	}
	return original_write(x, y, size);
}

asmlinkage long new_open(const char __user * path, int flags, umode_t mode)
{
	struct module_reg *new_node = NULL;
	int ret = 0;

	if (current->sys_vector != NULL) {
		if(!op_allowed(SYS_OPEN,current->sys_vector->sv)){
			printk("OPEN NA\n");
			return -EOPNOTSUPP;
		}
		printk("OPEN\n");
		new_node =(struct module_reg *)current->sys_vector->module_addr;
		ret = new_node->sys_op(4, SYS_OPEN, path, flags, mode);
		if(ret < 0)
                        return ret;

	}
	return original_open(path, flags, mode);
}

asmlinkage long new_read(unsigned int fd, char __user * path_name, size_t size)
{
	if (current->sys_vector != NULL) {
		struct module_reg *new_node = NULL;
		new_node =
			(struct module_reg *)current->sys_vector->module_addr;
	}
	return original_read(fd, path_name, size);
}

asmlinkage long new_rmdir(const char __user * filename)
{
	struct module_reg *new_node = NULL;
	int ret = 0;

	if (current->sys_vector != NULL) {
		if(!op_allowed(SYS_RMDIR,current->sys_vector->sv)){
			printk("RMDIR NA\n");
			return -EOPNOTSUPP;

		}
		printk("RMDIR\n");
		new_node = (struct module_reg *)current->sys_vector->module_addr;
		ret = new_node->sys_op(2, SYS_RMDIR, filename);
		if(ret < 0)
			return ret;
	}
	return original_rmdir(filename);
}

int set_systems_calls(unsigned long *syscall_table)
{
	int ret = 0;
	original_write = (void *)syscall_table[__NR_write];
	original_open = (void *)syscall_table[__NR_open];
	original_rmdir = (void *)syscall_table[__NR_rmdir];
	original_link = (void *)syscall_table[__NR_link];
	original_unlink = (void *)syscall_table[__NR_unlink];
	original_symlink = (void *)syscall_table[__NR_symlink];
	original_rename = (void *)syscall_table[__NR_rename];
	original_mkdir = (void *)syscall_table[__NR_mkdir];
	original_exit_group = (void *)syscall_table[__NR_exit_group];
	original_close = (void *)syscall_table[__NR_close];

	syscall_table[__NR_open] = (unsigned long)&new_open;
	syscall_table[__NR_write] = (unsigned long)&new_write;
	syscall_table[__NR_rmdir] = (unsigned long)&new_rmdir;
	syscall_table[__NR_link] = (unsigned long)&new_link;
	syscall_table[__NR_unlink] = (unsigned long)&new_unlink;
	syscall_table[__NR_symlink] = (unsigned long)&new_symlink;
	syscall_table[__NR_rename] = (unsigned long)&new_rename;
	syscall_table[__NR_mkdir] = (unsigned long)&new_mkdir;
	syscall_table[__NR_exit_group] = (unsigned long)&new_exit_group;
	syscall_table[__NR_close] = (unsigned long)&new_close;

	return ret;
}

int destory_new_system_calls(unsigned long *syscall_table)
{
	int ret = 0;
	syscall_table[__NR_open] = (unsigned long)original_open;
	syscall_table[__NR_write] = (unsigned long)original_write;
	syscall_table[__NR_rmdir] = (unsigned long)original_rmdir;
	syscall_table[__NR_link] = (unsigned long)original_link;
	syscall_table[__NR_unlink] = (unsigned long)original_unlink;
	syscall_table[__NR_symlink] = (unsigned long)original_symlink;
	syscall_table[__NR_rename] = (unsigned long)original_rename;
	syscall_table[__NR_mkdir] = (unsigned long)original_mkdir;
	syscall_table[__NR_exit_group] = (unsigned long)original_exit_group;
	syscall_table[__NR_close] = (unsigned long)original_close;
	return ret;
}
