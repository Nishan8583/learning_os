### **A Memory Trace — Key Points**

#### **1. Purpose**

- Demonstrates how memory accesses occur in a program using **paging**.
    
- Uses a simple C example to visualize how instruction and data memory accesses map through page tables.
    

---

#### **2. Example Code**
```c
int array[1000];
for (i = 0; i < 1000; i++)
    array[i] = 0;
```

#### **3. Assembly Breakdown**

Disassembled code (simplified x86):
```asm
1024 movl $0x0, (%edi,%eax,4)
1028 incl %eax
1032 cmpl $0x03e8, %eax
1036 jne 1024
```
- `%edi`: base address of `array`.
    
- `%eax`: index `i`.
    
- Multiply by 4 since `int` = 4 bytes.
    
- Loop repeats until `i == 1000`.
    

---

#### **4. Memory System Setup (Assumptions)**

- **Virtual address space:** 64 KB.
    
- **Page size:** 1 KB.
    
- **Page table:** Linear array located at **physical address 1 KB (1024)**.
    

---

#### **5. Page Table Mappings (Assumed)**

| Virtual Page (VPN) | Physical Frame (PFN) | Description                   |
| ------------------ | -------------------- | ----------------------------- |
| 1                  | 4                    | Code page (loop instructions) |
| 39                 | 7                    | Array start                   |
| 40                 | 8                    | 2nd part of array             |
| 41                 | 9                    | 3rd part                      |
| 42                 | 10                   | 4th part                      |
#### **6. Memory Access Pattern**

Each **loop iteration** involves:

- **Instruction fetches:** Each instruction needs
    
    - Page table lookup → find physical frame.
        
    - Instruction fetch from memory.
        
- **Data access (array write):**
    
    - Page table lookup for array element.
        
    - Actual write to physical memory.
        

**Total:** ~10 memory accesses per iteration  
→ 5 page table lookups + 5 actual instruction/data accesses.

---

#### **7. Visualization (Conceptually)**

- **Bottom graph:** Instruction memory (virtual → physical).
    
- **Middle graph:** Array data accesses.
    
- **Top graph:** Page table accesses (physical only).
    
- **X-axis:** Accesses over 5 loop iterations.
    

---

#### **8. Observations**

- Even a simple array initialization causes many memory accesses.
    
- Paging introduces overhead due to page table lookups.
    
- As the loop continues, new array pages (VPN 39–42) are accessed sequentially.
    
- Illustrates how real application memory behavior can quickly become complex.
    

---