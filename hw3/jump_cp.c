#include "main.h"

int read_file(struct file* file, char *data, int size) {
        int ret;
        mm_segment_t oldfs;
        oldfs = get_fs();
        set_fs(KERNEL_DS);
        ret = vfs_read(file, data, size,&file->f_pos);
        set_fs(oldfs);
        return ret;
}

int write_file(struct file *file,const char *data)
{
	mm_segment_t oldfs;
        int ret=0;
        oldfs = get_fs();
        set_fs(KERNEL_DS);
        vfs_write(file, data ,strlen(data), &file->f_pos);
        set_fs(oldfs);
}

char *search(const char *str)
{
	char *buffer1 = NULL, *parse = NULL;
	int read_bytes = 0;
	struct file *fp = NULL;
        char *b1 = NULL;
	buffer1 = kmalloc(PAGE_SIZE,GFP_KERNEL);
	b1 = buffer1;
	fp = filp_open("/usr/src/cddb.txt", O_RDWR | O_CREAT | O_APPEND , 0644);
        if (!fp  || IS_ERR(fp)) {
                 fp = NULL;
                 goto out;
        }
	read_bytes = read_file(fp, buffer1, PAGE_SIZE);
	while(buffer1!=NULL && strlen(buffer1)!=0)
	{
		parse = strsep(&buffer1,"\n");
		if(parse!=NULL)
		{
			if(strstr(parse,str)!= NULL)
			{
				break;
			}
		}
		parse = NULL;	
	}
	if(parse !=NULL)
		printk("\nFound  %s", parse);
	if(parse == NULL)
		write_file(fp,str);
out:
	buffer1 = b1;
	if(buffer1)
		kfree(buffer1);
	if(fp)
        	filp_close(fp,NULL);
	
	return parse;		
}
