section .text
global _start

  ; include .inc files
  %include "utils.inc"
  %include "malloc.inc"
  %include "free.inc"
  %include "mmap.inc"

_start:
  ; allocate memory
  mov   rdi, 64
  call  malloc
  mov   rbx, rax      ; Store allocated pointer

  ; write data to the memory
  mov   qword [rbx], 0x1234

  ; free allocated memory
  mov   rdi, rbx
  call  free

  ; exit
  mov   rax, SYS_EXIT
  mov   rdi, SUCCESS_CODE
  syscall

