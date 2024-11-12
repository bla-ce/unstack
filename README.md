# Custom Heap Allocator in Netwide Assembly

> [!WARNING]
> This custom heap allocator implementation is for educational or experimental purposes only.
> It is NOT suitable yet for production use due to the following limitations:
> 
> 1. Limited Error Handling: This implementation may lack robust error-checking mechanisms
>    and could fail unpredictably with large or unexpected allocations.
>
> 2. No Thread Safety: This allocator is not designed to handle concurrent memory allocations
>    from multiple threads. Use in a multi-threaded environment could lead to data races or crashes.
>
> 3. No Memory Protection: this implementation does not
>    guarantee that freed memory cannot be accessed, which may lead to undefined behavior.

## Usage

### Allocation

To allocate memory, use the malloc function:

```asm

mov     rdi, 64          ; Size in bytes
call    malloc           ; Allocates 64 bytes and returns pointer in RAX

cmp     rax, 0
jl      .error           ; make sure it didn't fail 
```

### Deallocation

To free allocated memory, use the free function:

```asm

mov     rdi, rax         ; Address of the memory chunk to free
call    free             ; Deallocates memory at the given address

cmp     rax, 0
jl      .error           ; make sure it didn't fail 
```

### Other functions
To allocate memory and initialise all bytes to zero, use the calloc function:

```asm

mov     rdi, 10 ; number of elements
mov     rsi, 64 ; size of each element
call    calloc  ; Allocate 10 elements of 64 bytes and initialise to 0 

cmp     rax, 0
jl      .error           ; make sure it didn't fail 
```

## Example

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
    mov     rdi, 64
    call    malloc

    cmp     rax, 0
    jl      .error

    mov     rbx, rax      ; Store allocated pointer

    ; write data to the memory
    mov     [rbx], 0x1234

    ; free allocated memory
    mov     rdi, rbx
    call    free

    cmp     rax, 0
    jl      .error

    ; exit
    mov     rax, SYS_EXIT
    mov     rdi, SUCCESS_CODE
    syscall

.error:
    mov     rax, SYS_EXIT
    mov     rdi, FAILURE_CODE
    syscall

```

