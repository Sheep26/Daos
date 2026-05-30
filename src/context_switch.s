.global context_switch

.section .text

context_switch:
    # Save pointers
    mov 4(%esp), %eax      # old_esp
    mov 8(%esp), %edx      # old_ebp
    mov 12(%esp), %ecx     # new_esp
    mov 16(%esp), %ebx     # new_ebp

    # Save current state
    mov %esp, (%eax)
    mov %ebp, (%edx)

    # Switch stacks
    mov %ecx, %esp
    mov %ebx, %ebp

    ret
