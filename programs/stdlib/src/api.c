#include "api.h"
#include "string.h"

struct command_argument *parse_command(char *command, int max)
{
    struct command_argument *root_command = 0;
    char scommand[1025];
    if (max >= (int)sizeof(scommand))
    {
        return 0;
    }

    str_n_cpy(scommand, command, sizeof(scommand));
    char *token = strtok(scommand, " ");
    if (!token)
    {
        goto out;
    }

    root_command = system_malloc(sizeof(struct command_argument));
    if (!root_command)
    {
        goto out;
    }

    str_n_cpy(root_command->argument, token, sizeof(root_command->argument));
    root_command->next = 0;

    struct command_argument *current = root_command;
    token = strtok(NULL, " ");
    while (token != 0)
    {
        struct command_argument *new_command = system_malloc(sizeof(struct command_argument));
        if (!new_command)
        {
            break;
        }

        str_n_cpy(new_command->argument, token, sizeof(new_command->argument));
        new_command->next = 0x00;
        current->next = new_command;
        current = new_command;
        token = strtok(NULL, " ");
    }
out:
    return root_command;
}
int getkeyblock()
{
    int val = 0;
    do
    {
        val = getkey();
    } while (val == 0);
    return val;
}

void terminal_readline(char *out, int max, bool output_while_typing)
{
    int i = 0;
    for (i = 0; i < max - 1; i++)
    {
        char key = getkeyblock();

        // Carriage return means we have read the line
        if (key == 13)
        {
            break;
        }

        if (output_while_typing)
        {
            system_putchar(key);
        }

        // Backspace
        if (key == 0x08 && i >= 1)
        {
            out[i - 1] = 0x00;
            // -2 because we will +1 when we go to the continue
            i -= 2;
            continue;
        }

        out[i] = key;
    }

    // Add the null terminator
    out[i] = 0x00;
}

int system_run(char *command)
{
    char buf[1024];
    str_n_cpy(buf, command, sizeof(buf));
    struct command_argument *root_command_argument = parse_command(buf, sizeof(buf));
    if (!root_command_argument)
    {
        return -1;
    }

    return invoke_system_command(root_command_argument);
}