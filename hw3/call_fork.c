#include  <stdio.h>
#include  <string.h>
#include  <sys/types.h>
#include  <unistd.h>
int main(void)
{
	int pid;

	sleep(40);

	pid = fork();
	if (pid == 0)
		rmdir("TESTDIR CHAILD TRACING");
	else
		rmdir("TESTING PARENT TRACING");

	sleep(30);
	return 0;
}
