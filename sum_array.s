.section .text
.globl sum_array
sum_array:
    # RDI = array, RSI = count, RDX = address of the overflow flag.
    xor %eax, %eax
    xor %ecx, %ecx
sum_next:
    cmp %rsi, %rcx
    jae sum_done
    movslq (%rdi,%rcx,4), %r8
    add %r8, %rax
    inc %rcx
    jmp sum_next

sum_done:
    # Accumulate in 64 bits, then check whether the result fits in EAX.
    movslq %eax, %r8
    cmp %rax, %r8
    setne %cl
    movzbl %cl, %ecx
    mov %ecx, (%rdx)
    ret

.section .note.GNU-stack,"",@progbits
