### Key Notes: Virtual Addresses in Memory

1. **Virtual Addresses**:
    
    - Every address you see in a user-level program is a **virtual address**.
    - Virtual addresses are an abstraction provided by the OS, not the actual physical memory locations.
2. **Role of the OS**:
    
    - The OS uses techniques to **virtualize memory**, mapping virtual addresses to physical memory.
    - Only the OS and hardware know the true physical locations of instructions and data.
3. **Printing Addresses**:
    
    - When you print an address in a program (e.g., a pointer or function location), it is a **virtual address**, not a physical one.
4. **Example Program**:
    
    - A simple C program (`va.c`) demonstrates the locations of:
        - **Code**: Location of the `main()` function.
        - **Heap**: Location of memory allocated using `malloc()`.
        - **Stack**: Location of a local variable.
5. **Output Example**:
    
    - On a 64-bit Mac, the program outputs:
        - Code: `0x1095afe50` (lower in the address space).
        - Heap: `0x1096008c0` (higher than code).
        - Stack: `0x7fff691aea64` (at the opposite end of the virtual address space).
6. **Memory Layout**:
    
    - **Code**: Found at the lower end of the virtual address space.
    - **Heap**: Located above the code.
    - **Stack**: Found at the higher end of the virtual address space.
7. **Translation**:
    
    - All these virtual addresses are translated by the OS and hardware to access their actual physical memory locations.
8. **Key Takeaway**:
    
    - Virtual addresses are an **illusion** created by the OS to simplify memory management and provide isolation between processes.

```c
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    printf("location of code : %p\n", main);
    printf("location of heap : %p\n", malloc(100e6));
    int x = 3;
    printf("location of stack: %p\n", &x);
    return x;
}
```