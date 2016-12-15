#include "../module.h"
#include <linux/module.h>       /* Needed by all modules */
#include <linux/kernel.h>       /* Needed for KERN_INFO */
#include <linux/init.h>         /* Needed for the macros */
#include <linux/slab.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/list.h>
#include <linux/string.h>
#include <stdbool.h>
#include <linux/file.h>
#include <linux/crypto.h>
#include <linux/scatterlist.h>
#include <linux/namei.h>
#include <linux/acpi.h>
#include <linux/ctype.h>
#include <linux/dcache.h>

extern int module_register(struct module_reg *module_reg);
extern int module_deregister(struct module_reg *module_reg);

/*
 * Find the last occurence of \0 or \n and return the offset of buffer
 */
int remove_garbage_value(char *data, int recordsize)
{
	int i= recordsize-1;
        if(data[i]!='\n' && data[i]!='\0')
        {
                while(i>0)
                {
                        if(data[i]=='\n' || data[i] == '\0')
                                break;
                        i--;
                }
        }
        if((i+1) < PAGE_SIZE && data[i]!='\0')
                data[++i]='\0';
        return strlen(data);
}

/*
 * read the data of the file in the buffer
 */
int read_file(struct file* file, char *data, int size) {
        int ret;
        mm_segment_t oldfs;
        oldfs = get_fs();
        set_fs(KERNEL_DS);
        ret = vfs_read(file, data, size , &file->f_pos);
        set_fs(oldfs);
        return ret;
}

/*
 * Input: s: Pointer to a string which has hex bytes to be converted to char,
 * endptr: end pointer of the string
 * Functionality: returns a character for given bytes seperated by white spaces
 */
unsigned char gethex(const char *s, char **endptr)
{
	char ch;
	while (isspace(*s)) s++;
	ch = strtoul(s, endptr, 16);
	return ch;
}


/*
 * Input: s: String with bytes, to be converted to characters, char_in_hex: Output is 
 * written to this variable, length : length of the output.
 * Functionality: returns a character string for given bytes of string seperated by white spaces
 */
void convert(const char *s,char *char_in_hex, int *length)
{
    	unsigned char *answer = char_in_hex;
    	unsigned char *p;
	for (p = answer; *s; p++)
	{
		*p = gethex(s, (char **)&s);
	}
	*length = p - answer;
}

/*
 * Input: input_file : The file which is to be scanned for virus,  blacklist_file: The
 * file which has blacklist bytes
 * Functionality: Checks for virus, returns true if it has virus and false otherwise.
 */
bool check_in_blacklist(struct file * input_file,struct file * blacklist_file)
{
	int read_blacklist_bytes = 0,read_file_bytes = 0, blacklist_size = 0, file_size= 0, original_file_size=0;
	bool virus_flag = false;
	int blacklist_fp = 0, hex_in_char_len=0;
	char *black_list_work_buff = NULL, *black_list_init_buff = NULL, *parse_virus= NULL, *input_file_buff= NULL;
	char *hex_in_char_ptr=NULL, *virus_name=NULL;
	
	input_file_buff = kmalloc(PAGE_SIZE,GFP_KERNEL);
	if(input_file_buff == NULL)
	{
		goto out;
	}
	input_file_buff[0]='\0';
	blacklist_size = i_size_read(file_inode(blacklist_file));
	original_file_size = i_size_read(file_inode(input_file));
	black_list_init_buff = kmalloc(PAGE_SIZE,GFP_KERNEL);
	black_list_init_buff[0]='\0';

	hex_in_char_ptr = kmalloc(PAGE_SIZE,GFP_KERNEL);
    	hex_in_char_ptr[0]='\0';

    	while(blacklist_size > 0 || (black_list_work_buff!=NULL && strlen(black_list_work_buff) > 0))
    	{        
        	hex_in_char_ptr[0]='\0';        
        	if(black_list_work_buff == NULL || strlen(black_list_work_buff) == 0)
        	{
			black_list_work_buff = black_list_init_buff;
			read_blacklist_bytes = read_file(blacklist_file, black_list_work_buff, PAGE_SIZE);
			blacklist_fp += remove_garbage_value(black_list_work_buff, read_blacklist_bytes);
        		blacklist_file->f_pos = blacklist_fp;
			blacklist_size -= strlen(black_list_work_buff);
		}

		if(black_list_work_buff != NULL)
		{
			parse_virus = strsep(&black_list_work_buff,"\n");
		}
		else
		{
			goto out;
		}
		if(parse_virus !=NULL)		
		{
			virus_name = strsep(&parse_virus,",");	
		}
		else
		{
			goto out;		
		}
		
		if(parse_virus !=NULL)    
        	{
            		convert(parse_virus,hex_in_char_ptr, &hex_in_char_len);
            		hex_in_char_ptr[hex_in_char_len]='\0';
        	}
		else
		{
			goto out;
		}

		file_size=original_file_size;
		input_file->f_pos=0;
		while(file_size > 0 )
		{			
			read_file_bytes = read_file(input_file, input_file_buff,  PAGE_SIZE-1);
			input_file_buff[read_file_bytes]='\0';
			if(strstr(input_file_buff, hex_in_char_ptr)!= NULL)
			{
				virus_flag = true;				
				goto out;				
			}
			if(input_file->f_pos!=original_file_size)
			{
				input_file->f_pos -= (hex_in_char_len);
				file_size = file_size - read_file_bytes + (hex_in_char_len);
			}
			else
			{
				file_size -= read_file_bytes;
				
			}
			input_file_buff[0]='\0';
		}		
	}

	out:	
	if(hex_in_char_ptr)
			kfree(hex_in_char_ptr);		
	
	if(input_file_buff)
		kfree(input_file_buff);

	if(black_list_init_buff)
		kfree(black_list_init_buff);

	return virus_flag;
}

