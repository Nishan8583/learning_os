### Key Notes on "OS Support for Segmentation"

1. **Memory Savings with Segmentation**:
    
    - Segmentation allows unused space (e.g., between stack and heap) to remain unallocated in physical memory.
    - This enables:
        - More address spaces to fit into physical memory.
        - Support for large and sparse virtual address spaces per process.
2. **OS Responsibilities on Context Switch**:
    
    - The **segment registers** must be saved and restored during a context switch.
    - Each process has its own virtual address space, and the OS ensures the correct segment registers are set before resuming a process.
3. **Handling Segment Growth**:
    
    - When a program requests memory (e.g., via `malloc()`):
        - If the existing heap can service the request, it allocates space and returns a pointer.
        - If the heap needs to grow, the memory-allocation library makes a **system call** (e.g., `sbrk()` in UNIX) to request more space.
    - The OS may:
        - Grant the request by increasing the segment size and updating the segment size register.
        - Reject the request if physical memory is insufficient or if the process exceeds its allowed memory usage.
4. **External Fragmentation**:
    
    - **Definition**: Physical memory becomes fragmented with small, non-contiguous free spaces, making it difficult to allocate or grow segments.
    - Example:
        - A process requests a 20KB segment.
        - There is 24KB free, but it is spread across three non-contiguous chunks, so the request cannot be satisfied.
5. **Solutions to External Fragmentation**:
    
    - **Compaction**:
        
        - Rearranges physical memory to consolidate free space into a contiguous block.
        - Steps:
            - Stop running processes.
            - Copy their data to a contiguous region.
            - Update segment registers to reflect new physical locations.
        - **Drawbacks**:
            - Expensive in terms of memory and CPU usage.
            - Makes growing existing segments harder, potentially requiring further rearrangement.
    - **Free-list Management Algorithms**:
        
        - Aim to keep large contiguous memory extents available for allocation.
        - Examples:
            - **Best-fit**: Allocates the smallest free block that fits the request.
            - **Worst-fit**: Allocates the largest free block.
            - **First-fit**: Allocates the first free block that fits the request.
            - **Buddy Algorithm**: Splits memory into power-of-2-sized blocks for efficient allocation.
        - **Limitation**: External fragmentation cannot be fully eliminated, but good algorithms minimize it.
6. **Key Insight**:
    
    - The existence of numerous memory allocation algorithms highlights the lack of a perfect solution.
    - The only way to avoid external fragmentation entirely is to avoid variable-sized memory allocations, as discussed in later chapters.