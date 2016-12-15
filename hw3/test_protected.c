#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(void)
{
	printf("Checking Protected Module : First time\n");
	sleep(30);
	open("/usr/src/check.protected", O_RDONLY);
	mkdir("/usr/src/check.protected", 640);
	rmdir("/usr/src/check.protected");
	open("/usr/src/check", O_RDONLY);

	printf("Checking Protected Module : Second time\n");
	sleep(30);
	link("/usr/src/check.xyz", "/usr/src/check.protected");
	symlink("/usr/src/check", "checksymlink.protected");
	unlink("/usr/src/checkunlink.protected");
	open("/usr/src/check", O_RDONLY);
	rename("/user/check", "/dir/check.protected");

	printf("Checking Protected Module : Third time\n");
	sleep(30);
	link("/usr/src/check", "/usr/src/checklink");
	symlink("/usr/src/check", "checksymlink");
	unlink("/usr/src/checkunlink.protected");
	open("/usr/src/check", O_RDONLY);
	rename("/user/check", "/dir/check");

	printf("Checking Protected Module : Fourth time\n");
	sleep(30);
	mkdir("/usr/src/check", 640);
	rmdir("/usr/src/check");
	link("/usr/src/check", "/usr/src/checklink");
	symlink("/usr/src/check", "checksymlink");
	unlink("/usr/src/checkunlink");
	open("/usr/src/check", O_RDONLY);
	rename("/user/check", "/dir/check");

	return 0;
}
