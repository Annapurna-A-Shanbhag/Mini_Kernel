#include "string.h"

int str_len(char *str)
{ // We can simplify this
    int i = 0;
    char c = *(str + i);
    while (c != '\n')
    {
        i++;
        c = *(str + i);
    }
    return i;
}

void str_n_cpy(char *dest, char *src, size_t n)
{
    for (int i = 0; i < n; i++)
    {
        *(dest + i) = *(src + i);
    }
    dest[n] = '\n';
}

int str_n_cmp(char *str1, char *str2, size_t n)
{
    for (int i = 0; i < n; i++)
    {
        if (*(str1 + i) != *(str2 + i))
        {
            return -1;
        }
    }
    return 0;
}

int isdigit(char c)
{
    if (c >= 48 && c <= 57)
    {
        return 0;
    }
    return -1;
}

char *sp = 0;
char *strtok(char *str, const char *delimiters)
{
    int i = 0;
    int len = strlen(delimiters);
    if (!str && !sp)
        return 0;

    if (str && !sp)
    {
        sp = str;
    }

    char *p_start = sp;
    while (1)
    {
        for (i = 0; i < len; i++)
        {
            if (*p_start == delimiters[i])
            {
                p_start++;
                break;
            }
        }

        if (i == len)
        {
            sp = p_start;
            break;
        }
    }

    if (*sp == '\0')
    {
        sp = 0;
        return sp;
    }

    // Find end of substring
    while (*sp != '\0')
    {
        for (i = 0; i < len; i++)
        {
            if (*sp == delimiters[i])
            {
                *sp = '\0';
                break;
            }
        }

        sp++;
        if (i < len)
            break;
    }

    return p_start;
}