### Key Notes on Supporting Large Address Spaces

1. **Current Approach: Entire Address Space in Memory**
    
    - The OS relocates the entire address space of a process into physical memory using base and bounds registers.
    - This includes unused "free" space between the stack and the heap.
2. **Inefficiency of Current Approach**
    
    - **Wasteful Memory Usage**: Unused space between the stack and heap still occupies physical memory.
    - **Scalability Issues**: Hard to run programs when the entire address space cannot fit into memory.
3. **Challenges with Large Address Spaces**
    
    - For small address spaces (e.g., tiny examples), waste seems minimal.
    - For large address spaces (e.g., 32-bit address space of 4 GB), the waste becomes significant:
        - Typical programs use only megabytes of memory but require the entire address space to be resident in memory.
4. **Need for Improvement**
    
    - The base and bounds approach lacks flexibility and is not efficient for large address spaces.
    - A better solution is needed to handle large address spaces with potentially significant free space between the stack and heap.
5. **Crux of the Problem**
    
    - **Key Question**: How can we support a large address space efficiently, especially when there is a lot of unused space between the stack and heap?