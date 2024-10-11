#include "keyboard.h"

struct keyboard *keyboard_head_list;
struct keyboard *keyboard_tail_list;

int keyboard_insert(struct keyboard *keyboard)
{
    int res = 0;
    if (keyboard->init == 0)
    {
        res = -EINVARG;
        goto out;
    }
    if (keyboard_tail_list)
    {
        keyboard_tail_list->next = keyboard;
        keyboard_tail_list = keyboard;
    }
    else
    {
        keyboard_head_list = keyboard_tail_list = keyboard;
    }
    res = keyboard->init();

out:
    return res;
}

static int keyboard_get_tail_index(struct process *process)
{
    return process->keyboard.tail % sizeof(process->keyboard.buffer);
}

void keyboard_backspace(struct process *process)
{
    process->keyboard.tail -= 1;
    int real_index = keyboard_get_tail_index(process);
    process->keyboard.buffer[real_index] = 0x00;
}

KEYBOARD_CAPS_LOCK_STATE keyboard_get_capslock(struct keyboard *keyboard)
{
    return keyboard->capslock_state;
}

void keyboard_set_capslock(struct keyboard *keyboard, KEYBOARD_CAPS_LOCK_STATE state)
{
    keyboard->capslock_state = state;
}

void keyboard_push(char c)
{
    struct process *process = process_current();
    if (!process)
    {
        return;
    }

    if (c == 0)
    {
        return;
    }

    int real_index = keyboard_get_tail_index(process);
    process->keyboard.buffer[real_index] = c;
    process->keyboard.tail++;
}

char keyboard_pop()
{
    if (!task_current_task())
    {
        return 0;
    }

    struct process *process = task_current_task()->process;
    int real_index = process->keyboard.head % sizeof(process->keyboard.buffer);
    char c = process->keyboard.buffer[real_index];
    if (c == 0x00)
    {
        // Nothing to pop return zero.
        return 0;
    }

    process->keyboard.buffer[real_index] = 0;
    process->keyboard.head++;
    return c;
}

void keyboard_initialization()
{
    keyboard_insert(classic_init());
}
