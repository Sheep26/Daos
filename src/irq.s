.code32
.global irq0
.global irq_ignore
.extern timer_handler

.type irq0, @function
.type irq_ignore, @function

irq0:
    pusha
    call timer_handler

    mov $0x20, %al
    out %al, $0x20
    popa

    iret

irq_ignore:
    pusha
    mov $0x20, %al
    out %al, $0x20
    popa

    iret
