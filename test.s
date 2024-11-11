global _start

_start:
    mov   rdi, 8
    bsr   rax, rdi

    mov   rdi, 16
    bsr   rax, rdi

    mov   rax, 60
    mov   rdi, 0
    syscall

