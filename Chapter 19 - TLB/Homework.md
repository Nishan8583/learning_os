## Homework: Measuring TLB Performance

**Goal:** Determine TLB size and miss cost.

### Concept:

- Access increasing numbers of pages and measure access time.
    
- Sudden jumps indicate TLB boundaries.
    

**Sample Code:**
```c
int jump = PAGESIZE / sizeof(int);
for (i = 0; i < NUMPAGES * jump; i += jump)
    a[i] += 1;
```
Expected Results (Figure 19.5):

| Pages Accessed | Avg Time (ns) | Inference        |
| -------------- | ------------- | ---------------- |
| ≤ 8            | 5 ns          | L1 TLB hits      |
| ≥ 16           | 20 ns         | L2 TLB hits      |
| ≥ 1024         | 70 ns         | Misses to memory |
**Shows:**

- Two-level TLB hierarchy.
    
- First-level small (~8–16 entries), second larger (~512 entries).
    
- Miss penalty ≈ 14× slower.
    

---

### Key Homework Questions

1. How precise is your timer (`gettimeofday()`)?
    
2. Write `tlb.c` to measure per-page cost.
    
3. Automate runs with a script varying number of pages.
    
4. Graph results (e.g., with ploticus or zplot).
    
5. Prevent compiler from optimizing away loop.
    
6. Pin program to one CPU to avoid TLB differences.
    
7. Initialize array before timing to avoid page-fault penalties.
    

---

## ✅ Key Takeaways

- TLB = critical performance feature of virtual memory.
    
- Exploits **locality** to reduce paging overhead.
    
- Miss handling can be by **hardware or OS**.
    
- Context switches require **flush or ASIDs**.
    
- TLB miss cost can dominate performance if not managed.