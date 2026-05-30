### Key Notes for Quick Review

#### 14.5 Underlying OS Support

- **`malloc()` and `free()`**:
    
    - These are **library calls**, not system calls.
    - They manage memory within the program's **virtual address space**.
    - Built on system calls like `brk` and `sbrk`.
- **`brk` and `sbrk`**:
    
    - `brk`: Adjusts the **program's break** (end of the heap) to increase or decrease heap size.
    - `sbrk`: Similar to `brk`, but takes an **increment** instead of an absolute address.
    - **Warning**: Do not call `brk` or `sbrk` directly; they are managed by the memory-allocation library.
- **`mmap()`**:
    
    - Can allocate **anonymous memory regions** (not tied to files, but to swap space).
    - Useful for creating memory regions that can act like a heap.
    - **Tip**: Refer to the `mmap()` manual for detailed usage.

#### 14.6 Other Calls

- **`calloc()`**:
    
    - Allocates memory and **initializes it to zero**.
    - Prevents errors from **uninitialized reads**.
- **`realloc()`**:
    
    - Resizes an already allocated memory region (e.g., for arrays).
    - Creates a **new larger memory region**, copies old data into it, and returns a pointer to the new region.

These notes summarize the key concepts and functions related to memory allocation and system calls for quick reference.