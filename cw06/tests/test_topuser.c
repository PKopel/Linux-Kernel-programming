#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>

long topuser(unsigned int* uid_storage) { return syscall(341, uid_storage); }

int main(void)
{
        unsigned int top_uid;

        if (topuser(&top_uid))
                err(-1, "topuser");

        printf("User with the most processes: %u\n", top_uid);
}