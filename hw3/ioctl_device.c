#include "main.h"
static unsigned int major = 0;
static struct ioctl_dev *ioctl_devices = NULL;
static struct class *ioctl_class = NULL;

void get_ioctl_helper(struct ioctl_arg *msg){
	struct task_struct *task = NULL;
        struct pid *pid_struct;
	void *addr;
	pid_struct = find_get_pid(msg->pid);
        task = pid_task(pid_struct, PIDTYPE_PID);

	if (msg->vector == 0) {
		if(task == NULL || task->sys_vector == NULL)
			return;
		decrement_module_rc(((struct module_reg *)task->sys_vector->module_addr));
		kfree(task->sys_vector);
		task->sys_vector = NULL;
		return;
	}
	addr = get_module(msg->module_type);
	if (addr != NULL) {
		if (task == NULL) {
			//printk("\nProcess not found with PID - %d",msg->pid);
			return;
		} else {
			//printk("\nProcess found with PID %d", msg->pid);
		}

		if (task->sys_vector == NULL) {
			task->sys_vector =
				(struct new_sys_vector *)
				kmalloc(sizeof(struct new_sys_vector),
						GFP_KERNEL);
			task->sys_vector->module_addr = addr;
			increment_module_rc((struct module_reg *)addr);
		} else if (task->sys_vector->module_addr != addr) {
			decrement_module_rc((struct module_reg *)task->sys_vector->module_addr);
			increment_module_rc((struct module_reg *)addr);
		}

		task->sys_vector->module_addr = addr;
		task->sys_vector->sv = msg->vector;
	} else {
		printk("\nNo module found.");
	}
}
EXPORT_SYMBOL(get_ioctl_helper);

long ioctl_funcs(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	int io_ret = 0;
	struct ioctl_arg *msg = NULL;
	char *ioctl_buffer = NULL;

	mutex_lock(&Lock);	

	switch (cmd) {
		case IOCTL_SET:
			msg = kmalloc(sizeof(struct ioctl_arg), GFP_KERNEL);
			if (msg == NULL) {
				ret = -ENOMEM;
				goto out;
			}

			if (copy_from_user(msg, (void*)arg, sizeof(struct ioctl_arg))) {
				ret = -EFAULT;
				goto out;
			}
			get_ioctl_helper(msg);
			break;

		case IOCTL_GET:

			ioctl_buffer = kmalloc(PAGE_SIZE, GFP_KERNEL);
			if (ioctl_buffer == NULL) {
				ret = -ENOMEM;
				goto out;
			}
			io_ret = get_vector(ioctl_buffer);
			if(copy_to_user((void*)arg, ioctl_buffer, strlen(ioctl_buffer)))
			{
				ret = -EFAULT;
				goto out;
			}

			if (io_ret < 0)
				printk("GET VECTOR HAS ISSUES\n");
			break;
	}
out:

	mutex_unlock(&Lock);

	if (msg != NULL)
		kfree(msg);
	if (ioctl_buffer != NULL)
		kfree(ioctl_buffer);
	return ret;
}

struct file_operations ioctl_fops = {
	.unlocked_ioctl = ioctl_funcs,
};

static int ioctl_construct_device(struct ioctl_dev *dev, int minor,
		struct class *class)
{
	int err = 0;
	dev_t devno = MKDEV(major, minor);
	struct device *device = NULL;

	BUG_ON(dev == NULL || class == NULL);

	/* Memory is to be allocated when the device is opened the first time */
	mutex_init(&dev->ioctl_mutex);

	cdev_init(&dev->cdev, &ioctl_fops);
	dev->cdev.owner = THIS_MODULE;

	err = cdev_add(&dev->cdev, devno, 1);
	if (err) {
		printk(KERN_WARNING
				"[target] Error %d while trying to add %s%d", err,
				DEVICE_NAME, minor);
		return err;
	}

	device = device_create(class, NULL,	/* no parent device */
			devno, NULL,	/* no additional data */
			DEVICE_NAME "%d", minor);

	if (IS_ERR(device)) {
		err = PTR_ERR(device);
		printk(KERN_WARNING
				"[target] Error %d while trying to create %s%d", err,
				DEVICE_NAME, minor);
		cdev_del(&dev->cdev);
		return err;
	}
	return 0;
}

int setup_ioctl_dev()
{
	int err;
	dev_t dev = 0;

	/* Get a range of minor numbers (starting with 0) to work with */
	err = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
	if (err < 0) {
		printk(KERN_WARNING "[target] alloc_chrdev_region() failed\n");
		return err;
	}
	major = MAJOR(dev);

	/* Create device class (before allocation of the array of devices) */
	ioctl_class = class_create(THIS_MODULE, DEVICE_NAME);
	if (IS_ERR(ioctl_class)) {
		err = PTR_ERR(ioctl_class);
		goto out;
	}

	/* Allocate the array of devices */
	ioctl_devices = (struct ioctl_dev *)kzalloc(sizeof(struct ioctl_dev),
			GFP_KERNEL);
	if (ioctl_devices == NULL) {
		err = -ENOMEM;
		goto out;
	}

	/* Construct devices */
	err = ioctl_construct_device(&ioctl_devices[0], 0, ioctl_class);
	if (err) {
		goto out;
	}

out:

	return err;
}

static void ioctl_destroy_device(struct ioctl_dev *dev, int minor,
		struct class *class)
{
	BUG_ON(dev == NULL || class == NULL);
	device_destroy(class, MKDEV(major, minor));
	cdev_del(&dev->cdev);
	mutex_destroy(&dev->ioctl_mutex);
	return;
}

void ioctl_cleanup()
{
	/* Get rid of character devices (if any exist) */
	if (ioctl_devices) {
		ioctl_destroy_device(&ioctl_devices[0], 0, ioctl_class);
		kfree(ioctl_devices);
	}

	if (ioctl_class)
		class_destroy(ioctl_class);

	/* [NB] ioctl_cleanup_module is never called if alloc_chrdev_region()
	 *         * has failed. */
	unregister_chrdev_region(MKDEV(major, 0), 1);
	return;
}
