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

  ; go to beginning of struct
  sub   rax, CHUNK_SIZE

  cmp   qword [rax+CHUNK_OFFSET_INUSE], 1
  jne   .error

  cmp   qword [rax+CHUNK_OFFSET_NEXT], 0
  jne   .error

  cmp   qword [rax+CHUNK_OFFSET_SIZE], 4096 ; page size - chunk_size
  jne   .error

  mov   rdi, 15
  call  malloc

  cmp   rax, 0
  jl    .error

  mov   [p2], rax

  sub   rax, CHUNK_SIZE

  cmp   qword [rax+CHUNK_OFFSET_INUSE], 1
  jne   .error

  cmp   qword [rax+CHUNK_OFFSET_NEXT], 0
  jne   .error

  cmp   qword [rax+CHUNK_OFFSET_SIZE], 4096 ; page size - chunk_size
  jne   .error

  mov   rdi, 8000
  call  malloc

  cmp   rax, 0
  jl    .error

  mov   [p3], rax

  sub   rax, CHUNK_SIZE

  cmp   qword [rax+CHUNK_OFFSET_INUSE], 1
  jne   .error

  cmp   qword [rax+CHUNK_OFFSET_NEXT], 0
  jne   .error

  cmp   qword [rax+CHUNK_OFFSET_SIZE], 8192 ; x2 page size - chunk_size
  jne   .error

  mov   rdi, [p2]
  call  free

  mov   rax, [p2]

  sub   rax, CHUNK_SIZE

  cmp   qword [rax+CHUNK_OFFSET_INUSE], 0
  jne   .error

  cmp   qword [rax+CHUNK_OFFSET_SIZE], 4096 ; page size - chunk_size
  jne   .error

  mov   rdi, [p3]
  call  free

  mov   rdi, [p1]
  call  free

  mov   rdi, 4000
  call  malloc

  cmp   rax, 0
  jl    .error

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

