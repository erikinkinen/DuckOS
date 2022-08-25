.globl _start 
_start:
    call kernel_main
.loop:
    jmp .loop
    