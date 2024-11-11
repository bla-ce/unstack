section .text
; allocates the specified number of bytes and initializes them to zero 
; @param  rdi: size
; @return rax: chunk or null if failed
calloc:
  ; call malloc
  call  malloc

  cmp   rax, 0
  jl    .return

  ; rax points to the data
  ; go to the beginning of the header
  sub   rax, CHUNK_SIZE
  mov   rcx, qword [rax+CHUNK_OFFSET_SIZE]
  sub   rcx, CHUNK_SIZE

  ; go back to data and set bytes to 0
  add   rax, CHUNK_SIZE
  mov   rdi, rax
  mov   rsi, 0
  mov   rdx, rcx
  call  memset

.return:
  ret

