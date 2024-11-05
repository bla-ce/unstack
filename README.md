# Custom Heap Allocator in Netwide Assembly

## Usage

### Allocation

To allocate memory, use the malloc function:

```asm

mov rdi, 64          ; Size in bytes
call malloc          ; Allocates 64 bytes and returns pointer in RAX
```

### Deallocation

To free allocated memory, use the free function:

```asm

mov rdi, rax         ; Address of the memory chunk to free
call free            ; Deallocates memory at the given address
```

## Tests

Run tests with 'make test'. Tests in the tests/ directory are executed with strace for system call tracing, exiting with 0 on success or -1 on failure.

```Makefile
    
make test
```

## Example

A simple example of using the allocator is provided in example.asm:

```asm

section .text
    global _start

    ; include .inc files
    %include "utils.inc"
    %include "malloc.inc"
    %include "free.inc"
    %include "mmap.inc"

_start:
    ; allocate memory
    mov rdi, 64
    call malloc
    mov rbx, rax      ; Store allocated pointer

    ; write data to the memory
    mov [rbx], 0x1234

    ; free allocated memory
    mov rdi, rbx
    call free

    ; exit
    mov rax, SYS_EXIT
    mov rdi, SUCCESS_CODE
    syscall
```

