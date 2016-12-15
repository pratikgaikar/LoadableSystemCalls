#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
int main()
{
	sleep(30);
	open("/tmp/antivirus_test.txt",O_RDONLY);

        return 0;
}

