1.	Mini Kernel is a basic kernel that resembles Linux in its functionality. This personal project was done to understand the inner working of various features of Linux Kernel and their interoperability with each other.
2.	The features supported by Mini Kernel are:
•	Booting the Kernel from hard disk
•	Process management
•	Dynamic memory
•	Paging
•	Filesystem -FAT16
•	Interrupt management- Keyboard, Exceptions
•	System calls
•	Context switches
•	Multi-processing
•	Inter Process communication using shared memory
•	Synchronization using mutex locks
3.	Languages used: 
•	C
•	Assembly
•	Shell script
•	GNU Make
4.	Tools:
•	Qemu
•	GDB
5.	The procedure to configure the project
•	Install Qemu (Well tested on Linux)
•	Create a GCC Cross compiler on the system ://wiki.osdev.org/GCC_Cross-Compiler
•	Open command prompt and run the below commands. 
               Note: Make sure you are on the right path.
o	cd Mini_Kernel 
o	chmod +x build.sh
o	./build.sh
o	qemu-system-i386 -hda  ./bin/os.bin


