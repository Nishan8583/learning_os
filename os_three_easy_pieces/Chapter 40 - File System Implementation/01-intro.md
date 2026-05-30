### Key Notes on File System Implementation (vsfs - Very Simple File System)

#### Overview

- **vsfs (Very Simple File System)**: A simplified version of a typical UNIX file system.
- Purpose: Introduces basic on-disk structures, access methods, and policies found in modern file systems.
- Pure software implementation: No hardware features are added to enhance performance, but device characteristics are considered for efficiency.
- Learning approach: Case studies starting with vsfs, followed by real-world file systems like AFS (Andrew File System) and ZFS (Zettabyte File System).

---

#### Key Concepts in File System Implementation

1. **The Crux: How to Implement a Simple File System**
    
    - **Key Questions**:
        - What structures are needed on the disk?
        - What do these structures need to track?
        - How are these structures accessed?
2. **Two Key Aspects of File Systems**:
    
    - **Data Structures**:
        - On-disk structures organize data and metadata.
        - Simple file systems (e.g., vsfs) use basic structures like arrays of blocks or objects.
        - Advanced file systems (e.g., SGI’s XFS) use complex tree-based structures.
    - **Access Methods**:
        - Map process calls (e.g., `open()`, `read()`, `write()`) to file system structures.
        - Identify which structures are read or written during system calls.
        - Evaluate the efficiency of these operations.

---

#### Mental Models of File Systems

- **Purpose**: Develop an abstract understanding of file system operations.
- **Key Questions to Build Mental Models**:
    - What on-disk structures store the file system’s data and metadata?
    - What happens when a process opens a file?
    - Which on-disk structures are accessed during a read or write operation?
- **Goal**: Understand both the data structures and access methods to form a comprehensive mental model of the file system.