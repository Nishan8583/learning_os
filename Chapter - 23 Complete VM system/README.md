# Summary of Chapter 23: Complete Virtual Memory Systems

This chapter provides a comprehensive overview of how complete virtual memory (VM) systems are designed and implemented, focusing on two major systems: **VAX/VMS** and **Linux**. It discusses the features required for a complete VM system, their impact on performance, functionality, and security, and how these systems have evolved over time.

---

## **23.1 VAX/VMS Virtual Memory**

### **Overview**

- The VAX-11 minicomputer architecture was introduced in the late 1970s by Digital Equipment Corporation (DEC).
- The operating system for the VAX-11 was **VAX/VMS**, designed to work across a wide range of machines, from inexpensive to high-end systems.
- The VAX/VMS system is notable for its ability to overcome hardware limitations through software innovations.

### **Memory Management Hardware**

- **32-bit virtual address space per process** divided into 512-byte pages.
- Virtual address structure:
    - 23-bit Virtual Page Number (VPN).
    - 9-bit offset.
    - Upper two bits of VPN used to differentiate segments (hybrid of paging and segmentation).
- Address space divided into:
    - **Process space (P0 and P1)**: Unique to each process.
        - P0: User program and heap (grows downward).
        - P1: Stack (grows upward).
    - **System space (S)**: Shared across processes, contains protected OS code and data.

### **Challenges and Solutions**

1. **Small Page Size (512 bytes):**
    
    - Made page tables excessively large.
    - Solution: Segmentation of user address space into P0 and P1 to reduce page table size.
    - User page tables (P0 and P1) were placed in kernel virtual memory to reduce memory pressure further.
    - Kernel could swap page tables to disk if memory was under pressure.
2. **Address Translation:**
    
    - Complex due to page tables being in kernel virtual memory.
    - Hardware-managed TLBs were used to speed up translation.

### **Page Replacement**

- VAX hardware lacked a reference bit in the page table entry (PTE), so VMS emulated it using protection bits.
- **Segmented FIFO Replacement Policy**:
    - Each process has a maximum number of pages it can keep in memory (Resident Set Size, RSS).
    - Pages are kept in a FIFO list, with evicted pages placed in global second-chance lists (clean-page free list and dirty-page list).
    - Pages can be reclaimed from these lists if accessed again before being overwritten, reducing disk I/O.
- **Clustering**:
    - Groups large batches of pages together for efficient disk I/O during swapping.

### **Optimizations**

1. **Demand Zeroing**:
    - Pages are marked inaccessible in the page table until accessed, reducing unnecessary work.
2. **Copy-on-Write (COW)**:
    - Pages are shared between processes and marked read-only until a write occurs, at which point a new page is allocated and copied.

---

## **23.2 The Linux Virtual Memory System**

### **Overview**

- Linux VM development has been driven by real-world engineering challenges, resulting in a feature-rich and flexible system.
- Focus on Linux for Intel x86 architecture, the most widely used deployment.

### **Linux Address Space**

![[Pasted image 20251024122838.png]]

- Divided into:
    - **User portion**: Contains user program code, stack, heap, and other parts.
    - **Kernel portion**: Contains kernel code, stacks, heap, and other parts.
- In 32-bit Linux:
    - User virtual addresses: `0x00000000` to `0xBFFFFFFF`.
    - Kernel virtual addresses: `0xC0000000` to `0xFFFFFFFF`.
- **Kernel Address Types**:
    1. **Kernel Logical Addresses**:
        - Directly mapped to physical memory.
        - Used for kernel data structures like page tables and kernel stacks.
        - Cannot be swapped to disk.
    2. **Kernel Virtual Addresses**:
        - Not directly mapped to physical memory.
        - Allocated using `vmalloc`.
        - Suitable for large buffers and scenarios requiring non-contiguous physical memory.

### **Page Table Structure**

- Linux uses a hardware-managed, multi-level page table structure.
- Transition from 32-bit to 64-bit x86:
    - 64-bit systems use a **four-level page table**.
    - Current systems use the bottom 48 bits of the 64-bit virtual address space.
    - Virtual address structure:
        - Top 16 bits: Unused.
        - Bottom 12 bits: Offset.
        - Middle 36 bits: Used for translation (P1 to P4 levels).

### **Large Page Support**

- Intel x86 supports multiple page sizes: 4 KB, 2 MB, and 1 GB.
- **Benefits of Huge Pages**:
    - Reduces the number of page table entries.
    - Improves TLB behavior and reduces TLB misses.
    - Shorter TLB-miss path and faster allocation in some scenarios.
