section .text
; frees previously allocated memory
; @param  rdi: pointer to chunk data
; @return rax: void
free:
  ; go to the beginning of the struct
  sub   rdi, CHUNK_SIZE

  ; chunk not in use
  mov   qword [rdi+CHUNK_OFFSET_INUSE], 0

  xor   rax, rax

  ret
