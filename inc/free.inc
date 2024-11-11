section .text
; returns the index of the corresponding bin
; @param  rdi: size
; @return rax: bin index
get_bin:
  bsr   rax, rdi

  ; free list is 0-indexed
  mov   rbx, N_BINS - 1
  sub   rax, FIRST_BIN_BSR_INDEX

  cmp   rax, rbx
  cmovge rax, rbx

  mov   rbx, 0
  cmp   rax, rbx
  cmovl rax, rbx

  ret

; adds a chunk of memory to the free list
; @param  rdi: chunk of memory to add
; @return rax: void
add_to_free_list:
  sub   rsp, 0x10

  mov   [rsp], rdi

  ; make sure the heap is not corrupted
  cmp   qword [rdi+CHUNK_OFFSET_MAGIC], MAGIC_VAL_HEAP_CORRUPT
  jne   .error

  mov   rdi, qword [rdi+CHUNK_OFFSET_SIZE]
  call  get_bin
  
  ; save bin index
  mov   qword [rsp+0x8], rax
  
  lea   rsi, [seg_free_list+0x8*rax]
  cmp   qword [rsi], 0
  je    .init_free_list
  
  mov   rdi, [rsp]
  mov   rbx, [rsi]
  
  mov   qword [rdi+CHUNK_OFFSET_NEXT], rbx
  mov   qword [rbx+CHUNK_OFFSET_PREV], rdi
  
  ; update free_list top - LIFO
  mov   rax, qword [rsp+0x8] 
  mov   [seg_free_list+0x8*rax], rdi

  xor   rax, rax
  
  jmp   .return
  
.init_free_list:
  xor   rax, rax
  mov   rdi, [rsp]
  mov   [seg_free_list+0x8*rax], rdi

.return:
  add   rsp, 0x10
  ret

.error
  mov   rax, FAILURE_CODE
  jmp   .return

; frees previously allocated memory
; @param  rdi: pointer to chunk data
; @return rax: void
free:
  ; go to the beginning of the struct
  sub   rdi, CHUNK_SIZE

  ; chunk not in use
  mov   qword [rdi+CHUNK_OFFSET_INUSE], 0
  call  add_to_free_list

  ret

