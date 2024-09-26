#include "memory.h"

void memnset(void *ptr, int c, size_t size)
{
    char *cptr = (char *)ptr; // De we really need a conversion??
    for (int i = 0; i < size; i++)
    {
        cptr[i] = (char)c;
    }
}

void *memncpy(void *dest, void *src, size_t size)
{
    char *d = dest;
    char *s = src;
    for (int i = 0; i < size; i++)
    {
        *d++ = *s++;
    }
    return dest;
}

int memncmp(void *s1, void *s2, int count)
{
    char *c1 = s1;
    char *c2 = s2;
    while (count-- > 0)
    {
        if (*c1++ != *c2++)
        {
            return c1[-1] < c2[-1] ? -1 : 1;
        }
    }

    return 0;
}