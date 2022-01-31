.extern _ZN10IDTManager11isr_handlerEP18InterruptRegisters

.macro ISR_NOERR index
    .section .text
    .global _isr\index
    .type _isr\index, @function
    _isr\index:
        cli
        pushq $0x00
        pushq $\index
        jmp isr_common
    .section .data
    .quad _isr\index
.endm

.macro ISR_ERR index
    .section .text
    .global _isr\index
    .type _isr\index, @function
    _isr\index:
        cli
        pushq $\index
        jmp isr_common
    .section .data
    .quad _isr\index
.endm

.altmacro
.section .data
.globl isr_handlers
isr_handlers:
.set i,0
.rept 256
    .if i == 2
    .section .data
    .quad _isr2
    .elseif i == 8 || (i > 9 && i < 15)
    ISR_ERR %i
    .else
    ISR_NOERR %i
    .endif
    .set i, i+1
.endr

.section .text

.global _isr2
.type _isr2, @function
_isr2:
    cli
1:
    hlt
    jmp 1b


.global isr_common
isr_common:
    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rsi
    pushq %rdi
    pushq %rbp
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15

    cld
    mov %rsp, %rdi
    call _ZN10IDTManager11isr_handlerEP18InterruptRegisters
    mov %rax, %rsp

    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rbp
    popq %rdi
    popq %rsi
    popq %rdx
    popq %rcx
    popq %rbx
    popq %rax

    add $16, %rsp
    iretq

