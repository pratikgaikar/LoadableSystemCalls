#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
int main ()
{
    pid_t child_pid;
    printf ("the main program process ID is %d\n", (int) getpid());
    sleep(40);
    fflush(NULL);
    child_pid = vfork () ;
    if (child_pid > 0) 
    {
        printf ("this is the parent process, with id %d\n", (int) getpid ());
        sleep(40);
	printf ("the child's process ID is %d\n",(int) child_pid );
    }
    else if (child_pid == 0)
        printf ("this is the child process, with id %d\n", (int) getpid ());
    else
        printf ("fork failed\n");

    return 0;
}
