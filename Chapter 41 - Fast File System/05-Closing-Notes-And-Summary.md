### Key Notes on FFS (Fast File System)

#### 1. **Handling Small Files and Internal Fragmentation**

- **Problem**: Many files were around 2KB in size, but using 4KB blocks caused significant internal fragmentation, wasting disk space.
- **Solution**:
    - Introduced **sub-blocks** (512-byte blocks) to allocate space for small files.
    - Example:
        - A 1KB file would use two 512-byte sub-blocks instead of a full 4KB block.
        - As the file grows to 4KB, FFS consolidates the sub-blocks into a single 4KB block and frees the sub-blocks for reuse.
- **Optimization**: To avoid inefficiency caused by frequent sub-block operations, the `libc` library was modified to buffer writes and issue them in 4KB chunks, minimizing sub-block usage.

#### 2. **Optimized Disk Layout for Performance**

- **Problem**: Sequential reads on consecutive sectors caused delays due to disk rotation (e.g., block 1 would rotate past the disk head before it could be read after block 0).
- **Solution**:
    - Introduced a **staggered layout** by skipping blocks during placement (e.g., skipping every other block).
    - Used **parameterization** to determine the optimal number of blocks to skip based on the disk's performance characteristics.
- **Limitation**: This layout achieved only 50% of peak bandwidth since it required two rotations to read all blocks on a track.
- **Modern Disks**:
    - Modern disks use **track buffers** to read entire tracks into an internal cache, eliminating the need for staggered layouts.
    - File systems no longer need to manage such low-level details due to improved abstraction and higher-level interfaces.

#### 3. **Usability Improvements**

- **Long File Names**: Allowed for longer, more expressive file names compared to the traditional fixed-size (e.g., 8-character) names.
- **Symbolic Links**:
    - Introduced symbolic links as flexible "aliases" to files or directories.
    - Overcame limitations of hard links:
        - Hard links could not point to directories (to avoid loops in the file system hierarchy).
        - Hard links were restricted to files within the same volume.
- **Atomic `rename()` Operation**: Ensured that renaming files was performed as a single, indivisible operation, improving reliability.

#### 4. **Legacy and Impact**

- FFS was a **milestone in file system history**, highlighting the importance of efficient file management in operating systems.
- Inspired many modern file systems, such as **Linux ext2 and ext3**, which are intellectual descendants of FFS.
- Key takeaway: **Treat the disk like it’s a disk**, accounting for its unique performance characteristics.

#### Code Example

- **Sub-block Allocation**:
    
    ```c
    // Example of sub-block allocation
    if (file_size < BLOCK_SIZE) {
        allocate_sub_blocks(file, file_size / SUB_BLOCK_SIZE);
    } else {
        allocate_full_block(file);
        free_sub_blocks(file);
    }
    ```
    
- **Staggered Layout**:
    
    ```c
    // Example of staggered block layout
    for (int i = 0; i < total_blocks; i += skip_factor) {
        place_block(disk, file, i);
    }
    ```
    
- **Symbolic Link Creation**:
    
    ```c
    // Example of creating a symbolic link
    int symlink(const char *target, const char *linkpath);
    symlink("/path/to/target", "/path/to/link");
    ```
    

#### 5. **Conclusion**

- FFS revolutionized file systems by addressing key challenges like space efficiency, performance optimization, and usability.
- Its innovations, such as sub-blocks, staggered layouts, long file names, symbolic links, and atomic operations, set the foundation for modern file systems.