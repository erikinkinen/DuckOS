.extern syscall_handler_c
syscall_handler:
    call syscall_handler_c
    iretq
.globl syscall_handler
