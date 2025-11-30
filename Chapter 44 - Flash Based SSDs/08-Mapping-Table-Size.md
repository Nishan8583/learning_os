### Key Notes on Flash Translation Layer (FTL) Mapping Techniques

#### 1. **Mapping Table Size**

- **Page-Level Mapping**:
    
    - Each 4KB page requires a 4-byte entry in the mapping table.
    - For a 1TB SSD, this results in 1GB of memory needed for mapping.
    - Impractical due to high memory requirements.
- **Block-Based Mapping**:
    
    - Reduces mapping information by using one pointer per block instead of per page.
    - Logical address space is divided into chunks (size of physical blocks).
    - Logical block address = **Chunk Number + Offset**.
    - Example:
        - Logical blocks 2000, 2001, 2002, 2003 → Physical block 1 (pages 4, 5, 6, 7).
        - Mapping: `Chunk 500 → Block 1 (starting at page 4)`.
    - **Performance Issue**:
        - Small writes (less than block size) require reading the entire block, modifying it, and writing it back.
        - Causes **write amplification**, reducing performance.

#### 2. **Hybrid Mapping**

- **Purpose**: Combines the benefits of page-level and block-level mapping to balance memory usage and performance.
- **Mechanism**:
    - Uses **log blocks** for flexible writing.
    - Maintains two mapping tables:
        - **Log Table**: Per-page mappings for log blocks.
        - **Data Table**: Per-block mappings for data blocks.
    - FTL first checks the log table for a logical block’s location. If not found, it checks the data table.
- **Log Block Management**:
    - To reduce the number of log blocks, the FTL periodically merges log blocks into data blocks.
    - Three types of merges:
        1. **Switch Merge**:
            - If all pages in a log block are written sequentially, the log block becomes the new data block.
            - Example:
                - Logical pages 1000, 1001, 1002, 1003 → Physical block 2 (pages 8, 9, 10, 11).
                - Overwritten with a’, b’, c’, d’ in log block 0 (pages 0, 1, 2, 3).
                - Log block 0 becomes the new data block, and block 2 is erased.
        2. **Partial Merge**:
            - If only some pages in a block are overwritten, the FTL reads the remaining pages from the old block and appends them to the log block.
            - Example:
                - Logical blocks 1000, 1001 overwritten → Log block 0 (pages 0, 1).
                - Logical blocks 1002, 1003 read from block 2 and appended to log block.
        3. **Full Merge**:
            - Requires gathering pages from multiple blocks to create a new data block.
            - Example:
                - Logical blocks 0, 4, 8, 12 written to log block A.
                - FTL must read and reconcile blocks 1, 2, 3, 5, 6, 7, etc., to create new data blocks.
            - **Performance Impact**: Full merges are costly and should be minimized.

#### 3. **Page Mapping Plus Caching**

- **Approach**:
    - Cache only the active parts of the FTL in memory to reduce memory usage.
- **Advantages**:
    - Works well if the workload accesses a small set of pages (locality of reference).
    - Reduces memory overhead while maintaining high performance.
- **Disadvantages**:
    - If the working set exceeds memory capacity:
        - Extra flash reads are required to load missing mappings.
        - Evicting dirty mappings incurs additional writes.
- **Trade-Off**:
    - Effective for workloads with high locality but can degrade performance for workloads with poor locality.

#### 4. **Key Challenges and Considerations**

- **Write Amplification**:
    - Block-based mapping increases write amplification for small writes.
- **Memory Overhead**:
    - Page-level mapping requires significant memory for large devices.
- **Merge Operations**:
    - Switch merges are efficient, but partial and full merges can degrade performance.
- **Caching**:
    - Balancing memory usage and performance is critical, especially for workloads with varying locality.

#### 5. **Summary of Mapping Techniques**

|**Mapping Technique**|**Advantages**|**Disadvantages**|
|---|---|---|
|**Page-Level Mapping**|High flexibility, efficient for small writes.|High memory overhead for large devices.|
|**Block-Based Mapping**|Reduces memory usage significantly.|Poor performance for small writes due to high write amplification.|
|**Hybrid Mapping**|Balances memory usage and performance.|Requires complex merge operations (switch, partial, full) that can harm performance.|
|**Page Mapping + Caching**|Reduces memory usage while maintaining performance for workloads with locality.|Poor performance for workloads with low locality; extra I/O for cache misses.|