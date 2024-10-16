#ifndef CLASSIC_H
#define CLASSIC_H

#include "keyboard.h"
#include "../string/string.h"
#include "../io/io.h"
#include "../idt/idt.h"
#include <stdint.h>

#define PS2_PORT 0x64
#define PS2_COMMAND_ENABLE_FIRST_PORT 0xAE

#define CLASSIC_KEYBOARD_KEY_RELEASED 0x80
#define ISR_KEYBOARD_INTERRUPT 0x21
#define KEYBOARD_INPUT_PORT 0x60
#define CLASSIC_KEYBOARD_CAPSLOCK 0x3A

struct keyboard *classic_init();


#endif
