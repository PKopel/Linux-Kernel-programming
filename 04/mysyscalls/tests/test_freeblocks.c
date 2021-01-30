#include <err.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>

long freeblocks(const char* pathname, uint64_t* count)
{
        return syscall(344, pathname, count);
}

int main(int argc, char** argv)
{
        uint64_t free_blocks_count;
        char* path = argv[1];

        if (freeblocks(path, &free_blocks_count))
                err(-1, "freeblocks");

        printf("%llu free blocks on path %s\n",
            (unsigned long long)free_blocks_count, path);

        return 0;
}