### Key Notes on Free-Space Management

1. **Definition and Importance**:
    
    - Free-space management is a fundamental aspect of memory management systems, whether for a `malloc` library (managing heap pages) or an OS (managing address space portions).
2. **Fixed-Sized Units**:
    
    - Managing free space is straightforward when divided into fixed-sized units.
    - A simple list of units can be maintained, and the first available unit is returned upon request.
3. **Variable-Sized Units**:
    
    - Managing free space becomes challenging when dealing with variable-sized units.
    - This scenario arises in:
        - User-level memory allocation libraries (e.g., `malloc()` and `free()`).
        - OS-level physical memory management using segmentation for virtual memory.
4. **External Fragmentation**:
    
    - **Definition**: Free space gets fragmented into small, non-contiguous chunks of varying sizes.
    - **Problem**: Requests may fail due to the lack of a single contiguous block, even if the total free space exceeds the request size.
    - **Example**:
        - Total free space: 20 bytes (two chunks of 10 bytes each).
        - Request: 15 bytes.
        - Result: Request fails due to fragmentation, despite sufficient total free space.
5. **Core Problem**:
    
    - **CRUX**: How to manage free space effectively to satisfy variable-sized requests while minimizing fragmentation.
6. **Key Questions**:
    
    - What strategies can be employed to manage free space?
    - How can fragmentation be minimized?
    - What are the time and space overheads of different approaches?

These notes summarize the challenges and considerations in free-space management, particularly in systems handling variable-sized memory allocations.

![alt](chapter-17-1.png)

### Key Assumptions and Concepts:

1. **Interface**:
    
    - `malloc(size_t size)`: Allocates a block of memory of at least the requested size and returns a pointer to it.
    - `free(void *ptr)`: Frees the memory block pointed to by `ptr`. The library must determine the size of the block being freed, as the user does not provide this information.
2. **Heap Management**:
    
    - The memory managed by the allocator is referred to as the **heap**.
    - A **free list** (or similar data structure) is used to track free chunks of memory within the heap.
3. **Fragmentation**:
    
    - **External Fragmentation**: Occurs when free memory is divided into small, non-contiguous blocks, making it difficult to allocate larger chunks.
    - **Internal Fragmentation**: Happens when allocated memory chunks are larger than requested, leading to wasted space within the allocated block. The focus here is primarily on external fragmentation.
4. **Non-relocatable Memory**:
    
    - Once memory is allocated to a program, it cannot be moved by the allocator. This restriction prevents compaction, which could otherwise reduce fragmentation.
5. **Contiguous Memory Region**:
    
    - The allocator manages a single, contiguous region of memory.
    - While the region can grow (e.g., using system calls like `sbrk`), the discussion assumes a fixed-size region for simplicity.
6. **Compaction**:
    
    - Compaction, which involves relocating memory to reduce fragmentation, is not possible in this context because allocated memory is "owned" by the program until explicitly freed.

### Additional Notes:

- The allocator's primary challenge is to efficiently manage free space while minimizing fragmentation.
- External fragmentation is particularly challenging because it limits the ability to allocate large contiguous blocks of memory, even if sufficient total free memory exists.
- Internal fragmentation, though less emphasized here, can also lead to inefficiencies, especially if the allocator consistently over-allocates memory.