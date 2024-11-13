global _start

%include "utils.inc"
%include "malloc.inc"
%include "calloc.inc"
%include "memset.inc"
%include "free.inc"
%include "mmap.inc"

section .text
_start:
  mov   rdi, 16
  call  malloc

  cmp   rax, 0
  jl    .error

  mov   [p1], rax

  mov   rdi,  32
  call  malloc

  cmp   rax, 0
  jl    .error

  mov   [p2], rax

  mov   rdi,  64
  call  malloc

  cmp   rax, 0
  jl    .error

  mov   [p3], rax

  mov   rdi, [p1]
  call  free

  mov   rdi, [p2]
  call  free

  mov   rdi, [p3]
  call  free

.exit:
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

