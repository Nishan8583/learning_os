### Key Notes on Memory Allocation Strategies

#### **General Overview**

- The ideal memory allocator should be **fast** and **minimize fragmentation**.
- Allocation and free requests are arbitrary, making it impossible to define a universally "best" strategy.
- Different strategies have their own **pros and cons**, depending on the workload and allocator implementation.

---

#### **Strategies for Managing Free Space**

1. **Best Fit**
    
    - **Description**: Searches the free list for chunks of memory that are as big or bigger than the requested size and returns the smallest suitable chunk.
    - **Pros**:
        - Reduces wasted space by closely matching the requested size.
    - **Cons**:
        - Requires an **exhaustive search** of the free list, leading to high performance overhead.
        - Often leaves behind **small unusable chunks**, increasing fragmentation.
2. **Worst Fit**
    
    - **Description**: Finds the largest free chunk, allocates the requested size, and leaves the remaining large chunk in the free list.
    - **Pros**:
        - Tries to avoid creating many small chunks by leaving larger chunks available.
    - **Cons**:
        - Also requires an **exhaustive search** of the free list.
        - Studies show it performs poorly, leading to **excess fragmentation** and high overhead.
3. **First Fit**
    
    - **Description**: Finds the **first block** in the free list that is large enough to satisfy the request and allocates it.
    - **Pros**:
        - **Fast**: No need to search the entire free list.
    - **Cons**:
        - Can **pollute the beginning** of the free list with small chunks, making future allocations less efficient.
    - **Optimization**: Use **address-based ordering** of the free list to make coalescing easier and reduce fragmentation.
4. **Next Fit**
    
    - **Description**: Similar to First Fit, but instead of starting the search at the beginning of the free list, it starts from where the last search left off.
    - **Pros**:
        - Spreads allocation requests more evenly across the free list, reducing splintering at the beginning.
    - **Cons**:
        - Performance is similar to First Fit, but fragmentation behavior may vary depending on workloads.


#### **Examples**

- **Free List**: Sizes 10, 30, and 20.
![alt](chapter-17-10.png)
- **Allocation Request**: Size 15.
    - **Best Fit**: Allocates from the 20-sized chunk, leaving a small leftover chunk (5).
![alt](chapter-17-11.png)
    - **Worst Fit**: Allocates from the 30-sized chunk, leaving a larger leftover chunk (15).
![alt](chapter-17-12.png)
    - **First Fit**: Allocates from the first chunk that fits (30 in this case), similar to Worst Fit but with reduced search cost.

---

#### **Key Takeaways**

- **Best Fit**: Minimizes wasted space but increases fragmentation and search cost.
- **Worst Fit**: Avoids small chunks but performs poorly overall.
- **First Fit**: Fast but can lead to fragmentation at the start of the free list.
- **Next Fit**: Similar to First Fit but spreads allocation more evenly across the list.
- **Real-World Considerations**: Advanced techniques like **coalescing** and workload-specific analysis are needed for deeper optimization.