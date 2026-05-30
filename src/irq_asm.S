.intel_syntax noprefix
.code32
.extern isr_handler
.extern irq_handler

.macro ISR_NOERRC num
.global isr\num

isr\num:
    cli
    push 0
    push \num
    jmp isr_common_stub
.endm

.macro ISR_ERRC num
.global isr\num

isr\num:
    cli
    push \num
    jmp isr_common_stub
.endm

.macro IRQ_M num
.global irq\num
.set irqnum, \num + 32

irq\num:
    cli
    push 0
    push irqnum
    jmp irq_common_stub
.endm

.macro COMMON_STUB name
.extern \name\()_handler

\name\()_common_stub:
    pusha

    push ds
    push es
    push fs
    push gs

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov eax, esp
    push eax

    call \name\()_handler

    add esp, 4        # remove pointer argument

    pop gs
    pop fs
    pop es
    pop ds

    popa

    add esp, 8        # remove interrupt number + error code/dummy
    iret
.endm

ISR_NOERRC 0
ISR_NOERRC 1
ISR_NOERRC 2
ISR_NOERRC 3
ISR_NOERRC 4
ISR_NOERRC 5
ISR_NOERRC 6
ISR_NOERRC 7

ISR_ERRC 8

ISR_NOERRC 9
ISR_ERRC 10
ISR_ERRC 11
ISR_ERRC 12
ISR_ERRC 13
ISR_ERRC 14

ISR_NOERRC 15
ISR_NOERRC 16
ISR_ERRC 17
ISR_NOERRC 18
ISR_NOERRC 19
ISR_NOERRC 20
ISR_NOERRC 21
ISR_NOERRC 22
ISR_NOERRC 23
ISR_NOERRC 24
ISR_NOERRC 25
ISR_NOERRC 26
ISR_NOERRC 27
ISR_NOERRC 28
ISR_NOERRC 29
ISR_ERRC 30
ISR_NOERRC 31

COMMON_STUB isr

IRQ_M 0
IRQ_M 1
IRQ_M 2
IRQ_M 3
IRQ_M 4
IRQ_M 5
IRQ_M 6
IRQ_M 7
IRQ_M 8
IRQ_M 9
IRQ_M 10
IRQ_M 11
IRQ_M 12
IRQ_M 13
IRQ_M 14
IRQ_M 15

COMMON_STUB irq