- **Incremental Adoption**:
    - Initially, huge pages were explicitly requested by applications (e.g., via `mmap()` or `shmget()`).
    - Transparent huge page support was later added, allowing the OS to allocate huge pages automatically.
- **Costs of Huge Pages**:
    - Internal fragmentation (wasted memory in sparsely used large pages).
    - Inefficient swapping.
    - Overhead of allocation in some cases.

### **Page Cache**

- Linux uses a **unified page cache** for:
    - Memory-mapped files.
    - File data and metadata.
    - Anonymous memory (heap and stack pages).
- Pages are tracked as **clean** (read but not updated) or **dirty** (modified).
- Dirty pages are periodically written to persistent storage by background threads (`pdflush`).
- **2Q Replacement Algorithm**:
    - Two lists: inactive list (A1) and active list (Aq).
    - Pages are promoted to the active list upon re-reference.
    - Replacement candidates are taken from the inactive list.
    - Approximation of LRU is used to manage the lists.

### **Security Features**

1. **Buffer Overflow Protection**:
    
    - **NX bit (No-eXecute)**: Prevents execution of code in certain regions (e.g., stack).
    - **Return-Oriented Programming (ROP)**: Attackers exploit existing code snippets (gadgets) in memory.
    - **Address Space Layout Randomization (ASLR)**: Randomizes placement of code, stack, and heap in the virtual address space to prevent ROP attacks.
    - **Kernel Address Space Layout Randomization (KASLR)**: Randomizes kernel address space layout for added security.
2. **Meltdown and Spectre Attacks**:
    
    - Exploit vulnerabilities in speculative execution by modern CPUs.
    - **Kernel Page-Table Isolation (KPTI)**: Removes most kernel data from user processes to mitigate attacks, but at a performance cost.

---

## **23.3 Summary**

- The chapter provides a detailed comparison of two virtual memory systems: **VAX/VMS** and **Linux**.
- Both systems demonstrate the evolution of VM systems, with Linux incorporating many innovations from VAX/VMS and adding its own.
- Key features of Linux VM include:
    - Multi-level page tables.
    - Support for huge pages.
    - Unified page cache with a modified 2Q replacement algorithm.
    - Security measures like ASLR, KASLR, and defenses against Meltdown and Spectre.
- The chapter emphasizes the importance of understanding the history and evolution of VM systems to appreciate modern advancements.

---

### **References**

- [B+13] “Efficient Virtual Memory for Big Memory Servers” by A. Basu, J. Gandhi, J. Chang, M. D. Hill, M. M. Swift. ISCA ’13.
- [BB+72] “TENEX, A Paged Time Sharing System for the PDP-10” by D. G. Bobrow, et al. CACM, Volume 15, March 1972.
- [BJ81] “Converting a Swap-Based System to do Paging in an Architecture Lacking Page-Reference Bits” by O. Babaoglu, W. N. Joy. SOSP ’81.
- [BC05] “Understanding the Linux Kernel” by D. P. Bovet, M. Cesati. O’Reilly Media, November 2005.
- [C03] “The Innovator’s Dilemma” by Clayton M. Christenson. Harper Paperbacks, January 2003.
- [C93] “Inside Windows NT” by H. Custer, D. Solomon. Microsoft Press, 1993.
- [G04] “Understanding the Linux Virtual Memory Manager” by M. Gorman. Prentice Hall, 2004.
- [G+17] “KASLR is Dead: Long Live KASLR” by D. Gruss, et al. Engineering Secure Software and Systems, 2017.
- [JS94] “2Q: A Low Overhead High Performance Buffer Management Replacement Algorithm” by T. Johnson, D. Shasha. VLDB ’94.
- [LL82] “Virtual Memory Management in the VAX/VMS Operating System” by H. Levy, P. Lipman. IEEE Computer, Volume 15:3, March 1982.
- [M04] “Cloud Atlas” by D. Mitchell. Random House, 2004.
- [O16] “Virtual Memory and Linux” by A. Ott. Embedded Linux Conference, April 2016.
- [RL81] “Segmented FIFO Page Replacement” by R. Turner, H. Levy. SIGMETRICS ’81.
- [S07] “The Geometry of Innocent Flesh on the Bone: Return-into-libc without Function Calls (on the x86)” by H. Shacham. CCS ’07, October 2007.
- [S+04] “On the Effectiveness of Address-space Randomization” by H. Shacham, et al. CCS ’04, October 2004.