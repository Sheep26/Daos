.intel_syntax noprefix
.code32
.extern apic_timer_handler
.global apic_stub

apic_stub:
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

    call apic_timer_handler

    add esp, 4        # remove pointer argument

    pop gs
    pop fs
    pop es
    pop ds

    popa
    iret
