.set IRQ_BASE, 0x20
.section .text

# publish jump label to outside
.extern _ZN16InterruptManager15handleInterruptEhj

.global _ZN16InterruptManager22HandleInterruptRequestEv

.macro HandleExceptions num
# c++ mangling {Ev for void return type}
.global _ZN16InterruptManager16HandleException\num\()Ev
_ZN16InterruptManager16HandleException\num\()Ev:
    movb $\num, (interruptNumber)
    jmp int_bottom
.endm


.macro HandleInterruptRequest num
.global _ZN16InterruptManager26HandleInterruptRequest\num\()Ev
_ZN16InterruptManager26HandleInterruptRequest\num\()Ev:
    movb $\num + IRQ_BASE, (interruptNumber)
    jmp int_bottom
.endm



HandleInterruptRequest 0x00
HandleInterruptRequest 0x01

int_bottom:
    # push all register, store away all old register values
    pusha
    pushl %ds
    pushl %es
    pushl %fs
    pushl %gs

    push %esp
    push (interruptNumber)
    call _ZN16InterruptManager15handleInterruptEhj
    # addl %5, %esp
    movl %eax, %esp

    popl %gs
    popl %fs
    popl %es
    popl %ds
    popa

_ZN16InterruptManager22HandleInterruptRequestEv:

    iret

handle_interrupt:
    interruptNumber: .byte 0

