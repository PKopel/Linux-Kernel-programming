#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>

#define TASK_COMM_LEN 16

long kernelps(size_t* entries_num, char** table_entries)
{
        return syscall(343, entries_num, table_entries);
}

int main(void)
{
        size_t processes_count;
        char** comm_table;

        if (kernelps(&processes_count, NULL))
                err(-1, "process count");

        printf("%zu processes\n", processes_count);

        if (processes_count <= 0)
                errx(-1, "number of processes should be positive");

        comm_table = malloc(processes_count * TASK_COMM_LEN * sizeof(char*));
        if (!comm_table)
                err(-1, "malloc");
        if (kernelps(&processes_count, comm_table))
                err(-1, "process names");

        printf("processes:\n");
        for (size_t i = 0; i < processes_count; i++)
                printf("%s\n", comm_table[i]);

        free(comm_table);

        return 0;
}