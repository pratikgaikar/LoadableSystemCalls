#include<stdio.h>
#include <fcntl.h>

int main(void)
{
	printf("First time\n");
	sleep(30);
	open("/usr/src/check", O_RDONLY);
	mkdir("/usr/src/check", 640);
	rmdir("/usr/src/check", 640);
	open("/usr/src/check", O_RDONLY);
	printf("Second time\n");
	sleep(30);
	link("/usr/src/check", "/usr/src/check");
	symlink("/usr/src/check", "checksymlink");
	unlink("/usr/src/checkunlink");
	open("/usr/src/check", O_RDONLY);
	rename("/user/check", "/dir/check");
	printf("Third time\n");
	sleep(30);
	link("/usr/src/check", "/usr/src/checklink");
	symlink("/usr/src/check", "checksymlink");
	unlink("/usr/src/checkunlink");
	open("/usr/src/check", O_RDONLY);
	rename("/user/check", "/dir/check");
	printf("Fourth time\n");
	sleep(30);
	mkdir("/usr/src/check", 640);
	rmdir("/usr/src/check", 640);
	link("/usr/src/check", "/usr/src/checklink");
	symlink("/usr/src/check", "checksymlink");
	unlink("/usr/src/checkunlink");
	open("/usr/src/check", O_RDONLY);
	rename("/user/check", "/dir/check");

	return 0;
}
