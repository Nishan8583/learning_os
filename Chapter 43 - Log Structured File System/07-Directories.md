### Key Notes on "What About Directories?" (Section 43.8)

1. **Directory Structure in LFS**:
    
    - Directories in LFS (Log-Structured File System) are similar to classic UNIX file systems.
    - A directory is a collection of `(name, inode number)` mappings.
    - Example: Creating a file `foo` in a directory involves:
        - Writing a new inode for the file.
        - Writing the file's data.
        - Updating the directory data and its inode to reference the new file.
2. **Steps to Access a File in LFS**:
    
    - To access a file (e.g., `/home/remzi/foo`):
        1. Look up the directory's inode in the **inode map** (e.g., `A3` for `dir`).
        2. Read the directory inode to find the location of the directory data (e.g., `A2`).
        3. Read the directory data block to get the `(name, inode number)` mapping (e.g., `(foo, k)`).
        4. Use the inode map to find the location of the file's inode (e.g., `A1` for `foo`).
        5. Read the file's data block (e.g., `A0`).
3. **Inode Map**:
    
    - The inode map (imap) stores the locations of inodes on disk.
    - It is usually cached in memory for faster access.
    - The imap contains information about the location of both:
        - The directory file's inode.
        - The newly created file's inode.
4. **Recursive Update Problem**:
    
    - **Problem**: In file systems that never update in place (like LFS), updating an inode's location would require updating:
        - The directory pointing to the file.
        - The parent directory of that directory.
        - And so on, up the file system tree.
    - **Solution in LFS**:
        - LFS avoids this problem using the **inode map**.
        - The directory's `(name, inode number)` mapping remains unchanged.
        - Only the imap is updated to reflect the new location of the inode.
        - This indirection prevents recursive updates across the file system tree.
5. **Illustration of File Creation in LFS**:
    
    - When creating a file `foo` in a directory:
        
![alt](chapter-43-05.png)
        
    - Key components:
        - `A0`: Data block of the file.
        - `A1`: Inode of the file.
        - `A2`: Data block of the directory.
        - `A3`: Inode of the directory.
        - `m[k]` and `m[dir]`: Mappings in the inode map for the file and directory, respectively.
6. **Key Takeaway**:
    
    - The inode map is a critical structure in LFS that:
        - Tracks inode locations.
        - Solves the recursive update problem by avoiding direct updates to directories when inode locations change.