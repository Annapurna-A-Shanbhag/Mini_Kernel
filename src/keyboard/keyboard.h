#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "../memory/memory.h"
#include "../status.h"
#include "../kernel.h"
#include "../task/process.h"
#include "../task/task.h"
#include "./classic.h"

#define KEYBOARD_CAPS_LOCK_ON 1
#define KEYBOARD_CAPS_LOCK_OFF 0

typedef unsigned int KEYBOARD_CAPS_LOCK_STATE;

typedef int (*KEYBOARD_INIT_FUNCTION)();

struct keyboard
{
    char name[20];
    KEYBOARD_INIT_FUNCTION init;
    KEYBOARD_CAPS_LOCK_STATE capslock_state;
    struct keyboard *next;
};

struct process;
void keyboard_initialization();
void keyboard_backspace(struct process *process);
void keyboard_push(char c);
char keyboard_pop();
int keyboard_insert(struct keyboard *keyboard);
void keyboard_set_capslock(struct keyboard *keyboard, KEYBOARD_CAPS_LOCK_STATE state);
KEYBOARD_CAPS_LOCK_STATE keyboard_get_capslock(struct keyboard *keyboard);

#endif