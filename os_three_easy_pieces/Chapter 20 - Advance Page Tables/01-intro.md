### Key Notes on Page Table Size and Optimization Techniques

1. **Problem with Linear Page Tables**:
    
    - Linear page tables are large and consume excessive memory.
    - Example: For a 32-bit address space (2³² bytes) with 4KB (2¹² bytes) pages:
        - Total virtual pages = 2³² / 2¹² = ~1 million pages.
        - Each page-table entry (PTE) is 4 bytes.
        - Total page table size = 1 million pages × 4 bytes = 4MB.
    - With 100 active processes, memory usage for page tables alone can reach hundreds of megabytes.
2. **Challenges**:
    
    - Each process typically has its own page table.
    - Memory overhead becomes significant with multiple active processes.
3. **Key Question (Crux)**:
    
    - **How to make page tables smaller?**
    - Explore techniques to reduce memory usage while maintaining efficiency.
4. **Inefficiencies of Linear Page Tables**:
    
    - Simple array-based page tables are inefficient for large address spaces.
    - Memory is wasted on unused portions of the address space.
5. **Goal**:
    
    - Develop new data structures or techniques to optimize page table size.
    - Address inefficiencies while balancing performance and memory usage.

### Key Notes on "Bigger Pages" and Multiple Page Sizes

#### **1. Simple Solution: Bigger Pages**

- **Concept**: Increasing page size reduces the size of the page table.
    - Example: For a 32-bit address space with 16KB pages:
        - VPN: 18 bits, Offset: 14 bits.
        - Page table size: 1MB (a 4x reduction compared to 4KB pages).
- **Problem**: Larger pages lead to **internal fragmentation**.
    - Applications may allocate large pages but use only small portions, wasting memory.
- **Common Page Sizes**:
    - Most systems use **small page sizes** to minimize fragmentation:
        - 4KB (e.g., x86).
        - 8KB (e.g., SPARCv9).

#### **2. Multiple Page Sizes**

- **Support**: Many architectures (e.g., MIPS, SPARC, x86-64) support multiple page sizes.
    - Small pages (4KB or 8KB) are standard.
    - Large pages (e.g., 4MB) are used for specific applications.
- **Use Cases**:
    - Large pages are beneficial for applications like **databases** or **high-end commercial systems**.
    - They reduce **TLB pressure** by allowing more address space to be accessed with fewer TLB misses.
- **Challenges**:
    - Using multiple page sizes increases the complexity of the OS virtual memory manager.
    - Large pages are often implemented via application-level interfaces to simplify usage.

#### **3. Trade-offs**

- **Advantages of Bigger Pages**:
    - Smaller page tables.
    - Reduced TLB misses with large pages.
- **Disadvantages**:
    - Internal fragmentation with larger pages.
    - Increased OS complexity with multiple page sizes.

#### **Conclusion**

- While bigger pages reduce page table size, they introduce memory inefficiency (internal fragmentation).
- Multiple page sizes offer flexibility but add complexity to the OS.