#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>

long timeuser(unsigned int* uid_storage) { return syscall(342, uid_storage); }

int main(void)
{
        unsigned int timeuser_uid;

        if (timeuser(&timeuser_uid))
                err(-1, "timeuser");

        printf("User using most time: %u\n", timeuser_uid);
}