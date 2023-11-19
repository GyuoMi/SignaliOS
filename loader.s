#  we set this magic number so the bootloader recognises the kernel
# it is a hexadecimal number
# stored in compiler variable
.set MAGIC, 0x1badb002
.set FLAGS, (1<<0 | 1<<1)
.set CHECKSUM, -(MAGIC + FLAGS)

# now these are put into the .o compiled file
.section .multiboot
    .long MAGIC
    .long FLAGS
    .long CHECKSUM

# text section
# assume kernelMain is there, the linker will take care of it
.section .text
.extern kernelMain # declare external symbols for kernelMain and 
.extern callConstructors # invoke pointers to constructors 
.global loader # Declare loader as a global symbol

# entry point
loader:
# esp is the register that stores the memory address of the stack pointer
    mov $kernel_stack, %esp # set stack pointer to kernel_stack
    call callConstructors
    # before the bootloader loads the kernel it will need to store the following into RAM
    push %eax # accumulator eax used since we start off in 32 bits
    push %ebx # base stores magic number, same idea with 32 bits
    call kernelMain 

# common bootloader practice to keep bootloader idling whil waiting on OS
_stop:
    cli # disable interrupts
    hlt # halt cpu
    jmp _stop # infinite loop jumps back to top of _stop


.section .bss
# we do this to prevent overwriting in the stack
.space 2*1024*1024; # 2 MiB reserved for kernel stack
kernel_stack:

