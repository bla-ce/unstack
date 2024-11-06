section .text
; checks if we can merge with next block
; @param  rdi: chunk
; @return rax: bool
can_coalesce:
  xor   rax, rax
  mov   rsi, qword [rdi+CHUNK_OFFSET_NEXT]
  cmp   rsi, 0
  je    .return

  cmp   qword [rsi+CHUNK_OFFSET_INUSE], 0
  sete  al
  movzx rax, al  

.return:
  ret

; coalesces two adjacent blocks
; @param  rdi: free chunk
; @return rax: chunk
_coalesce:
  ; get next next pointer
  mov   rsi, qword [rdi+CHUNK_OFFSET_NEXT]
  cmp   rsi, 0
  je    .return

  mov   rax, qword [rsi+CHUNK_OFFSET_NEXT]
  mov   rbx, qword [rsi+CHUNK_OFFSET_SIZE]

  mov   qword [rdi+CHUNK_OFFSET_NEXT], rax
  add   qword [rdi+CHUNK_OFFSET_SIZE], rbx

.return:
  mov   rax, rdi

  ret

; frees previously allocated memory
; @param  rdi: pointer to chunk data
; @return rax: void
free:
  sub   rsp, 0x8

  ; go to the beginning of the struct
  sub   rdi, CHUNK_SIZE
  mov   [rsp], rdi

  ; chunk not in use
  mov   qword [rdi+CHUNK_OFFSET_INUSE], 0

  ; check if we can coalesce adjacent block
  call  can_coalesce

  cmp   rax, 0
  je    .return

  mov   rdi, [rsp]
  call  _coalesce

.return:
  xor   rax, rax

  add   rsp, 0x8
  ret
