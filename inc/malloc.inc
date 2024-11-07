section .bss
  heap_start  resq 1

section .data
  CHUNK_SIZE equ end_chunk-chunk

  CHUNK_OFFSET_SIZE   equ 0
  CHUNK_OFFSET_INUSE  equ 0x8
  CHUNK_OFFSET_NEXT   equ 0x10
  CHUNK_OFFSET_DATA   equ 0x18

  chunk:
    size  dq 0
    inuse dq 0
    next  dq 0
  end_chunk:

section .text
; checks if the chunk can be splitted
; @param  rdi: chunk
; @param  rsi: size
; @return rax: bool
can_split:
  xor   rax, rax

  ; needs to remove header size from the chunk
  mov   rbx, qword [rdi+CHUNK_OFFSET_SIZE]
  sub   rbx, CHUNK_SIZE

  cmp   rbx, rsi
  setg  al
  movzx rax, al

  ret

; splits the chunk of memory to allocate the smallest size
; @param  rdi: chunk
; @param  rsi: size
; @return rax: pointer to new chunk
_split:
  sub   rsp, 0x20

  mov   [rsp], rdi
  mov   qword [rsp+0x8], rsi
  
  ; save current next address and chunk size onto the stack
  mov   rax, qword [rdi+CHUNK_OFFSET_NEXT]
  mov   [rsp+0x10], rax

  mov   rax, qword [rdi+CHUNK_OFFSET_SIZE]
  mov   qword [rsp+0x18], rax
  
  ; get new next address and size
  mov   rsi, rdi
  add   rsi, qword [rsp+0x8]
  mov   qword [rdi+CHUNK_OFFSET_NEXT], rsi

  mov   rbx, qword [rsp+0x8]
  mov   qword [rdi+CHUNK_OFFSET_SIZE], rbx
  mov   qword [rdi+CHUNK_OFFSET_INUSE], 1
  
  ; init second chunk
  mov   rax, qword [rsp+0x10]
  mov   qword [rsi+CHUNK_OFFSET_INUSE], 0
  mov   qword [rsi+CHUNK_OFFSET_NEXT], rax

  mov   rbx, qword [rsp+0x18]
  sub   rbx,  qword [rsp+0x8]
  mov   qword [rsi+CHUNK_OFFSET_SIZE], rbx

  mov   rax, rdi

  add   rsp, 0x20
  ret

; gets memory from the mmap syscall
; @param  rdi: size
; @return rax: returns a pointer to the allocated area
request_from_os:
  mov   rsi, PAGE_SIZE
  call  _align
  
  mov   rdi, rax
  call  mmap

  ret

; aligns the size in rdi by the value in rsi
; @param  rdi: size
; @param  rsi: alignment
; @return rax: aligned size
_align:
  ; alignment - 1
  dec   rsi

  ; (size + alignment - 1)
  mov   rax, rdi
  add   rax, rsi

  ; aligned = (size+alignment-1) & ~(alignment-1)
  not   rsi
  and   rax, rsi

  ret

; searches a valid previously allocated chunk
; @param  rdi: size
; @return rax: pointer to chunk, 0 if no valid chunk
search_chunk:
  sub   rsp, 0x10

  mov   qword [rsp], rdi
  mov   qword [rsp+0x8], 0

  mov   rdi, [heap_start]

.loop:
  ; check if chunk in use
  cmp   qword [rdi+CHUNK_OFFSET_INUSE], 0 
  je    .block_not_inuse

.next:
  mov   rsi, qword [rdi+CHUNK_OFFSET_NEXT]

  ; if next pointer is null, no chunk available
  cmp   rsi, 0
  je    .end_heap

  ; go to next pointer
  mov   rdi, rsi

  jmp   .loop

