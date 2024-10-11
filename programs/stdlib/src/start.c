#include "api.h"

extern int main(int argc, char **argv);

void c_main()
{
    
    struct process_arguments arguments;
    process_get_arguments(&arguments);

    int res = main(arguments.argc, arguments.argv);
    if (res == 0)
    {
    }
}