
# 🧩 Multi-Level Page Tables — Notes (from _Operating Systems: Three Easy Pieces_)

---

## 🌱 Motivation

**Problem:**  
A linear page table wastes memory because it needs one entry per virtual page, even for unused address regions.

**Goal:**  
Save memory by avoiding page-table entries for unused parts of the address space.

---

## 🏗️ Core Idea

Instead of one large, continuous page table:

- Break the linear page table into **page-sized chunks**.
    
- If an entire page of entries is **invalid**, **don’t allocate that page at all**.
    
- Use a new structure called the **Page Directory** to track which pages of the page table exist.
    

Each **Page Directory Entry (PDE)**:

- Has a **valid bit**.
    
- Has a **page frame number (PFN)** of the page of the page table.
    
- If the PDE is invalid → the entire page of PTEs doesn’t exist in memory.
    

Thus, the page directory acts like a “map of maps,” pointing only to valid chunks of the page table.

---

## 🧱 Example (from the book)

Example address space:

- Virtual address space: **16 KB**
    
- Page size: **64 bytes**
    
- → 14-bit address space: **8-bit VPN** + **6-bit offset**
    
- Each PTE: **4 bytes** → Full linear page table = 256 × 4 bytes = **1 KB**
    
- 1 KB page table = **16 pages** of 64 bytes each (each page holds 16 PTEs).
    

Used virtual pages:

- VPNs **0–1** → code
    
- VPNs **4–5** → heap
    
- VPNs **254–255** → stack
    
- Rest → unused
    

---

## 🔁 Two-Level Table Construction

1. Break up the linear page table (256 entries) into **16 pages** (each 16 PTEs).
    
2. Create a **page directory** with **16 entries**, one for each page of the page table.
    

Each PDE describes a **page of PTEs**:

- If the PDE is valid → points to a page of the page table (via PFN).
    
- If invalid → no page table page allocated for that region.
    

---

## 🔍 Address Translation Steps

Given a virtual address:

1. **Extract bits for Page Directory Index (PDIndex)**
    
    - Top 4 bits of VPN (since 16 entries → 4 bits).
        
    - `PDEAddr = PageDirBase + (PDIndex * sizeof(PDE))`
        
2. **Check PDE validity**
    
    - If invalid → address not in memory → **page fault/exception**.
        
    - If valid → PDE’s PFN points to a **page of the page table**.
        
3. **Extract bits for Page Table Index (PTIndex)**
    
    - Remaining 4 bits of VPN.
        
    - `PTEAddr = (PDE.PFN << SHIFT) + (PTIndex * sizeof(PTE))`
        
4. **Get PTE and check validity**
    
    - If valid → extract **PTE.PFN**, which gives the **physical page frame**.
        
    - Combine PFN and **offset** to get physical address: