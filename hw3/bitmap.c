#include "main.h"

	int
get_vector (char *ioctl_buffer)
{

	struct task_struct *ioctl_task;
	char* ptr;
	char *str_type = NULL;
	int ret = 0;

	struct linked_list_module *iterator = head_node;
	str_type = kmalloc (16, GFP_KERNEL);
	if (str_type == NULL)
	{
		ret = -ENOMEM;
		goto out;
	}
	str_type[0] = '\0';
	ptr = str_type;

	while (iterator != NULL)
	{
		strncat (ioctl_buffer, "Module:", 7);
		sprintf (str_type, "%d", iterator->module->type);
		strcat (ioctl_buffer, str_type);
		strncat (ioctl_buffer, "\n", 1);

		strncat (ioctl_buffer, "RC_Count:", 9);
		str_type = ptr;
		str_type[0] = '\0';
		sprintf (str_type, "%d", module_refcount(iterator->module->module));
		strcat (ioctl_buffer, str_type);
		strncat (ioctl_buffer, "\n", 1);


		for_each_process (ioctl_task)
		{
			struct module_reg *new_node = NULL;
			if (ioctl_task->sys_vector != NULL)
			{
				new_node =
					(struct module_reg *) ioctl_task->sys_vector->module_addr;
				if (new_node->type == iterator->module->type)
				{
					strncat (ioctl_buffer, "PID:", 4);
					str_type = ptr;
					str_type[0] = '\0';
					sprintf (str_type, "%d", ioctl_task->pid);
					strcat (ioctl_buffer, str_type);
					strncat (ioctl_buffer, ",", 1);

					strncat (ioctl_buffer, "Vector:", 7);
					str_type = ptr;
					str_type[0] = '\0';
					sprintf (str_type, "%d", ioctl_task->sys_vector->sv);
					strcat (ioctl_buffer, str_type);
					strncat (ioctl_buffer, "\n", 1);
				}
			}
		}

		iterator = iterator->next;
	}

out:
	if (str_type)
		kfree (str_type);
	return ret;
}



	int
add_module (struct module_reg *module_reg)
{
	int ret = 0;
	struct linked_list_module *new_node = NULL;
	struct linked_list_module *curr = head_node;
	struct linked_list_module *prev = NULL;
	new_node =
		(struct linked_list_module *) kmalloc (sizeof (struct linked_list_module),
				GFP_KERNEL);
	if (new_node == NULL)
	{
		printk ("\ncouldn't allocate new node");
		ret = -ENOMEM;
	}
	new_node->module = module_reg;
	new_node->next = NULL;

	if (head_node == NULL)
	{
		head_node = new_node;
	}
	else
	{
		while (curr != NULL)
		{
			prev = curr;
			curr = curr->next;
		}
		prev->next = new_node;
	}
	return ret;
}

	void
remove_module (module_type type)
{

	struct linked_list_module *prev = NULL;
	struct linked_list_module *curr = head_node;
	while (curr != NULL)
	{
		if (curr->module->type == type)
		{
			if (prev != NULL)
			{
				prev->next = curr->next;
			}
			else
			{
				head_node = head_node->next;

			}
			kfree (curr);
			return;
		}
		prev = curr;
		curr = curr->next;
	}
	return;
}

	void *
get_module (module_type type)
{
	struct linked_list_module *iterator = head_node;
	while (iterator != NULL)
	{
		if (iterator->module->type == type)
		{
			break;
		}
		iterator = iterator->next;
	}

	if (iterator != NULL)
		return iterator->module;
	else
		return NULL;
}

	void
increment_module_rc (struct module_reg *module_reg)
{
	struct linked_list_module *iterator = head_node;
	while (iterator != NULL)
	{
		if (iterator->module == module_reg)
		{
			break;
		}
		iterator = iterator->next;
	}
	if (iterator != NULL)
	{
		__module_get(iterator->module->module);
	}
}

	void
decrement_module_rc (struct module_reg *module_reg)
{
	struct linked_list_module *iterator = head_node;
	while (iterator != NULL)
	{
		if (iterator->module == module_reg)
		{
			break;
		}
		iterator = iterator->next;
	}
	if (iterator != NULL)
	{
		if(module_refcount(iterator->module->module)>0)
		{
			module_put(iterator->module->module);
		}
	}
}
