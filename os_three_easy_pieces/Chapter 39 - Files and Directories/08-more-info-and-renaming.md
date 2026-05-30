### Key Notes on File Metadata and File Removal

---

#### **Getting Information About Files**

1. **File Metadata**:
    
    - Metadata is data about files stored by the file system.
    - Includes details such as file size, inode number, ownership, access/modification times, and more.
2. **System Calls for Metadata**:
    
    - `stat()` and `fstat()` system calls are used to retrieve metadata.
    - These calls fill a `stat` structure with file information.
3. **Structure of `stat`**:
    
    ```c
    struct stat {
        dev_t st_dev;     // ID of device containing file
        ino_t st_ino;     // inode number
        mode_t st_mode;   // protection (permissions)
        nlink_t st_nlink; // number of hard links
        uid_t st_uid;     // user ID of owner
        gid_t st_gid;     // group ID of owner
        dev_t st_rdev;    // device ID (if special file)
        off_t st_size;    // total size, in bytes
        blksize_t st_blksize; // block size for filesystem I/O
        blkcnt_t st_blocks;   // number of blocks allocated
        time_t st_atime;  // time of last access
        time_t st_mtime;  // time of last modification
        time_t st_ctime;  // time of last status change
    };
    ```
    
4. **Command Line Tool `stat`**:
    
    - The `stat` command can be used to view file metadata.
    - Example:
        
        ```bash
        prompt> echo hello > file
        prompt> stat file
        File: ‘file’
        Size: 6          Blocks: 8          IO Block: 4096 regular file
        Device: 811h/2065d   Inode: 67158084   Links: 1
        Access: 2011-05-03 15:50:20.157594748 -0500
        Modify: 2011-05-03 15:50:20.157594748 -0500
        Change: 2011-05-03 15:50:20.157594748 -0500
        ```
        

---

#### **Removing Files**

1. **Deleting Files**:
    
    - Files can be deleted using the `rm` command in UNIX/Linux.
    - Example:
        
        ```bash
        prompt> rm foo
        ```
        
2. **System Call for File Deletion**:
    
    - The `rm` command internally uses the `unlink()` system call to remove files.
    - Example traced output:
        
        ```bash
        prompt> strace rm foo
        ...
        unlink("foo") = 0
        ...
        ```
        
3. **Why `unlink()`?**:
    
    - The term "unlink" is used because files in UNIX are linked to directories via references (links).
    - The `unlink()` system call removes the link between the file and its directory entry.
    - If no other links to the file exist, the file's data is deleted.