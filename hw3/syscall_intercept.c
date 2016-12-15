#include "main.h"

unsigned long *syscall_table = NULL;

/*Get kernel version */
char *acquire_kernel_version (char *buf) {
	struct file *proc_version = NULL;
	char *kernel_version = NULL;
	mm_segment_t oldfs;
	oldfs = get_fs();
	set_fs (KERNEL_DS);

	proc_version = filp_open(PROC_V, O_RDONLY, 0);
	if (IS_ERR(proc_version) || (proc_version == NULL)) {
		return NULL;
	}
	vfs_read(proc_version, buf, MAX_VERSION_LEN, &(proc_version->f_pos));
	kernel_version = strsep(&buf, " ");
	kernel_version = strsep(&buf, " ");
	kernel_version = strsep(&buf, " ");

	if(proc_version != NULL) {
		filp_close(proc_version, 0);
	}
	set_fs(oldfs);
	//printk("Kernel version: %s\n", kernel_version);
	return kernel_version;
}

int find_sys_call_table (char *kern_ver)
{
	unsigned long temp = 0;
	char *system_map_entry = NULL;
	int i = 0, ret = 0, ret_kstrtoul=0;
	char *filename = NULL;
	size_t filename_length = strlen(kern_ver) + strlen(BOOT_PATH) + 1;
	struct file *f = NULL;
	mm_segment_t oldfs;
	oldfs = get_fs();
	set_fs (KERNEL_DS);

	if(kern_ver == NULL) {
		printk("acquiring kernel version failed\n");
		ret = -1;
		goto out;
	}

	filename = kzalloc(filename_length, GFP_KERNEL);
	if (filename == NULL) {
		printk("kmalloc failed on System.map-<version> filename allocation\n");
		ret = -1;
		goto out;
	}
	filename[0] = '\0';
	strncpy(filename, BOOT_PATH, strlen(BOOT_PATH));
	strncat(filename, kern_ver, strlen(kern_ver));

	f = filp_open(filename, O_RDONLY, 0);
	if (IS_ERR(f) || (f == NULL)) {
		printk("Error opening System.map-<version> file: %s\n", filename);
		ret = -1;
		goto out;
	}

	system_map_entry = kzalloc(MAX_VERSION_LEN, GFP_KERNEL);
	if (system_map_entry == NULL) {
		printk("kmalloc failed on System.map-<version> map entry allocation\n");
		ret = -1;
		goto out;
	}
	system_map_entry[0] = '\0';

	while (vfs_read(f, system_map_entry + i, 1, &f->f_pos) == 1) {
		if ( system_map_entry[i] == '\n' || i == MAX_VERSION_LEN ) {
			i = 0;
			if (strstr(system_map_entry, "sys_call_table") != NULL) {
				char *sys_string;
				char *system_map_entry_ptr = system_map_entry;

				sys_string = kzalloc(MAX_VERSION_LEN, GFP_KERNEL);
				if (sys_string == NULL) {
					printk("kmalloc failed on sys_string allocation\n");
					ret = -1;
					goto out;
				}
				sys_string[0] = '\0';
				strncpy(sys_string, strsep(&system_map_entry_ptr, " "), MAX_VERSION_LEN);

				ret_kstrtoul=kstrtoul(sys_string, 16, &temp);
				syscall_table = (unsigned long *) temp;
				//printk("syscall_table retrieved\n");

				kfree(sys_string);
				break;
			}

			system_map_entry[0] = '\0';
			continue;
		}
		i++;
	}

out:    if(f != NULL) {
		filp_close(f, 0);
	}
	if(filename != NULL) {
		kfree(filename);
	}
	if(system_map_entry != NULL) {
		kfree(system_map_entry);
	}
	set_fs(oldfs);
	return ret;
}

int setup_system_call()
{
	int ret =0;
	char *kernel_version = NULL;
	int syscall_table_success = 0;
	kernel_version = kzalloc(MAX_VERSION_LEN, GFP_KERNEL);
	if (kernel_version == NULL) {
		ret = -ENOMEM;
		goto out;
	}

	kernel_version[0] = '\0';
	syscall_table_success = find_sys_call_table(acquire_kernel_version(kernel_version));
	if(syscall_table_success == -1) {
		printk("syscall table address retrieval failed:\n");
		goto out;
	}
	else {
		if (syscall_table != NULL) {
			write_cr0(read_cr0() & (~0x10000));
			set_systems_calls(syscall_table);
			write_cr0(read_cr0() | 0x10000);
			//printk("sys_call_table hooked successfully\n");
		}
		else {
			printk("syscall_table is NULL\n");
		}
	}
out:
	if(kernel_version != NULL) {
		kfree(kernel_version);
	}
	return  ret;
}

void system_call_cleanup()
{
	if (syscall_table != NULL) {
		write_cr0(read_cr0() & (~0x10000));
		destory_new_system_calls(syscall_table);
		write_cr0(read_cr0() | 0x10000);
		//printk("sys_call_table unhooked successfully\n");
	}
	else {
		printk("syscall_table is NULL\n");
	}
}
