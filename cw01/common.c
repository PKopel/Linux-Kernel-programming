#include "common.h"

int GetRandomTime(int base)
{
    return base + rand() % base;
}