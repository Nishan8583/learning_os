### Key Notes: Embedding a Free List

1. **Concept of Free List**:
    
    - A free list is a structure used to manage free chunks of memory in the heap.
    - The list is built inside the free space itself, as the memory-allocation library cannot use `malloc()` internally.
2. **Initialization**:
    
    - Example assumes a 4KB heap managed as a free list.
		```c
		typedef struct __node_t { 
		int size; 
		struct __node_t *next; 
		} node_t;
		```


    - A single entry is initialized in the free list with the size of the heap minus the header size.
    - Code snippet:
        
        ```c
node_t *head = mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_ANON|MAP_PRIVATE, -1, 0);
head->size = 4096 - sizeof(node_t);
head->next = NULL;
        ```
        
    - After initialization, the free list contains one node of size 4088 bytes.
![[chapter-17-6.png]]
3. **Memory Allocation**:
    
    - When a memory request (e.g., 100 bytes) is made:
        - A chunk large enough to accommodate the request is found.
        - The chunk is split into two:
            - One chunk for the request (including the header).
            - Remaining free chunk.
        - Example: A 100-byte request results in a 108-byte allocation (100 bytes + 8-byte header), leaving 3980 bytes free.
![[chapter-17-7.png]]
3. **Multiple Allocations**:
    
    - After three 100-byte allocations:
        - The first 324 bytes of the heap are allocated (3 headers + 3 chunks of 100 bytes).
        - The free list contains a single node of size 3764 bytes.
![[chapter-17-8.png]]
3. **Freeing Memory**:
    
    - When a chunk is freed (e.g., the middle chunk):
        - The library calculates the size of the freed region.
        - The freed chunk is added back to the free list (e.g., at the head).
        - Example: After freeing the middle chunk, the free list contains:
            - A small free chunk (100 bytes).
            - A large free chunk (3764 bytes).
![[chapter-17-9.png]]
3. **Fragmentation**:
    
    - Without coalescing, freeing multiple chunks leads to fragmentation.
    - Example: After freeing all chunks, the memory appears fragmented despite being fully free.
4. **Coalescing**:
    
    - To resolve fragmentation:
        - Traverse the free list and merge neighboring chunks.
        - This restores the heap to a contiguous free space.
5. **Key Challenges**:
    
    - Fragmentation is a common issue in memory management.
    - Coalescing is essential to maintain efficient memory usage.

### Visualizations:

- **Initial Heap**: Single free chunk of 4088 bytes.
- **After Allocation**: Allocated regions interspersed with remaining free space.
- **After Freeing**: Fragmented free list with multiple small chunks.
- **After Coalescing**: Restored contiguous free space.