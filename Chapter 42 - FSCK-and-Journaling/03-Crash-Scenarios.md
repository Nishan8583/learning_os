### Key Notes on Crash Scenarios and Crash Consistency Problem

#### **Crash Scenarios**

When writing data to disk, crashes can lead to various scenarios depending on which writes succeed or fail. Below are the key scenarios:

1. **Single Write Succeeds:**
    
    - **Only Data Block (Db) is written:**
        - Data is on disk, but no inode points to it, and the bitmap does not mark the block as allocated.
        - **Result:** The write is effectively ignored; no crash consistency issue arises.
    - **Only Updated Inode (I[v2]) is written:**
        - Inode points to the disk address (e.g., block 5) where Db was supposed to be written, but Db is not written.
        - **Result:** File-system inconsistency arises:
            - Bitmap indicates block 5 is unallocated.
            - Inode indicates block 5 is allocated.
            - Reading block 5 results in garbage data.
    - **Only Updated Bitmap (B[v2]) is written:**
        - Bitmap indicates block 5 is allocated, but no inode points to it.
        - **Result:** File-system inconsistency arises:
            - Space leak occurs as block 5 is marked as allocated but is not used.
2. **Two Writes Succeed, One Fails:**
    
    - **Inode (I[v2]) and Bitmap (B[v2]) are written, but not Data Block (Db):**
        - Metadata appears consistent:
            - Inode points to block 5.
            - Bitmap indicates block 5 is in use.
        - **Problem:** Block 5 contains garbage data.
    - **Inode (I[v2]) and Data Block (Db) are written, but not Bitmap (B[v2]):**
        - Inode points to the correct data on disk.
        - Bitmap does not reflect the allocation of block 5.
        - **Result:** File-system inconsistency arises between the inode and the bitmap.
    - **Bitmap (B[v2]) and Data Block (Db) are written, but not Inode (I[v2]):**
        - Bitmap indicates block 5 is allocated, and the block contains data.
        - No inode points to block 5.
        - **Result:** File-system inconsistency arises:
            - Block 5 is allocated and contains data, but it is orphaned (no file references it).

---

#### **The Crash Consistency Problem**

- Crashes during disk writes can lead to:
    - **Inconsistencies in file system data structures:**
        - Disagreement between inodes, bitmaps, and data blocks.
    - **Space leaks:**
        - Blocks are marked as allocated but are not used.
    - **Garbage data:**
        - Users may read invalid or outdated data from disk.
- **Ideal Goal:**
    - Transition the file system atomically from one consistent state to another (e.g., before and after a file is appended).
- **Challenge:**
    - Disks commit writes one at a time.
    - Crashes or power loss can interrupt updates, leading to inconsistent states.
- **General Problem:**
    - This issue is referred to as the **crash-consistency problem** or the **consistent-update problem**.