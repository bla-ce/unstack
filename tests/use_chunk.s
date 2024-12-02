global _start

%include "utils.inc"
%include "malloc.inc"
%include "calloc.inc"
%include "memset.inc"
%include "free.inc"
%include "mmap.inc"

section .text
_start:
  mov   rdi, 1024
  call  malloc

  cmp   rax, 0
  jl    .error

  mov   [p1], rax

  mov   rdi, rax

  mov   rax, 0xFE
  mov   rcx, 128
  rep   stosq

  mov   rdi, [p1]
  call  free
  
  mov   rdi, 12
  call  malloc

  mov   rdi, 64
  call  malloc

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

