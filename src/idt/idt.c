#include "idt.h"

struct idt_desc idt[TOTAL_NO_INTERRUPTS];
struct idt_desc_table idtr;
static ISR80H_COMMAND isr80h_commands[NUMBER_OF_ISR80H_COMMANDS];
static INTERRUPT_CALLBACK_FUNCTION interrupt_callbacks[TOTAL_NO_INTERRUPTS];
extern void *interrupt_pointer_table[TOTAL_NO_INTERRUPTS];

extern void isr80h_wrapper();
extern void idt_load(struct idt_desc_table *ptr);
extern void isr();
extern void no_interrupt();

void initial()
{
    print("IDT Initialization\n");
}

void no_interrupt_handler()
{
    print("No interrupt handler\n");
    outb(0x20, 0x20);
}

void idt_zero()
{
    print("Divide by zero error\n");
}

void interrupt_handler(int interrupt, struct interrupt_frame *frame)
{
    kernel_page();
    if (interrupt_callbacks[interrupt] != 0)
    {
        task_current_save_state(frame);
        
        //terminal_writechar(interrupt+97,15);
        //print_c("caught ",interrupt);
        interrupt_callbacks[interrupt](frame);
    }

    task_page();
    outb(0x20, 0x20);
}

void idt_set(int index,void *addr)
{
    struct idt_desc *desc= &idt[index];
    desc->offset_first = ((uint32_t)addr) & 0x0000ffff;
    desc->segment_selector = KERNEL_CODE_SEGMENT_SELECTOR;
    desc->zero = 0x0;
    desc->type_attribute = 0xEE;
    desc->offset_second = ((uint32_t)addr) >> 16;
}

void register_isr80h_command(int command_no, ISR80H_COMMAND command)
{
    if (command_no < 0 || command_no > NUMBER_OF_ISR80H_COMMANDS)
    {
        panic("The command is out of bounds\n");
    }
    if (isr80h_commands[command_no])
    {
        panic("You are trying to override an existing command\n");
    }
    isr80h_commands[command_no] = command;
}

int idt_register_interrupt_callback(int interrupt_no, INTERRUPT_CALLBACK_FUNCTION ISR)
{
    if (interrupt_no < 0 || interrupt_no > TOTAL_NO_INTERRUPTS)
    {
        return -EINVARG;
    }
    interrupt_callbacks[interrupt_no] = ISR;
    return 0;
}

void idt_clock()
{
    //print("IDT CLOCK\n");
    outb(0x20, 0x20);
    task_next();
}

void idt_exception_handler()
{
    //print("Exception\n");
    process_terminate(task_current_task()->process);
    task_next();
}

void idt_initialization()
{
    
    memnset(idt, 0, sizeof(idt));
    for (int i = 0; i < TOTAL_NO_INTERRUPTS; i++)
    {
        idt_set(i, interrupt_pointer_table[i]);
    }
    idt_set(0x00, idt_zero);
    idt_set(0x80, isr80h_wrapper);

    for (int i = 0; i < 0x20; i++)
    {
        idt_register_interrupt_callback(i, idt_exception_handler);
    }

    idt_register_interrupt_callback(0x20,idt_clock);

    idtr.addr = (uint32_t)idt;
    idtr.size = (uint16_t)sizeof(idt)-1;

    

    idt_load(&idtr);
}

void *isr80h_handle_command(int command, struct interrupt_frame *frame)
{
    void *result = 0;

    if (command < 0 || command >= NUMBER_OF_ISR80H_COMMANDS)
    {
        // Invalid command
        return 0;
    }

    ISR80H_COMMAND command_func = isr80h_commands[command];
    if (!command_func)
    {
        return 0;
    }

    result = command_func(frame);
    return result;
}

void *isr80h_handler(int command, struct interrupt_frame *frame)
{
    //print("Okay we are reaching here\n");
    void *res = 0;
    kernel_page();
    task_current_save_state(frame);
    res = isr80h_handle_command(command-1, frame);
    task_page();
    return res;
}
