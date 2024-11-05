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
; gets memory from the mmap syscall
; @param  rdi: page aligned size
; @return rax: returns a pointer to the allocated area
request_from_os:
  call    mmap

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

; allocates a chunk of memory of rsi bytes
; @param  rdi: size
; @return rax: pointer to chunk data
malloc:
  sub   rsp, 0x10

  ; make sure the size if greater than 0
  cmp   rdi, 0
  je    .null
  jl    .error 

  ; add chunk_size to the size
  add   rdi, CHUNK_SIZE

  ; TODO: getpagesize()
  ; align the size by the page size
  mov   rsi, PAGE_SIZE
  call  _align

  ; save aligned size onto the stack
  mov   qword [rsp], rax

  mov   rdi, rax
  call  request_from_os

  cmp   rax, 0
  jl    .error

  ; initialize chunk
  ; store size of the data, needs to sub chunk size
  mov   rbx, qword [rsp]
  sub   rbx, CHUNK_SIZE
  mov   qword [rax+CHUNK_OFFSET_SIZE], rbx
  mov   qword [rax+CHUNK_OFFSET_INUSE], 1
  mov   qword [rax+CHUNK_OFFSET_NEXT], 0

  ; save chunk address onto the stack
  mov   [rsp+0x8], rax

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

  ; returns pointer to the data
  add   rax, CHUNK_OFFSET_DATA

  jmp   .return

.init_heap:
  ; initialize the heap
  mov   qword [heap_start], rax 

  ; returns pointer to the data
  add   rax, CHUNK_OFFSET_DATA

  jmp   .return

.error:
  mov   rax, FAILURE_CODE
  jmp   .return

.null:
  xor   rax, rax

.return:
  add   rsp, 0x10
  ret

