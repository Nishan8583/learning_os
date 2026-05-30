### **18.4 Paging: Also Too Slow**

**1. Problem Overview**

- Page tables are **not only large**, but also **slow to access**.
    
- Every **memory reference** (load/store/instruction fetch) requires **an additional memory access** to retrieve the page table entry (PTE).
    
- This effectively **doubles memory access time**.
    

---

**2. Example Instruction**
```asm
movl 21, %eax
```
- Hardware must:
    
    1. Translate **virtual address (21)** → **physical address (117)**.
        
    2. Fetch **PTE** from the process’s page table.
        
    3. Use **PFN + offset** to form the final physical address.
        
    4. Fetch the actual data from physical memory.
        

---

**3. How Translation Works (Step-by-Step)**

|Step|Description|
|---|---|
|1|Extract **VPN** from virtual address:  <br>`VPN = (VirtualAddress & VPN_MASK) >> SHIFT`|
|2|Compute **PTE address**:  <br>`PTEAddr = PageTableBaseRegister + (VPN * sizeof(PTE))`|
|3|Fetch the **PTE** from memory:  <br>`PTE = AccessMemory(PTEAddr)`|
|4|Validate access:  <br>- If invalid → **Segmentation Fault**.  <br>- If protection violation → **Protection Fault**.|
|5|Form **physical address**:  <br>`offset = VirtualAddress & OFFSET_MASK`  <br>`PhysAddr = (PTE.PFN << PFN_SHIFT)|
|6|Fetch data from physical memory:  <br>`Register = AccessMemory(PhysAddr)`|
**4. Performance Impact**

- Every memory access now involves:
    
    - **1 access to fetch PTE**, and
        
    - **1 access to fetch actual data**.
        
- ⇒ **At least 2 memory accesses per instruction**, causing ~**2× slowdown**.
    

---

**5. Key Issues to Solve**

- **Memory overhead:** Page tables take too much space.
    
- **Speed overhead:** Each access requires multiple memory lookups.
    
- Need **hardware and software optimizations** (later solved using **TLBs** and **multi-level paging**).
    

---

**6. Summary**

- Paging simplifies memory management but introduces **significant performance costs**.
    
- Efficient paging requires addressing **both memory usage and access latency** problems.
    

---