.block_not_inuse:
  ; get aligned size to allocate
  mov   rax, qword [rsp]

  ; compare the chunk size with needed size
  cmp   qword [rdi+CHUNK_OFFSET_SIZE], rax
  jl    .next

  ; check if already found an available chunk
  cmp   qword [rsp+0x8], 0
  je    .init_best_fit

  ; if yes, check if this chunk is smaller
  mov   rsi, [rsp+0x8]
  ; get best fit chunk size to compare
  mov   rbx, qword [rsi+CHUNK_OFFSET_SIZE]
  cmp   rbx, rax
  jle   .next

  ; save best fit if smaller
  mov   [rsp+0x8], rdi

  jmp   .next

.init_best_fit:
  mov   [rsp+0x8], rdi
  jmp   .next

.end_heap:
  cmp   qword [rsp+0x8], 0
  je    .not_found

  mov   rdi, [rsp+0x8]

  ; use this chunk
  mov   qword [rdi+CHUNK_OFFSET_INUSE], 1

  ; return chunk
  mov   rax, rdi
  jmp   .return

.not_found:
  xor   rax, rax

.return:
  add   rsp, 0x10
  ret

; allocates a chunk of memory of rsi bytes
; @param  rdi: size
; @return rax: pointer to chunk data
malloc:
  sub   rsp, 0x18

  ; make sure the size if greater than 0
  cmp   rdi, 0
  je    .null
  jl    .error 

  ; add chunk_size to the size
  add   rdi, CHUNK_SIZE

  ; align the size by the page size
  mov   rsi, 0x8
  call  _align

  ; save aligned size onto the stack
  mov   qword [rsp], rax

  ; check if the heap is empty
  cmp   qword [heap_start], 0
  je    .request

  ; search for an available chunk of data
  mov   rdi, rax
  call  search_chunk

  cmp   rax, 0
  je    .request

  ; return the chunk
  jmp   .found

.request:
  ; call mmap
  mov   rdi, qword [rsp]
  call  request_from_os

  cmp   rax, 0
  jl    .error
  
  ; save chunk address onto the stack
  mov   [rsp+0x8], rax

  ; initialize chunk
  ; store size of the data
  mov   rdi, qword [rsp]

  ; if request from os, size needs to be page aligned
  mov   rsi, PAGE_SIZE
  call  _align 

  mov   rsi, [rsp+0x8]

  mov   qword [rsi+CHUNK_OFFSET_SIZE], rax
  mov   qword [rsi+CHUNK_OFFSET_INUSE], 1
  mov   qword [rsi+CHUNK_OFFSET_NEXT], 0

  ; check if the heap has been initialized
  cmp   qword [heap_start], 0
  je    .init_heap

  ; add chunk to the heap linked list
  ; look for last node
  mov   rdi, [heap_start]

.loop:
  cmp   qword [rdi+CHUNK_OFFSET_NEXT], 0
  je    .append_chunk

  mov   rsi, qword [rdi+CHUNK_OFFSET_NEXT]
  mov   rdi, rsi

  jmp   .loop

.append_chunk:
  mov   rsi, [rsp+0x8]
  mov   qword [rdi+CHUNK_OFFSET_NEXT], rsi

  jmp   .found

.init_heap:
  ; initialize the heap
  mov   rax, [rsp+0x8]
  mov   qword [heap_start], rax 

  jmp   .found

.error:
  mov   rax, FAILURE_CODE
  jmp   .return

.null:
  xor   rax, rax
  jmp   .return

; once a chunk has been found, find if can be splitted
.found:
  ; save chunk pointer onto the stack
  mov   [rsp+0x10], rax

  mov   rdi, rax 
  mov   rsi, qword [rsp]
  call  can_split

  mov   rdi, rax
  mov   rax, [rsp+0x10]
  add   rax, CHUNK_OFFSET_DATA

  cmp   rdi, 0
  je    .return

  mov   rdi, [rsp+0x10]
  mov   rsi, qword [rsp]
  call  _split

  add   rax, CHUNK_OFFSET_DATA

.return:
  add   rsp, 0x18
  ret

