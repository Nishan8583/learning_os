### Key Notes on Garbage Collection and TRIM in SSDs

#### **44.8 Garbage Collection**

1. **Log-Structured Storage and Garbage Creation**:
    
    - Log-structured storage systems inherently create garbage (invalid or outdated data) due to overwrites.
    - Garbage collection (GC) is the process of reclaiming space occupied by invalid data (dead blocks).
2. **Example of Garbage Creation**:
    
    - Logical blocks (e.g., 100, 101, 2000, 2001) are written to physical pages.
    - When blocks 100 and 101 are overwritten (e.g., with `c1` and `c2`), they are written to new physical pages (e.g., pages 4 and 5).
    - The mapping table is updated to reflect the new locations:
        
        ```
        Mapping Table: 100 -> 4, 101 -> 5, 2000 -> 2, 2001 -> 3
        ```
        
    - Old physical pages (e.g., 0 and 1) now contain garbage data.
3. **Garbage Collection Process**:
    
    - **Steps**:
        1. Identify blocks with garbage pages (e.g., block 0 with pages 0 and 1 as garbage).
        2. Read live data (e.g., pages 2 and 3) from the block.
        3. Write live data to the end of the log (e.g., pages 6 and 7).
        4. Erase the block to free it for future use.
    - **Post-GC State**:
        
        ```
        Mapping Table: 100 -> 4, 101 -> 5, 2000 -> 6, 2001 -> 7
        Block 0: All pages erased (E)
        Block 2: Pages 6 and 7 contain live data (b1, b2)
        ```
        
4. **Determining Live vs. Dead Pages**:
    
    - Each block stores metadata to identify which logical blocks are stored in its pages.
    - The mapping table is used to check if a page holds live data:
        - Pages still pointed to by the mapping table are live.
        - Pages not pointed to are dead and eligible for garbage collection.
5. **Cost of Garbage Collection**:
    
    - GC involves reading and rewriting live data, which can be expensive.
    - Ideal blocks for GC are those with only dead pages, as they can be erased without data migration.
6. **Optimizing Garbage Collection**:
    
    - **Overprovisioning**:
        - SSDs often include extra flash capacity to delay GC and perform it in the background during idle times.
        - Overprovisioning increases internal bandwidth, reducing the impact of GC on client performance.

---

#### **TRIM: A New Storage API**

1. **Purpose of TRIM**:
    
    - TRIM informs the SSD that specific blocks are no longer needed (e.g., after file deletion).
    - The SSD can then remove these blocks from the Flash Translation Layer (FTL) and reclaim the physical space during GC.
2. **How TRIM Works**:
    
    - TRIM takes an address (and possibly a length) and marks the specified blocks as no longer needed.
    - This allows the SSD to optimize its internal operations by not tracking unnecessary data.
3. **Relevance of TRIM**:
    
    - For traditional hard drives, TRIM is not useful due to static block mappings.
    - For log-structured SSDs, TRIM is highly beneficial as it improves GC efficiency and overall performance.
4. **Impact of TRIM on Interface and Implementation**:
    
    - The implementation of log-structured SSDs influences the need for TRIM.
    - TRIM enables a more effective implementation by providing the SSD with knowledge of unneeded blocks.

---

#### **Code Example: Mapping Table and Garbage Collection Process**

**Initial State (Before Overwrite):**

```
Mapping Table: 100 -> 0, 101 -> 1, 2000 -> 2, 2001 -> 3
Block 0:
Page  Content  State
00    a1       V
01    a2       V
02    b1       V
03    b2       V
```

**After Overwrite (Garbage Created):**

```
Mapping Table: 100 -> 4, 101 -> 5, 2000 -> 2, 2001 -> 3
Block 0:
Page  Content  State
00    a1       V (Garbage)
01    a2       V (Garbage)
02    b1       V
03    b2       V
04    c1       V
05    c2       V
```

**After Garbage Collection:**

```
Mapping Table: 100 -> 4, 101 -> 5, 2000 -> 6, 2001 -> 7
Block 0:
Page  Content  State
00    E        (Erased)
01    E        (Erased)
02    E        (Erased)
03    E        (Erased)
Block 2:
Page  Content  State
06    b1       V
07    b2       V
```

---

These notes summarize the key concepts of garbage collection and TRIM in SSDs, providing a quick reference for understanding their processes and importance.