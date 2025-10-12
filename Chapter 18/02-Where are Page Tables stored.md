### **18.2 Where Are Page Tables Stored?**

**1. Page Tables Can Be Very Large**

- Example: **32-bit address space** with **4KB pages**.
    
    - 32-bit address → 20-bit **VPN** + 12-bit **offset**.
        
    - 2202^{20}220 pages = ~1 million pages per process.
        
    - Each **Page Table Entry (PTE)** = 4 bytes.
        
    - → **4 MB per page table** for a single process.
        
- With 100 processes → ~**400 MB** memory just for page tables.
    
- For 64-bit systems, size becomes even more massive.
    

**2. Definition: Page Table**

- Core OS data structure for **memory management**.
    
- Stores **virtual-to-physical address translations**.
    
- Usually **one page table per process**.
    
- Structure may be **hardware-defined** (older systems) or **OS-managed** (modern systems).
    

**3. Storage Location**

- Page tables are **not stored on-chip** (MMU can’t hold them — too big).
    
- Instead, they are kept in **main physical memory**, managed by the OS.
    
- The OS later may **virtualize its own memory**, allowing page tables themselves to be **paged out or swapped to disk** (concept introduced later).
    

**4. Key Takeaway**

- Page tables are essential but **memory-intensive**.
    
- Efficient storage and management are critical for performance and scalability.