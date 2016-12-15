#include "main.h"

struct mutex Lock;
struct linked_list_module *head_node = NULL;

static int module_register(struct module_reg *module_reg)
{
	int ret = 0;
	printk("\nREGISTERING MODULE TYPE - %d", module_reg->type);
	ret = add_module(module_reg);	//adding to linked list
	return ret;
}
EXPORT_SYMBOL(module_register);

static int module_deregister(struct module_reg *module_reg)
{
	int ret = 0;
	
	printk("\nDE-REGISTERING MODULE TYPE - %d", module_reg->type);
	remove_module(module_reg->type);   //removing module
	return ret;
}
EXPORT_SYMBOL(module_deregister);

static int __init router_init(void)
{
	int ret = 0;
	//struct mutex mutexlock;

	ret = setup_ioctl_dev();
	ret = setup_system_call();
	mutex_init(&Lock);
	printk(KERN_INFO "MAIN DRIVER LOADED \n");
	return ret;
}

static void __exit router_exit(void)
{
	system_call_cleanup();
	ioctl_cleanup();
	return;
}

MODULE_LICENSE("GPL");
module_init(router_init);
module_exit(router_exit);
