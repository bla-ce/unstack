section .text
; creates a new mapping in the virtual address space
; @param  rdi: size
; @return rax: pointer to the mapped area
mmap:
  mov   rsi, rdi
  mov   rax, SYS_MMAP
  xor   rdi, rdi
  mov   rdx, PROT_READ
  or    rdx, PROT_WRITE
  mov   r10, MAP_SHARED
  or    r10, MAP_ANONYMOUS
  xor   r8, r8
  xor   r9, r9
  syscall

  ret

; deletes the mappings for the specified address range
; @param  rdi: addr
; @param  rsi: size
; @return rax: status
unmap:
  mov   rax, SYS_MUNMAP  ; other registers already valid
  syscall

  ret

