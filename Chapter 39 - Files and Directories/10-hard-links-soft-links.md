### Key Notes on Hard Links and Symbolic Links

---

#### **Hard Links**

1. **Definition**:
    
    - A hard link is another name for an existing file, created using the `link()` system call or the `ln` command.
    - Both the original file and the hard link share the same inode number, meaning they refer to the same underlying file data.
2. **Creating Hard Links**:
    
    - Use the `ln` command to create a hard link.
    - Example:
        
        ```bash
        prompt> echo hello > file
        prompt> ln file file2
        prompt> cat file2
        hello
        ```
        
3. **Inode Sharing**:
    
    - Hard links share the same inode number as the original file.
    - Example:
        
        ```bash
        prompt> ls -i file file2
        67158084 file
        67158084 file2
        ```
        
4. **Unlinking and Reference Count**:
    
    - The `unlink()` system call removes the link between a file name and its inode.
    - The file is only deleted when the reference count (link count) in the inode reaches zero.
    - Example:
        
        ```bash
        prompt> echo hello > file
        prompt> stat file
        ... Inode: 67158084 Links: 1 ...
        prompt> ln file file2
        prompt> stat file
        ... Inode: 67158084 Links: 2 ...
        prompt> rm file
        prompt> stat file2
        ... Inode: 67158084 Links: 1 ...
        prompt> rm file2
        ```
        
5. **Key Characteristics**:
    
    - Hard links cannot be created for directories (to avoid cycles in the directory tree).
    - Hard links cannot span across different file systems (as inode numbers are unique only within a file system).

---

#### **Symbolic Links (Soft Links)**

1. **Definition**:
    
    - A symbolic link is a special type of file that contains a reference (pathname) to another file or directory.
    - Unlike hard links, symbolic links do not share the same inode as the original file.
2. **Creating Symbolic Links**:
    
    - Use the `ln -s` command to create a symbolic link.
    - Example:
        
        ```bash
        prompt> echo hello > file
        prompt> ln -s file file2
        prompt> cat file2
        hello
        ```
        
3. **Differences from Hard Links**:
    
    - Symbolic links are a separate file type and have their own inode.
    - They can link to directories and files across different file systems.
    - Symbolic links can become "dangling" if the original file is deleted.
4. **File Type and Size**:
    
    - Symbolic links are identified as a different file type (`l` in `ls -l` output).
    - The size of a symbolic link is the length of the pathname it stores.
    - Example:
        
        ```bash
        prompt> ls -al
        -rw-r----- 1 user user 6 May 3 19:10 file
        lrwxrwxrwx 1 user user 4 May 3 19:10 file2 -> file
        ```
        
5. **Dangling References**:
    
    - If the original file is deleted, the symbolic link points to a non-existent pathname.
    - Example:
        
        ```bash
        prompt> echo hello > file
        prompt> ln -s file file2
        prompt> rm file
        prompt> cat file2
        cat: file2: No such file or directory
        ```
        
6. **Symbolic Link to Longer Pathnames**:
    
    - The size of the symbolic link increases with the length of the pathname it references.
    - Example:
        
        ```bash
        prompt> echo hello > alongerfilename
        prompt> ln -s alongerfilename file3
        prompt> ls -al
        -rw-r----- 1 user user 6 May 3 19:17 alongerfilename
        lrwxrwxrwx 1 user user 15 May 3 19:17 file3 -> alongerfilename
        ```