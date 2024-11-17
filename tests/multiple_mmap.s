global _start

%include "utils.inc"
%include "malloc.inc"
%include "calloc.inc"
%include "memset.inc"
%include "free.inc"
%include "mmap.inc"

section .text
_start:
  mov   rdi, 2000
  call  malloc

  cmp   rax, 0
  jl    .error

  mov   [p1], rax

  sub   rax, CHUNK_SIZE

  cmp   qword [rax+CHUNK_OFFSET_SIZE], 2000+CHUNK_SIZE
  jne   .error

  mov   rdi, [p1]
  call  free

  mov   rdi, 5000
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

