section .text
; checks if we can merge with next or prev chunk
; @param  rdi: chunk
; @return rax: chunk
coalesce:
  sub   rsp, 0x8

  ; save chunk pointer onto the stack
  mov   [rsp], rdi

  ; check if can coalesce with next chunk
  xor   rax, rax
  mov   rsi, qword [rdi+CHUNK_OFFSET_NEXT]
  cmp   rsi, 0
  je    .prev

  cmp   qword [rsi+CHUNK_OFFSET_INUSE], 0
  sete  al
  movzx rax, al

  cmp   rax, 1
  jne   .prev

  call  _coalesce
  mov   rdi, rax

  ; check if can coalesce with prev chunk
.prev:
  mov   rsi, qword [rdi+CHUNK_OFFSET_PREV]
  cmp   rsi, 0
  je    .return

  cmp   qword [rsi+CHUNK_OFFSET_INUSE], 0
  sete  bl
  movzx rbx, bl

  mov   rdi, rsi
  call  _coalesce
  
.return:
  mov   rax, [rsp]
  add   rsp, 0x8
  ret

; coalesces two adjacent chunks
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

  ; check if we can coalesce adjacent chunk
  call  coalesce

.return:
  xor   rax, rax

  add   rsp, 0x8
  ret

