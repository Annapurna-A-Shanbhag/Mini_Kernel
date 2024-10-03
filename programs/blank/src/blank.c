#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "api.h"

int main(int argc, char **argv)
{

    while (1)
    {
        print(argv[0]);
        for (int i = 0; i < 1000000; i++)
        {
        }
    }
    return 0;
}