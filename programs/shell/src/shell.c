#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "api.h"
int main(int argc, char **argv)
{
    print("MiniOS v1.0.0\n");
    while (1)
    {
        print("> ");
        char buf[1024];
        terminal_readline(buf, sizeof(buf), true);
        print("\n");
        system_run(buf);

        print("\n");
    }
    return 0;
}