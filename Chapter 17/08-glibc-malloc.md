### Key Notes on Understanding glibc malloc

#### General Overview

- **Heap Memory**:
    
    - Fascination with how heap memory is obtained, managed, and exploited.
    - Memory allocators include `dlmalloc`, `ptmalloc2` (glibc), `jemalloc`, `tcmalloc`, etc.
    - Performance of memory-hungry applications depends heavily on the memory allocator.
- **glibc malloc**:
    
    - Forked from `dlmalloc` and integrated into glibc in 2006.
    - Added threading support for better performance in multi-threaded applications.

---

#### System Calls

- **brk**: Used for main thread heap allocation (contiguous memory).
- **mmap**: Used for thread-specific heap allocation or large memory requests (non-contiguous memory).

---

#### Threading and Per-Thread Arenas

- **Threading Support**:
    
    - `dlmalloc`: Shared freelist, causing contention in multi-threaded apps.
    - `ptmalloc2`: Introduced per-thread arenas, where each thread maintains its own heap and freelist.
- **Per-Thread Arena**:
    
    - Separate heap for each thread to avoid contention.
    - Main thread uses the "main arena."
    - Threads create their own arenas using `mmap`.
- **Arena Limits**:
    
    - **32-bit systems**: Number of arenas = 2 × number of cores.
    - **64-bit systems**: Number of arenas = 8 × number of cores.
    - Threads share arenas if the limit is exceeded.

---

#### Multiple Heaps and Data Structures

- **Heap Info**: Header for each heap in a thread arena.
- **Arena Header**: Single header for all heaps in a thread arena, containing bin and chunk info.
- **Chunk Header**: Each memory chunk has its own header.

---

#### Chunk Types

1. **Allocated Chunk**:
    
    - Stores user data and metadata (e.g., size, flags).
    - Flags:
        - `P`: Previous chunk is in use.
        - `M`: Chunk is mmap'd.
        - `N`: Chunk belongs to a thread arena.
2. **Free Chunk**:
    
    - Metadata includes size, forward (`fd`), and backward (`bk`) pointers for bin linkage.
3. **Top Chunk**:
    
    - The topmost chunk of an arena, used when no free chunks are available.
    - Extended using `sbrk` (main arena) or `mmap` (thread arena).
4. **Last Remainder Chunk**:
    
    - Remainder from the most recent split of a small request.
    - Improves locality of reference by allocating consecutive chunks close to each other.

---

#### Bins (Freelist Data Structures)

- **Fast Bin**:
    
    - Chunks: 16–80 bytes.
    - Single-linked list (LIFO).
    - No coalescing (to improve speed but may cause fragmentation).
- **Unsorted Bin**:
    
    - Recently freed chunks of any size.
    - Circular double-linked list.
    - Speeds up allocation by reusing recently freed chunks.
- **Small Bin**:
    
    - Chunks: <512 bytes.
    - Circular double-linked list (FIFO).
    - Coalescing enabled to reduce fragmentation.
- **Large Bin**:
    
    - Chunks: ≥512 bytes.
    - Circular double-linked list.
    - Chunks stored in decreasing order of size.
    - Coalescing enabled.

---

#### Memory Allocation and Deallocation

- **malloc**:
    
    - Fast Bin: Retrieves the first chunk from the binlist.
    - Small/Large Bin: Searches for a suitable chunk, splits it if necessary, and returns the user chunk.
    - Top Chunk: Used if no free chunks are available in bins.
- **free**:
    
    - Fast Bin: Adds the chunk to the front of the binlist.
    - Small/Large Bin: Coalesces adjacent free chunks and adds them to the unsorted bin.

---

#### Key Features of glibc malloc

- **Thread-Specific Arenas**:
    
    - Avoids contention by maintaining separate heaps for threads.
    - Limits the number of arenas based on system cores.
- **Bins for Free Chunks**:
    
    - Categorized into fast, unsorted, small, and large bins based on chunk size.
    - Optimized for speed and memory reuse.
- **Top Chunk**:
    
    - Used as a fallback when no free chunks are available in bins.
    - Extended dynamically using system calls.
- **Last Remainder Chunk**:
    
    - Enhances memory locality by allocating consecutive chunks close to each other.

---

#### Performance Considerations

- **Fast Bins**: Prioritize speed but may cause fragmentation.
- **Coalescing**: Reduces fragmentation but slows down deallocation.
- **Threading**: Per-thread arenas improve performance in multi-threaded applications.

---

#### Practical Example

- **Main Thread**:
    
    - Uses `brk` to allocate contiguous heap memory.
    - Freed memory is added to the main arena's bins.
- **Thread 1**:
    
    - Uses `mmap` to allocate non-contiguous heap memory.
    - Freed memory is added to the thread arena's bins.
- **Arena Sharing**:
    
    - Threads share arenas when the arena limit is exceeded.
    - Threads block if no free arena is available.

---

#### Summary

- glibc malloc is a highly optimized memory allocator designed for multi-threaded applications.
- It uses per-thread arenas, bins, and dynamic memory allocation strategies to balance speed and memory efficiency.
- Understanding its internal mechanisms (e.g., arenas, bins, chunks) is crucial for optimizing application performance and debugging memory-related issues.


Reference: https://sploitfun.wordpress.com/2015/02/10/understanding-glibc-malloc/ 