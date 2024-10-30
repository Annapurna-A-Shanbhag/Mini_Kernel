# Mini Kernel From Scratch!

Mini Kernel is a working multi-processing kernel that mimics Linux functionality, developed as a personal project to gain a deeper understanding of Linux kernel internals and how various components work together.

## Features

Mini Kernel supports the following features:
- **Booting**: Kernel booting from the hard disk
- **Process Management**
- **Dynamic Memory Management**
- **Paging**
- **Filesystem**: FAT16 support
- **Interrupt Management**: Keyboard interrupts, exceptions handling
- **System Calls**
- **Context Switching**
- **Multi-processing**
- **ELF-Loader**: C program execution
- **Inter-process Communication (IPC)**: Shared memory implementation
- **Synchronization**: Mutex locks for process synchronization
  

## Languages Used
- **C**
- **Assembly**
- **Shell Script**
- **GNU Make**

## Tools
- **Qemu**
- **GDB**

## Project Setup

Follow these steps to configure and run Mini Kernel:

1. **Install Qemu** (Tested on Linux).
2. **Create a GCC Cross Compiler**. Follow instructions at: [GCC Cross Compiler Setup](https://wiki.osdev.org/GCC_Cross-Compiler).
3. **Run the commands below** from the terminal, ensuring you're in the correct directory.

   ```bash
   cd Mini_Kernel
   chmod +x build.sh
   ./build.sh
   qemu-system-i386 -hda ./bin/os.bin


