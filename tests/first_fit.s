global _start

%include "utils.inc"
%include "malloc.inc"
%include "free.inc"
%include "mmap.inc"

section .text
_start:
  mov   rdi, 10
  call  malloc

  cmp   rax, 0
  jl    .error

  mov   [p1], rax

  mov   rdi, rax
  call  free

  mov   rdi, 15
  call  malloc

  cmp   rax, 0
  jl    .error

  mov   [p2], rax

  mov   rdi, [p1]
  cmp   rdi, rax
  jne   .error

  mov   rdi, 12
  call  malloc

  cmp   rax, 0
  jl    .error

  mov   [p3], rax

  mov   rdi, 8000
  call  malloc

  cmp   rax, 0
  jl    .error

  mov   [p4], rax

  mov   rdi, [p3]
  call  free

  mov   rdi, 4149
  call  malloc

  cmp   rax, 0
  jl    .error

  mov   [p5], rax
  mov   rdi, [p3]

  cmp   rdi, rax
  je    .error

  mov   rdi, 300
  call  malloc

  cmp   rax, 0
  jl    .error

  mov   [p6], rax
  mov   rdi, [p3]

  cmp   rax, rdi
  jne   .error

  mov   rax, SYS_EXIT
  mov   rdi, SUCCESS_CODE
  syscall

.error:
  mov   rax, SYS_EXIT
  mov   rdi, FAILURE_CODE
  syscall

section .data
  p1    dq 0
  p2    dq 0
  p3    dq 0
  p4    dq 0
  p5    dq 0
  p6    dq 0

