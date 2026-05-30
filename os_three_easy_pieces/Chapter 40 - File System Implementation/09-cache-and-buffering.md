### Key Notes on Caching and Buffering (40.7)

#### **1. Importance of Caching in File Systems**

- **File I/O is expensive**: Reading and writing files involves multiple I/O operations to the slow disk.
- **Caching in DRAM**: File systems use system memory (DRAM) to cache important blocks to improve performance.
- **Without caching**: Opening a file with a long pathname (e.g., `/1/2/3/.../100/file.txt`) would require hundreds of reads to access directory inodes and data.
- **Early file systems**: Used a fixed-size cache (e.g., 10% of total memory) allocated at boot time, with strategies like **LRU (Least Recently Used)** to manage which blocks to keep in the cache.

#### **2. Static vs. Dynamic Partitioning**

- **Static partitioning**: Fixed-size caches could lead to memory wastage if the file system didn’t need the allocated memory.
- **Dynamic partitioning**: Modern systems use a **unified page cache** that integrates virtual memory pages and file system pages, allowing flexible memory allocation based on current needs.

#### **3. Benefits of Caching**

- **For reads**:
    - The first file open may generate significant I/O traffic, but subsequent opens of the same file or files in the same directory will hit the cache, avoiding I/O.
- **For writes**:
    - Write traffic must eventually go to disk for persistence, but write buffering offers performance benefits:
        1. **Batching writes**: Delays writes to combine multiple updates into fewer I/Os (e.g., updating an inode bitmap for multiple file creations).
        2. **Scheduling writes**: Buffers writes in memory to optimize the order of I/Os, improving performance.
        3. **Avoiding unnecessary writes**: Delaying writes can avoid them entirely (e.g., if a file is created and then deleted before being written to disk).

#### **4. Write Buffering and Trade-offs**

- **Buffering duration**: Most modern file systems buffer writes in memory for **5 to 30 seconds**.
- **Trade-off**:
    - **Pros**: Improved performance through batching, scheduling, and avoiding unnecessary writes.
    - **Cons**: Risk of data loss if the system crashes before updates are written to disk.

#### **5. Handling Write Buffering in Specific Applications**

- Some applications (e.g., databases) cannot tolerate data loss due to write buffering.
- Solutions for such applications:
    - Use **fsync()** to force writes to disk.
    - Use **direct I/O interfaces** to bypass the cache.
    - Use the **raw disk interface**, avoiding the file system entirely.