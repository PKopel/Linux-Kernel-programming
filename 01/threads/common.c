#include "common.h"

void error(char *msg)
{
    perror(msg);
    exit(-1);
}

int get_random_index(int bound)
{
    return rand() % bound;
}

int get_random_time(int base)
{
    return base + rand() % base;
}