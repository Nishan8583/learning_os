### Key Notes for Quick Review

#### 40.5 Free Space Management

1. **Purpose**:
    
    - File systems must track which inodes and data blocks are free or used.
    - This is essential for allocating space for new files or directories.
2. **Free Space Management in vsfs**:
    
    - Uses **two simple bitmaps**:
        - One for tracking free inodes.
        - One for tracking free data blocks.
    - Example process for creating a file:
        - Search the inode bitmap for a free inode.
        - Allocate the inode to the file and mark it as used (set to `1`).
        - Update the on-disk bitmap with the new information.
    - A similar process is followed for allocating data blocks.
3. **Block Allocation Considerations**:
    
    - Some file systems (e.g., Linux ext2/ext3) use **pre-allocation policies**:
        - Search for a sequence of free blocks (e.g., 8 blocks) when creating a new file.
        - Allocate these contiguous blocks to improve performance by reducing fragmentation.

---

#### Aside: Free Space Management Techniques

1. **Bitmaps**:
    
    - A common method for managing free space.
    - Each bit represents the status of a block or inode (free or used).
2. **Free Lists**:
    
    - Used in early file systems.
    - A single pointer in the superblock points to the first free block.
    - Each free block contains a pointer to the next free block, forming a linked list.
    - When a block is allocated, the head block is used, and the list is updated.
3. **Modern Approaches**:
    
    - Modern file systems use more advanced data structures for free space management.
    - Example: **SGI’s XFS** uses a **B-tree** to compactly represent free disk chunks.
    - These advanced structures allow for better time-space trade-offs.