### Key Notes: Growing the Heap

- **Heap Exhaustion**:
    
    - When the heap runs out of space, the simplest approach is to fail gracefully by returning `NULL`.
    - This is an acceptable and honorable approach in some scenarios.
- **Dynamic Heap Growth**:
    
    - Most traditional memory allocators start with a small heap size.
    - When the heap runs out of space, they request additional memory from the operating system.
- **System Call for Heap Growth**:
    
    - Allocators typically use system calls like `sbrk` (common in UNIX systems) to grow the heap.
    - The OS:
        1. Finds free physical memory pages.
        2. Maps these pages into the requesting process's address space.
        3. Returns the new end address of the heap to the allocator.
- **Outcome**:
    
    - The allocator uses the newly allocated memory chunks to continue servicing allocation requests.