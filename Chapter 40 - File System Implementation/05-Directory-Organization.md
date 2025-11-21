### Key Notes for Quick Review

#### 40.4 Directory Organization

1. **Directory Structure in vsfs**:
    
    - Directories are organized as a list of `(entry name, inode number)` pairs.
    - Each entry includes:
        - `inum`: Inode number.
        - `reclen`: Record length (total bytes for the name + leftover space).
        - `strlen`: String length (actual length of the name).
        - `name`: Name of the entry.
2. **Example Directory Structure**:
    
    ```
    inum | reclen | strlen | name
    5    | 12     | 2      | .
    2    | 12     | 3      | ..
    12   | 12     | 4      | foo
    13   | 12     | 4      | bar
    24   | 36     | 28     | foobar_is_a_pretty_longname
    ```
    
    - `.` refers to the current directory.
    - `..` refers to the parent directory.
3. **Handling Deletions**:
    
    - Deleting a file (e.g., via `unlink()`) can leave empty space in the directory.
    - Empty space is marked with a reserved inode number (e.g., zero).
    - Record length (`reclen`) allows reusing old, larger entries for new files.
4. **Directories as Special Files**:
    
    - Directories are treated as a special type of file.
    - They have an `inode` in the inode table, marked as "directory."
    - Data blocks of directories are pointed to by the inode and stored in the data block region.
5. **Alternative Directory Structures**:
    
    - Simple linear lists are not the only way to store directory entries.
    - Example: XFS uses B-trees for directories, enabling faster file creation by avoiding full scans of the directory.

---

#### Aside: Linked-Based Approaches

1. **Linked List in Inodes**:
    
    - Inodes can use a single pointer to the first block of a file.
    - Additional pointers are added at the end of each data block for larger files.
2. **Performance Issues**:
    
    - Linked file allocation performs poorly for workloads requiring:
        - Reading the last block of a file.
        - Random access.
3. **Improved Linked Allocation**:
    
    - Some systems use an **in-memory table of link information**:
        - Table indexed by the address of a data block `D`.
        - Entry contains `D`'s next pointer (address of the next block in the file).
        - Null-value or marker indicates the end-of-file or free block.
4. **File Allocation Table (FAT)**:
    
    - FAT is a linked-based allocation scheme.
    - Differences from UNIX file systems:
        - No inodes; directory entries store metadata and refer directly to the first block of a file.
        - Hard links are not possible.
5. **References**:
    
    - FAT file system: Classic Windows file system before NTFS.
    - For more details, see Brouwer [B02].