### Key Notes for Quick Review

#### **41.5 Measuring File Locality**

1. **Namespace Locality Analysis**:
    
    - Analyzed file system access traces (SEER traces) to measure "distance" between file accesses in the directory tree.
    - **Distance Metric**:
        - Distance = How far up the directory tree you must go to find the common ancestor of two files.
        - Example:
            - File `f` opened, then reopened → Distance = 0.
            - File `dir/f` followed by `dir/g` → Distance = 1 (same directory).
            - File `proj/src/foo.c` followed by `proj/obj/foo.o` → Distance = 2 (common ancestor: `proj`).
2. **SEER Trace Observations**:
    
    - 7% of file accesses were to the same file (distance = 0).
    - ~40% of file accesses were to the same file or files in the same directory (distance = 0 or 1).
    - ~25% of file accesses had a distance of 2, indicating multi-level directory structures (e.g., `proj/src` and `proj/obj`).
3. **Random Trace Comparison**:
    
    - Randomly ordered file accesses showed less namespace locality.
    - However, some locality still exists due to the shared root ancestor.

---

#### **41.6 The Large-File Exception in FFS**

1. **Problem with Large Files**:
    
    - Large files can fill up a block group, leaving no space for related files, which reduces file-access locality.
2. **FFS Large-File Placement Policy**:
    
    - **Without Large-File Exception**:
        - A large file fills up one block group entirely, leaving no room for related files.
        - Example:
            
            ```
            group   inodes      data
            0       /a--------  /aaaaaaaaa aaaaaaaaaa aaaaaaaaaa a---------
            1       ----------  ---------- ---------- ----------
            2       ----------  ---------- ---------- ----------
            ```
            
    - **With Large-File Exception**:
        - Large files are spread across multiple block groups after the first few blocks.
        - Example (chunk size = 5 blocks):
            
            ```
            group   inodes      data
            0       /a--------  /aaaaa---- ---------- ----------
            1       ----------  aaaaa----- ---------- ----------
            2       ----------  aaaaa----- ---------- ----------
            3       ----------  aaaaa----- ---------- ----------
            4       ----------  aaaaa----- ---------- ----------
            5       ----------  aaaaa----- ---------- ----------
            6       ----------  ---------- ---------- ----------
            ```
            
3. **Performance Trade-offs**:
    
    - Spreading large files across groups increases seek time for sequential access.
    - To mitigate this, **chunk size** is chosen carefully to balance seek time and transfer time (amortization).
4. **Amortization Example**:
    
    - Disk positioning time (seek + rotation) = 10ms.
    - Disk transfer rate = 40 MB/s.
    - To spend equal time seeking and transferring:
        
        ```
        Chunk size = 40 MB/s × 10 ms = 409.6 KB
        ```
        
    - For higher bandwidth utilization:
        - 90% peak bandwidth → Chunk size ≈ 3.6 MB.
        - 99% peak bandwidth → Chunk size ≈ 39.6 MB.
5. **FFS Implementation**:
    
    - FFS uses a simple approach based on inode structure:
        - First 12 direct blocks in the same group as the inode.
        - Each subsequent indirect block (and its pointed blocks) placed in a different group.
        - Example: With 4KB block size and 32-bit disk addresses, every 1024 blocks (4MB) are placed in separate groups, except the first 48KB.
6. **Disk Trends**:
    
    - Disk transfer rates improve faster than mechanical aspects (seek time, rotation speed).
    - Mechanical costs become relatively more expensive over time.
    - To reduce these costs, larger chunks of data should be transferred between seeks.