# Beyond Physical Memory: Policies

## 22.0 Introduction

When physical memory is plentiful, page faults are easy to handle — the OS simply allocates a free page.  
When memory is scarce, the OS must **evict pages** to make room for new ones.  
The **replacement policy** determines **which page(s) to evict** and is one of the most crucial design choices in virtual memory systems.

---

## The Crux: How to Decide Which Page to Evict

The OS must choose wisely which page(s) to evict when memory is full.  
These choices are guided by **replacement policies**, each trying to minimize future page faults.

---

## 22.1 Cache Management

Main memory acts as a **cache** for all virtual memory pages.  
The goal of a replacement policy is to:

- **Minimize cache misses** (page faults), or equivalently,
    
- **Maximize cache hits**.
    

### Average Memory Access Time (AMAT)

AMAT=TM+(PMiss×TD)AMAT = T_M + (P_{Miss} \times T_D)AMAT=TM​+(PMiss​×TD​)

- **T_M** = memory access time
    
- **T_D** = disk access time
    
- **P_Miss** = probability of a miss
    

Even small miss rates dramatically increase AMAT due to disk latency.

**Example:**

- TM=100nsT_M = 100nsTM​=100ns, TD=10msT_D = 10msTD​=10ms
    
- If PMiss=0.1P_{Miss} = 0.1PMiss​=0.1:
    
    AMAT=100ns+0.1×10ms=1.0001msAMAT = 100ns + 0.1 \times 10ms = 1.0001msAMAT=100ns+0.1×10ms=1.0001ms
- If PMiss=0.001P_{Miss} = 0.001PMiss​=0.001:
    
    AMAT=100ns+0.001×10ms=10.1μsAMAT = 100ns + 0.001 \times 10ms = 10.1\mu sAMAT=100ns+0.001×10ms=10.1μs
- Even a **tiny miss rate dominates** AMAT.
    

---

## 22.2 Optimal Replacement Policy (MIN)

Developed by **Belady (1966)** — also called **MIN**.

**Idea:** Evict the page that will be accessed **farthest in the future**.  
This minimizes cache misses — hence “optimal.”

### Example Trace:

Access stream:  
`0, 1, 2, 0, 1, 3, 0, 3, 1, 2, 1`

With a cache of 3 pages:

- 6 hits, 5 misses → **Hit rate = 54.5%**
    
- Excluding compulsory misses → **85.7% hit rate**
    

**Cold-start (Compulsory) Miss:** first-time reference to a page.  
**Capacity Miss:** cache too small to hold working set.  
**Conflict Miss:** arises in hardware caches (not in fully-associative OS caches).

### Key Takeaway

- **Optimal** serves as a _comparison baseline_.
    
- Not implementable in real systems (future unknown).
    
- Useful for benchmarking policy effectiveness.
    

---

## 22.3 FIFO (First-In, First-Out)

**Policy:** Evict the oldest page in memory (first brought in).

### Advantages

- Very simple to implement.
    

### Disadvantages

- May evict heavily used pages just because they’re “old.”
    

### Example

Reference stream: `0, 1, 2, 0, 1, 3, 0, 3, 1, 2, 1`  
→ **36.4% hit rate** (57.1% ignoring compulsory misses)

### Belady’s Anomaly

For FIFO, increasing cache size can **worsen** performance.  
Example sequence: `1, 2, 3, 4, 1, 2, 5, 1, 2, 3, 4, 5`

**Reason:** FIFO lacks the **stack property** — larger caches don’t necessarily include smaller ones’ contents.

---

## 22.4 Random Replacement

**Policy:** Randomly pick a page to evict.

### Pros

- Simple.
    
- Avoids pathological cases like FIFO.
    

### Cons

- Unpredictable; may evict valuable pages.
    

### Example

Same reference stream → performance fluctuates:

- Sometimes matches OPT (~40% of trials)
    
- Sometimes much worse (few hits)
    

Random can occasionally outperform deterministic policies in corner cases.

---

## 22.5 LRU (Least Recently Used)

**Motivation:** Use **past behavior** to predict **future use**.

### Principle of Locality

Programs tend to:

- Access the same **data/code** repeatedly (**temporal locality**)
    
- Access **nearby** memory locations (**spatial locality**)
    

### Policies

