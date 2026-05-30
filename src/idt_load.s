.global idt_load

idt_load:
    mov 4(%esp), %eax   # argument = &idt_ptr
    lidt (%eax)

    ret
