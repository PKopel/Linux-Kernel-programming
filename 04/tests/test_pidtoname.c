#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>

#define TASK_COMM_LEN 16

long pidtoname(int pid, char name[TASK_COMM_LEN])
{
        return syscall(345, pid, name);
}

int main(int argc, char** argv)
{
        char name[TASK_COMM_LEN];
        int pid;

        pid = atol(argv[1]);

        if (pid < 0)
                errx(-1, "pid value out of range");

        if (pidtoname(pid, name))
                err(-1, "pidtoname");

        printf("Name of pid %d: %s\n", pid, name);

        return 0;
}