int check_for_virus(const char *filename, int flags, umode_t mode)
{
	int ret = 0;
	struct file *black_list= NULL, *input_file = NULL;
	bool is_virus=false;
	char *virus_file_name = NULL;
	struct inode *inode;
	umode_t im;
	
	/*Open blacklist file*/
	black_list = filp_open("/tmp/blacklist", O_RDONLY, 0);
        if(IS_ERR(black_list)) {
		printk("blacklist file is not present\n");
		black_list = NULL;		
		goto out;
        }
	
	/*Open input file for scan*/
	input_file = filp_open(filename, flags, mode);
	//input_file = filp_open("/usr/src/hw3-cse506p33/hw3/modules/antivirus_test.txt", O_RDONLY, 0);
        if(IS_ERR(input_file)) {

		printk("Not able to open input file\n");
                input_file = NULL;
		goto out;
        }
	
	/*check for regular file*/
	inode = file_inode(input_file);
    	if(inode != NULL)
	{    
        	im = inode->i_mode;
        	if(S_ISCHR(im) > 0)
		{
			printk("Error in S_ISCHR\n");
            		goto out;
        	}
        	if(S_ISBLK(im) > 0) 
		{
			printk("ERROR in ISBLK\n");
            		goto out;
        	}
    	}

	printk("Checking for a virus\n");	
	/* Check for virus content */
	is_virus=check_in_blacklist(input_file,black_list);
	
	if(is_virus)
	{
		printk("Inside check_for_virus : It says a virus\n");
		ret = -1;  /*set file as a virus file*/
		goto out;
	}
out:	
	/*Close blacklist file */
	if(black_list != NULL)
		filp_close(black_list, NULL);

	/*Close input file */
	if(input_file !=NULL)
		filp_close(input_file,NULL);

	/*Free memory*/	
	if(virus_file_name !=NULL)
		kfree(virus_file_name);
	
	return ret;
}


long anti_open(const char __user * path, int flags, umode_t mode)
{
	int ret=0;
	ret = check_for_virus(path,flags, mode);
	if(ret==-1)
		printk("File is a virus\n");
	else if(ret==0)
		printk("File is not virus\n");
        return ret;
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
                        ret=anti_open(path_name,arg1,arg2);
                        break;
		default:
			break;
	}
	return ret;
}

struct module_reg antivirus={
        .type =  ANTIVIRUS_MODULE,
        .sys_op = system_call_entry,
	.module = THIS_MODULE
};

static int __init antivirus_init(void)
{
        module_register(&antivirus);
        printk(KERN_INFO "ANTIVIRUS MODULE IS LOADED.\n");
        return 0;
}

static void __exit antivirus_exit(void)
{
        module_deregister(&antivirus);
	printk(KERN_INFO "ANTIVIRUS MODULE UNLOADED.\n");
}

MODULE_LICENSE("GPL");

module_init(antivirus_init);
module_exit(antivirus_exit);