- **LFU:** Evict least frequently used page.
    
- **LRU:** Evict least recently used page (most common).
    
- **Opposites:** MRU, MFU (rarely effective).
    

### Example

LRU performs as well as OPT in the sample trace by correctly retaining recently used pages.

### Types of Locality

- **Spatial Locality:** Accesses to nearby pages likely.
    
- **Temporal Locality:** Recently accessed pages likely to be accessed soon again.
    

---

## 22.6 Workload Examples

### 1. No Locality

- Accesses are random across all pages.
    
- LRU, FIFO, Random perform **equally**.
    
- OPT still better (future knowledge helps).
    

### 2. 80–20 Workload

- 80% of references → 20% of pages (hot pages).
    
- LRU performs **best** because it keeps hot pages longer.
    
- FIFO & Random do reasonably but not as well.
    
- OPT still ideal.
    

### 3. Looping Sequential Workload

- Sequentially access 50 pages in a loop.
    
- LRU & FIFO perform **poorly (0% hit rate)** for cache size 49.
    
- Random performs **better** since it occasionally keeps useful pages.
    
- Demonstrates LRU’s **worst-case** behavior.
    

---

## 22.7 Implementing Historical Algorithms

Implementing LRU perfectly is **costly**:

- Requires updating timestamps or lists **on every access**.
    
- Impractical for large memory systems (e.g., 4GB → 1 million pages).
    

### Challenge

How to **approximate** LRU efficiently?

---

## 22.8 Approximating LRU – The Clock Algorithm

Introduced by **Corbató (1969)**.

### Use Bit (Reference Bit)

- One bit per page, set to **1** by hardware on access.
    
- OS periodically clears bits.
    

### Clock Algorithm

- Pages arranged in a **circular list**.
    
- A **clock hand** scans:
    
    1. If use bit = 1 → clear it, move hand forward.
        
    2. If use bit = 0 → evict that page.
        

This approximates LRU behavior efficiently.

**Performance:**  
Close to LRU, better than FIFO/Random, requires little overhead.

---

## 22.9 Considering Dirty Pages

Modified by **Corbató** to prefer evicting **clean pages**.

### Dirty Bit

- Set by hardware when a page is written. (Dirty page means pages that are modified in memory, not updated in disk)
    
- Clean pages can be evicted **without writing to disk**.
    
- Saves I/O time.
    

### Eviction Preference

1. Unused + Clean pages
    
2. Unused + Dirty pages (second priority)
    

---

## 22.10 Other VM Policies

### 1. Page Selection Policy

- **Demand Paging:** Load page only when accessed.
    
- **Prefetching:** Predict next page (e.g., load P+1 after P).
    

### 2. Page Write Policy

- **Clustering/Grouping:** Write multiple dirty pages together for efficiency.
    
- Reduces disk overhead from many small writes.
    

---

## 22.11 Thrashing

Occurs when total memory demand exceeds physical memory → constant paging.

### Solutions

- **Admission Control:** Run fewer processes so working sets fit in memory.
    
- **Out-of-Memory Killer (Linux):** Terminates large memory users.
    

Goal: “Do less work well than everything poorly.”

---

## 22.12 Summary

- VM uses **page replacement policies** to manage limited memory.
    
- **FIFO**, **Random**, **LRU**, and **Clock** are key examples.
    
- **Clock** approximates **LRU** efficiently.
    
- Modern variants (e.g., **ARC**) add **scan resistance** to handle pathological workloads.
    
- Fast storage (e.g., SSDs) is **reviving research** into smarter replacement policies.
    

---

## Key References

- **Belady (1966):** Introduced Optimal/MIN algorithm.
    
- **Corbató (1969):** Clock algorithm.
    
- **Denning (1970):** Page selection and thrashing.
    
- **Megiddo & Modha (2003):** ARC algorithm (modern LRU improvement).
    
- **Saxena & Swift (2010):** FlashVM — VM management on SSDs.
    

---

## Practice & Simulation

**Try:** `paging-policy.py` (provided by OSTEP)

### Example Exercises

1. Simulate FIFO, LRU, and OPT for random address traces.
    
2. Find worst-case reference patterns for each policy.
    
3. Generate traces with locality and compare performance.
    
4. Use tools like `valgrind` to collect real memory traces and analyze hit rates vs. cache size.