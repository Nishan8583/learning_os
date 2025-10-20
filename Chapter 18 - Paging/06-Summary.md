
# 🧠 Paging — Summary (18.6)

## **1. Purpose of Paging**

- **Paging** is introduced as a solution for **virtualizing memory** efficiently.
    

---

## **2. Advantages**

- ✅ **No External Fragmentation:**
    
    - Memory is divided into **fixed-size pages**, preventing unused gaps.
        
- ✅ **Flexible Memory Use:**
    
    - Supports **sparse virtual address spaces** — only maps pages that are actually used.
        

---

## **3. Challenges**

- ⚠️ **Performance Overhead:**
    
    - Each memory access may require **extra page table lookups**, slowing execution.
        
- ⚠️ **Memory Waste:**
    
    - Large **page tables** can consume significant physical memory.