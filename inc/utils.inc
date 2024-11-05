section .data
  SYS_MMAP    equ 9
  SYS_MUNMAP  equ 11
  SYS_EXIT    equ 60

  SUCCESS_CODE  equ 0
  FAILURE_CODE  equ -1

  PAGE_SIZE equ 0x1000

  PROT_READ     equ 1
  PROT_WRITE    equ 2
  MAP_ANONYMOUS equ 32
  MAP_SHARED    equ 1
  MAP_PRIVATE   equ 2
  
