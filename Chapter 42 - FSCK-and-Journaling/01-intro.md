### Key Notes: Crash Consistency - FSCK and Journaling

#### **Crash Consistency Problem**

- **File System Data Structures**:
    
    - File systems manage persistent data structures (e.g., files, directories, metadata) that must survive power loss or crashes.
    - Persistent data structures are stored on devices like hard disks or SSDs.
- **The Challenge**:
    
    - Crashes or power losses during updates to on-disk structures can leave the file system in an inconsistent state.
    - Example: If two structures (A and B) need to be updated, and the system crashes after updating only A, the disk state becomes inconsistent.
- **The Crux**:
    
    - **Problem**: How to ensure the file system remains in a consistent state despite crashes or power losses.
    - **Goal**: After a crash, the system should be able to boot and mount the file system with a reasonable on-disk state.

---

#### **Solutions to Crash Consistency**

1. **FSCK (File System Checker)**:
    
    - **Definition**: An older approach used by file systems to detect and repair inconsistencies after a crash.
    - **Process**:
        - Scans the entire file system after a crash.
        - Identifies and fixes inconsistencies in the on-disk structures.
    - **Drawbacks**:
        - Time-consuming, especially for large file systems.
        - Can delay system recovery significantly.
2. **Journaling (Write-Ahead Logging)**:
    
    - **Definition**: A modern approach that logs changes before applying them to the main file system structures.
    - **How It Works**:
        - Changes are first written to a journal (a special log).
        - Once the journal is safely written, the actual changes are applied to the file system.
        - If a crash occurs, the system can replay the journal to restore consistency.
    - **Advantages**:
        - Faster recovery compared to FSCK.
        - Ensures a consistent state with minimal overhead.
    - **Flavors of Journaling**:
        - **Writeback Mode**: Only metadata is journaled; data blocks are written directly to the disk.
        - **Ordered Mode**: Metadata is journaled, and data blocks are written to the disk before metadata is updated.
        - **Data Journaling Mode**: Both metadata and data blocks are journaled, ensuring the highest level of consistency but with more overhead.
    - **Example**: Linux ext3 file system implements journaling with these modes.