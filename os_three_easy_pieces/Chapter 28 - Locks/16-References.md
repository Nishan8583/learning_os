### Key Notes on References

---

#### **Books and Papers**

1. **[D91] "Just Win, Baby: Al Davis and His Raiders" by Glenn Dickey (1991)**:
    
    - Discusses Al Davis and the Raiders, including the famous quote "Just Win, Baby."
    - Not specifically recommended but cited for the quote.
2. **[D+13] "Everything You Always Wanted to Know about Synchronization but Were Afraid to Ask" by Tudor David, Rachid Guerraoui, Vasileios Trigonakis (2013)**:
    
    - A comprehensive paper comparing various locking strategies using hardware primitives.
    - Focuses on modern multiprocessor systems and their synchronization mechanisms.
3. **[D68] "Cooperating Sequential Processes" by Edsger W. Dijkstra (1968)**:
    
    - Seminal paper discussing the original concurrency problem and Dekker’s solution.
    - Available online: [EWD123](http://www.cs.utexas.edu/users/EWD/ewd01xx/EWD123.PDF).
4. **[H91] "Wait-free Synchronization" by Maurice Herlihy (1991)**:
    
    - Introduces the concept of **wait-free synchronization** for concurrent data structures.
    - Landmark paper, though its ideas have been slow to gain widespread adoption due to complexity.
5. **[H93] "MIPS R4000 Microprocessor User’s Manual" by Joe Heinrich (1993)**:
    
    - Details the MIPS architecture, including the `Load-Linked` and `Store-Conditional` instructions.
    - Available: [MIPS R4000 Manual](http://cag.csail.mit.edu/raw/documents/R4400%20Uman%20book%20Ed2.pdf).
6. **[L81] "Observations on the Development of an Operating System" by Hugh Lauer (1981)**:
    
    - Retrospective on the development of the Pilot OS, an early PC operating system.
    - Highlights the importance of concise and efficient code (Lauer’s Law).
7. **[L09] "glibc 2.9 (Linux pthreads implementation)"**:
    
    - Found in the `nptl` subdirectory of the GNU libc library.
    - Contains the implementation of pthread support in Linux.
    - Available: [glibc 2.9](http://ftp.gnu.org/gnu/glibc).
8. **[M82] "The Architecture of the Burroughs B5000: 20 Years Later and Still Ahead of the Times?" by A. Mayer (1982)**:
    
    - Discusses the Burroughs B5000 architecture and its early use of test-and-set and two-phase locks.
    - Available: [Burroughs B5000](https://helios.fg.rbc.com/www.ajwm.net/amayer/papers/B5000.html).
9. **[M15] "OSSpinLock Is Unsafe" by J. McCall (2015)**:
    
    - Highlights the dangers of using `OSSpinLock` on macOS with threads of different priorities.
    - Explains how priority inversion can lead to indefinite spinning.
    - Available: [OSSpinLock Is Unsafe](https://mjtsai.com/blog/2015/12/16/osspinlock-is-unsafe).
10. **[MS91] "Algorithms for Scalable Synchronization on Shared-Memory Multiprocessors" by John M. Mellor-Crummey and M. L. Scott (1991)**:
    
    - A detailed survey of locking algorithms using hardware instructions.
    - Does not rely on OS support.
    - Published in ACM TOCS, Volume 9, Issue 1.
11. **[P81] "Myths About the Mutual Exclusion Problem" by G.L. Peterson (1981)**:
    
    - Introduces **Peterson’s Algorithm** for mutual exclusion using only loads and stores.
    - Published in _Information Processing Letters_.
12. **[R97] "What Really Happened on Mars?" by Glenn E. Reeves (1997)**:
    
    - Describes the **priority inversion problem** on the Mars Pathfinder mission.
    - Highlights the importance of correct concurrent code, especially in critical systems like space exploration.
    - Available: [Mars Pathfinder Priority Inversion](https://www.ostep.org/Citations/mars.html).
13. **[S05] "Guide to Porting from Solaris to Linux on x86" by Ajay Sood (2005)**:
    
    - A guide for developers transitioning from Solaris to Linux.
    - Available: [Porting Guide](http://www.ibm.com/developerworks/linux/library/l-solar/).
14. **[S09] "OpenSolaris Thread Library" by Sun**:
    
    - Source code for the OpenSolaris thread library.
    - Available: [OpenSolaris Thread Library](https://src.opensolaris.org/source/xref/onnv/onnv-gate/usr/src/lib/libc/port/threads/synch.c).
15. **[W09] "Load-Link, Store-Conditional"**:
    
    - Wikipedia article explaining `Load-Linked` and `Store-Conditional` instructions across different architectures.
    - Lists instructions for Alpha, PowerPC, MIPS, and ARM architectures.
    - Available: [Load-Link/Store-Conditional](https://en.wikipedia.org/wiki/Load-Link/Store-Conditional).
16. **[WG00] "The SPARC Architecture Manual: Version 9" by D. Weaver, T. Germond (2000)**:
    
    - Comprehensive manual on the SPARC architecture, including atomic operations.
    - Available: [SPARC Architecture Manual](http://www.sparc.org/standards/SPARCV9.pdf).

---

#### **Key Takeaways**

- **Historical Context**:
    - Early locking mechanisms like **Dahm Locks** (1960s) and **Peterson’s Algorithm** (1981) laid the groundwork for modern synchronization techniques.
- **Modern Synchronization**:
    - Modern locks rely on a combination of **hardware primitives** (e.g., test-and-set, compare-and-swap, fetch-and-add) and **OS support** (e.g., futex, park/unpark).
- **Priority Inversion**:
    - A critical issue in concurrent programming where high-priority threads are blocked by lower-priority threads holding locks.
    - Solutions include **priority inheritance** and ensuring equal thread priorities.
- **Further Reading**:
    - Explore the **Solaris** and **Linux** codebases for real-world implementations of locks.
    - Refer to **Tudor David et al. (2013)** for a comparison of locking strategies on modern multiprocessors.
    - Study **Dijkstra (1968)** and **Peterson (1981)** for foundational work on mutual exclusion and concurrency.