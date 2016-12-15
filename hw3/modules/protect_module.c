#include "../module.h"

extern int module_register(struct module_reg *module_reg);
extern int module_deregister(struct module_reg *module_reg);

long protect_open(const char __user * path, int flags, umode_t mode)
{
        if(strstr(path,".protected")!=NULL)
        {
                printk("\nFOUND PROTECTED FILE CANT OPEN\n");
                return -EPERM;
        }
        else
                return 0;

}

long protect_unlink(const char __user *pathname)
{
        if(strstr(pathname,".protected")!=NULL)
        {
                printk("\nFOUND PROTECTED CANT UNLINK\n");
                return -EPERM;
        }
        else
                return 0;
}

long  protect_symlink(const char __user *old, const char __user *new)
{
        if(strstr(old,".protected")!=NULL)
        {
		printk("\nFOUND PROTECTED CANT SYMLINK\n");
                return -EPERM;
        }
        else
                return 0;
}

long  protect_link(const char __user *oldname,const char __user *newname)
{
        if(strstr(oldname,".protected")!=NULL)
        {
                printk("\nFOUND PROTECTED CANT LINK\n");
		return -EPERM;
        }
        else
                return 0;

}

long protect_write(unsigned int fd, const char __user * path_name, size_t size){

        if(strstr(path_name,".protected")!=NULL)
        {
                printk("\nFOUND PROTECTED CANT WRITE\n");
		return -EPERM;
        }
        else
                return 0;
}

long  protected_rename(const char __user *oldname, const char __user *newname){
        
	if(strstr(oldname,".protected")!=NULL)
        {
                printk("\nFOUND PROTECTED CANT  RENAME\n");
                return -EPERM;
        }
        else
                return 0;
}

long protect_rmdir(const char __user *filename)
{
	if(strstr(filename,".protected")!=NULL)
        {
                printk("\nFOUND PROTECTED CANT REMOVE DIR\n");
                return -EPERM;
        }
        else
                return 0;

}

int system_call_entry(int num, ...)
{
        int ret=0;
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
                        ret=protect_open(path_name,arg1,arg2);
                        break;

                case SYS_WRITE:
                        arg1 =  va_arg(arguments,int);
                        path_name = va_arg(arguments, const char *);
                        arg2 = va_arg(arguments,int);
                        ret = protect_write(arg1,path_name,arg2);
                        break;

                case SYS_RMDIR:
                        ret = protect_rmdir(va_arg(arguments,const char *));
                        break;

                case SYS_UNLINK:
                        ret = protect_unlink(va_arg(arguments,const char *));
                        break;

                case SYS_SYMLINK:
                        ret = protect_symlink(va_arg(arguments,const char *),va_arg(arguments,const char *));
                        break;

                case SYS_LINK:
                         ret = protect_link(va_arg(arguments,const char *),va_arg(arguments,const char *));
                        break;
		case SYS_RENAME:
			ret = protected_rename(va_arg(arguments,const char *),va_arg(arguments,const char *));
			break;
                default:
                        break;
        }
        return ret;
}

struct module_reg protect = {
	.type =  PROTECT_MODULE,
	.sys_op = system_call_entry,
	.module = THIS_MODULE
};

static int __init protect_init(void)
{
	module_register(&protect);
	printk(KERN_INFO "PROTECTION MODULE LOADED\n");
	return 0;
}

static void __exit protect_exit(void)
{
	module_deregister(&protect);
	printk(KERN_INFO "PROTECTION MODULE UNLOADED\n");
}

MODULE_LICENSE("GPL");

module_init(protect_init);
module_exit(protect_exit);
